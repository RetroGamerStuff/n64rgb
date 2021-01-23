//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2021 by Peter Bartmann <borti4938@gmail.com>
//
// N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////////
//
// Company:  Circuit-Board.de
// Engineer: borti4938
//
// Module Name:    n64adv_ppu_top
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV:    EP4CE10E22
//                 Cyclone 10 LP: 10CL010YE144
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


module n64adv_ppu_top (
  // N64 Video Input
  VCLK,
  nVRST,
  nVDSYNC,
  VD_i,

  // Misc Information Exchange
  PPUState,
  ConfigSet,

  OSDCLK,
  OSD_VSync,
  OSDWrVector,
  OSDInfo,

  // VCLK for video output
  USE_VPLL,
  VCLK_Tx_select,
  VCLK_Tx,
  nVRST_Tx,

  // Video Output
//   nBLANK,
  VD_o,
  nCSYNC, // nCSYNC and nCSYNC for ADV712x

  // Jumper VGA Sync / Filter AddOn
  UseVGA_HVSync,
  nVSYNC_or_F2,
  nHSYNC_or_F1
);


`include "vh/n64adv_cparams.vh"
`include "vh/n64adv_vparams.vh"
`include "vh/n64adv_ppuconfig.vh"

input VCLK;
input nVRST;
input nVDSYNC;
input [color_width_i-1:0] VD_i;

output [11:0] PPUState;
input  [68:0] ConfigSet;

input        OSDCLK;
output       OSD_VSync;
input [24:0] OSDWrVector;
input [ 1:0] OSDInfo;

input        USE_VPLL;
output [1:0] VCLK_Tx_select;
input        VCLK_Tx;
input        nVRST_Tx;

// output reg nBLANK = 1'b0;
output reg [`VDATA_O_CO_SLICE] VD_o = {3*color_width_o{1'b0}};
output reg [              1:0] nCSYNC = 2'b00;

input UseVGA_HVSync;
output reg nVSYNC_or_F2 = 1'b0;
output reg nHSYNC_or_F1 = 1'b0;



// start of rtl

wire [1:0] vinfo_pass;  // [1:0] {vmode,n64_480i}
wire palmode, n64_480i;

wire [68:0] ConfigSet_resynced;

wire vdata_valid_w[0:2];
wire [`VDATA_I_SY_SLICE] vdata_w_sy_0;
wire [`VDATA_I_FU_SLICE] vdata_w[1:2];

wire vdata_tp_valid_w;
wire [`VDATA_I_FU_SLICE] vdata_tp_w;

wire vdata_valid_pp_w[0:3];
wire [`VDATA_I_FU_SLICE] vdata21_pp_w[0:1];
wire [`VDATA_O_FU_SLICE] vdata24_pp_w[2:3];
wire [3:0] Sync_pp_o;

wire [20:0] vinfo_mult;
wire [13:0] linex_timing;

wire AutoFilter_w;


reg [ 3:0] cfg_gamma;
reg cfg_testpat, cfg_nvideblur_0, cfg_n16bit_mode;

reg cfg_exchange_rb_o, cfg_nEN_YPbPr, cfg_nEN_RGsB, cfg_nvideblur_1;
reg cfg_ifix, cfg_SL_method, cfg_SL_id, cfg_SL_en;
reg [ 2:0] cfg_filter;
reg [ 1:0] cfg_linemult;
reg [ 4:0] cfg_SLHyb_str;
reg [ 7:0] cfg_SL_str;
reg cfg_dejitter;
reg [ 6:0] cfg_linex_hshift;
reg [ 5:0] cfg_linex_vshift;

reg [1:2] Filter;



// apply some assignments
// ----------------------

assign palmode  = vinfo_pass[1];
assign n64_480i = vinfo_pass[0];

assign VCLK_Tx_select = cfg_linemult;

assign vinfo_mult = {cfg_linemult,cfg_ifix,cfg_SLHyb_str,cfg_SL_str,cfg_SL_method,cfg_SL_id,cfg_SL_en,palmode,n64_480i};
assign linex_timing = {cfg_dejitter,cfg_linex_hshift,cfg_linex_vshift};

assign vdata_valid_pp_w[0] = cfg_testpat ? vdata_tp_valid_w : vdata_valid_w[2];
assign vdata21_pp_w[0] = cfg_testpat ? vdata_tp_w : vdata_w[2];

assign Sync_pp_o = vdata24_pp_w[3][`VDATA_O_SY_SLICE];
assign AutoFilter_w = cfg_filter == 3'b000;

assign PPUState = {palmode,n64_480i,1'b0,cfg_linemult,~cfg_nEN_YPbPr,~cfg_nEN_RGsB,~cfg_nvideblur_1,~cfg_n16bit_mode,Filter,AutoFilter_w};


// write configuration register
// ----------------------------

always @(posedge VCLK) begin
  cfg_testpat     <=  ConfigSet[`show_testpattern_bit];
  cfg_gamma       <=  ConfigSet[`gamma_slice];
  cfg_n16bit_mode <= ~ConfigSet[`n16bit_mode_bit];
  if (!n64_480i) begin
    cfg_nvideblur_0 <= ~ConfigSet[`videblur_bit];
  end else begin
    cfg_nvideblur_0 <= 1'b1;
  end
end

register_sync #(
  .reg_width(69),
  .reg_preset(69'd0)
) sync4txlogic_u(
  .clk(VCLK_Tx),
  .clk_en(1'b1),
  .nrst(1'b1),
  .reg_i(ConfigSet),
  .reg_o(ConfigSet_resynced)
);

always @(posedge VCLK_Tx) begin
  cfg_exchange_rb_o    <=  ConfigSet_resynced[`Exchange_RB_out_bit];
  cfg_filter           <=  ConfigSet_resynced[`FilterSet_slice];
  cfg_nEN_YPbPr        <= ~ConfigSet_resynced[`YPbPr_bit];
  cfg_nEN_RGsB         <= ~ConfigSet_resynced[`RGsB_bit];
  cfg_dejitter         <= palmode & ConfigSet_resynced[`pal_dejitter_bit];
  cfg_linex_hshift     <= ConfigSet_resynced[`linex_hshift_slice];
  cfg_linex_vshift     <= ConfigSet_resynced[`linex_vshift_slice];
  if (!n64_480i) begin
    cfg_nvideblur_1      <= ~ConfigSet_resynced[`videblur_bit];
    cfg_ifix             <= 1'b0;
    if (palmode | !USE_VPLL)
      cfg_linemult         <= {1'b0,^ConfigSet_resynced[`v240p_linemult_slice]}; // do not allow LineX3 in PAL mode or if PLL of VCLK (for LineX3) is not locked (or not used)
    else
      cfg_linemult         <= ConfigSet_resynced[`v240p_linemult_slice];
    cfg_SLHyb_str        <= ConfigSet_resynced[`v240p_SL_hybrid_slice];
    cfg_SL_str           <= ((ConfigSet_resynced[`v240p_SL_str_slice]+8'h01)<<4)-1'b1;
    cfg_SL_method        <= ConfigSet_resynced[`v240p_SL_method_bit];
    cfg_SL_id            <= ConfigSet_resynced[`v240p_SL_ID_bit];
    cfg_SL_en            <= ConfigSet_resynced[`v240p_SL_En_bit];
  end else begin
    cfg_nvideblur_1      <= 1'b1;
    cfg_ifix             <= ConfigSet_resynced[`v480i_field_fix_bit];
    cfg_linemult         <= {1'b0,ConfigSet_resynced[`v480i_linex2_bit]};
    if (ConfigSet_resynced[`v480i_SL_linked_bit]) begin // check if SL mode is linked to 240p
      cfg_SLHyb_str        <= ConfigSet_resynced[`v240p_SL_hybrid_slice];
      cfg_SL_str           <= ((ConfigSet_resynced[`v240p_SL_str_slice]+8'h01)<<4)-1'b1;
      cfg_SL_str           <= ConfigSet_resynced[`v240p_SL_str_slice];
      cfg_SL_id            <= ConfigSet_resynced[`v240p_SL_ID_bit];
    end else begin
      cfg_SLHyb_str        <= ConfigSet_resynced[`v480i_SL_hybrid_slice];
      cfg_SL_str           <= ((ConfigSet_resynced[`v480i_SL_str_slice]+8'h01)<<4)-1'b1;
      cfg_SL_id            <= ConfigSet_resynced[`v480i_SL_ID_bit];
    end
    cfg_SL_method        <= 1'b0;
    cfg_SL_en            <= ConfigSet_resynced[`v480i_SL_En_bit];
  end
end



// get vinfo
// =========

n64_vinfo_ext get_vinfo_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .Sync_pre(vdata_w_sy_0),
  .Sync_cur(VD_i[3:0]),
  .vinfo_o(vinfo_pass)
);


// video data demux
// ================

n64a_vdemux video_demux_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .VD_i(VD_i),
  .demuxparams_i({palmode,cfg_nvideblur_0,cfg_n16bit_mode}),
  .vdata_valid_0(vdata_valid_w[0]),
  .vdata_r_sy_0(vdata_w_sy_0),
  .vdata_valid_1(vdata_valid_w[1]),
  .vdata_r_1(vdata_w[1])
);


// OSD Menu Injection
// ==================

osd_injection osd_injection_u(
  .OSDCLK(OSDCLK),
  .OSD_VSync(OSD_VSync),
  .OSDWrVector(OSDWrVector),
  .OSDInfo(OSDInfo),
  .VCLK(VCLK),
  .nVRST(nVRST),
  .vdata_valid_i(vdata_valid_w[1]),
  .vdata_i(vdata_w[1]),
  .vdata_valid_o(vdata_valid_w[2]),
  .vdata_o(vdata_w[2])
);

// Test Pattern Generator
// ======================

testpattern testpattern_u(
  .VCLK(VCLK),
  .nRST(nVRST),
  .palmode(palmode),
  .vdata_sync_valid_i(vdata_valid_w[0]),
  .vdata_sync_i(vdata_w_sy_0),
  .vdata_valid_o(vdata_tp_valid_w),
  .vdata_o(vdata_tp_w)
);


// Post-Processing
// ===============

// Gamma Correction
// ----------------

gamma_module gamma_module_u(
  .VCLK(VCLK),
  .nRST(nVRST),
  .gammaparams_i(cfg_gamma),
  .vdata_valid_i(vdata_valid_pp_w[0]),
  .vdata_i(vdata21_pp_w[0]),
  .vdata_valid_o(vdata_valid_pp_w[1]),
  .vdata_o(vdata21_pp_w[1])
);


// Line Multiplier
// ---------------

linemult linemult_u(
  .VCLK_i(VCLK),
  .nVRST_i(nVRST),
  .vdata_valid_i(vdata_valid_pp_w[1]),
  .vdata_i(vdata21_pp_w[1]),
  .vinfo_mult(vinfo_mult),
  .linex_timing(linex_timing),
  .VCLK_o(VCLK_Tx),
  .nVRST_o(nVRST_Tx),
  .vdata_valid_o(vdata_valid_pp_w[2]),
  .vdata_o(vdata24_pp_w[2])
);



// Color Transformation
// --------------------

vconv vconv_u(
  .VCLK(VCLK_Tx),
  .nRST(nVRST_Tx),
  .nEN_YPbPr(cfg_nEN_YPbPr),  // enables color transformation on '0'
  .vdata_valid_i(vdata_valid_pp_w[2]),
  .vdata_i(vdata24_pp_w[2]),
  .vdata_valid_o(vdata_valid_pp_w[3]),
  .vdata_o(vdata24_pp_w[3])
);


// Part 7: assign final outputs
// ============================
//
// Filter AddOn Notes:
// Filter setting from NIOS II core:
// - 000: Auto
// - 001: 9.5MHz
// - 010: 18.0MHz
// - 011: 36.0MHz
// - 100: Bypassed (i.e. 72MHz on non-flex)
//
// FILTER 1 | FILTER 2 | DESCRIPTION
// ---------+----------+--------------------
//      0   |     0    |  SD filter ( 9.5MHz)
//      0   |     1    |  ED filter (18.0MHz)
//      1   |     0    |  HD filter (36.0MHz)
//      1   |     1    | FHD filter (72.0MHz)
//
// (Bypass SF is hard wired to 1)
//

always @(posedge VCLK_Tx or negedge nVRST_Tx)
  if (!nVRST_Tx) begin
    nVSYNC_or_F2 <= 1'b0;
    nHSYNC_or_F1 <= 1'b0;
    nCSYNC <= 2'b00;
    VD_o <= {3*color_width_o{1'b0}};
  end else begin
    Filter <= AutoFilter_w ? cfg_linemult : cfg_filter[1:0] - 1'b1;
    if (UseVGA_HVSync) begin
      if (vdata_valid_pp_w[3]) begin
        nVSYNC_or_F2 <= Sync_pp_o[3];
        nHSYNC_or_F1 <= Sync_pp_o[1];
      end
    end else begin
      nVSYNC_or_F2 <= Filter[2];
      nHSYNC_or_F1 <= Filter[1];
    end
    
    if (vdata_valid_pp_w[3]) begin
    //  nBLANK <= Sync_pp_o[2];
      nCSYNC[1] <= Sync_pp_o[0];
      if (cfg_nEN_RGsB & cfg_nEN_YPbPr)
        nCSYNC[0] <= 1'b0;
      else
        nCSYNC[0] <= Sync_pp_o[0];
      VD_o <= cfg_exchange_rb_o ? {vdata24_pp_w[3][`VDATA_O_BL_SLICE],vdata24_pp_w[3][`VDATA_O_GR_SLICE],vdata24_pp_w[3][`VDATA_O_RE_SLICE]} : vdata24_pp_w[3][`VDATA_O_CO_SLICE];
    end;
  end

endmodule
