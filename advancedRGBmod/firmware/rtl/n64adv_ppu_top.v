//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmail.com>
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
input  [46:0] ConfigSet;

input        OSDCLK;
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

wire [1:0] vinfo_pass;  // [3:0] {vmode,n64_480i}
wire palmode, n64_480i;

wire [`VDATA_I_SY_SLICE] vdata_r_sy_0;
wire [`VDATA_I_FU_SLICE] vdata_r[1:3];

wire [16:0] vinfo_mult;
wire nVDSYNC_srgb_out;
wire [`VDATA_O_FU_SLICE] vdata_srgb_out;

wire [`VDATA_O_FU_SLICE] vdata_testpattern;

wire [`VDATA_O_FU_SLICE] vdata_vc_in;
wire [`VDATA_O_FU_SLICE] vdata_vc_out;

wire [3:0] Sync_o;

reg [`VDATA_O_CO_SLICE] vdata_shifted[0:1];
initial begin
  vdata_shifted[0] = {3*color_width_o{1'b0}};
  vdata_shifted[1] = {3*color_width_o{1'b0}};
end

reg [1:2] Filter;
wire AutoFilter_w;

reg        cfg_testpat       = 1'b0;
reg        cfg_exchange_rb_o = 1'b0;
reg [ 2:0] cfg_filter        = 3'b000;
reg        cfg_nEN_YPbPr     = 1'b0;
reg        cfg_nEN_RGsB      = 1'b0;
reg [ 3:0] cfg_gamma         = 4'b0000;
reg        cfg_nvideblur     = 1'b0;
reg        cfg_n15bit_mode   = 1'b0;
reg        cfg_ifix          = 1'b0;
reg [ 1:0] cfg_linemult      = 2'b00;
reg [ 4:0] cfg_SLHyb_str     = 5'b00000;
reg [ 3:0] cfg_SL_str        = 4'b0000;
reg        cfg_SL_method     = 1'b0;
reg        cfg_SL_id         = 1'b0;
reg        cfg_SL_en         = 1'b0;


// apply some assignments
// ----------------------

assign palmode  = vinfo_pass[1];
assign n64_480i = vinfo_pass[0];

assign VCLK_Tx_select = cfg_linemult;

assign vinfo_mult = {cfg_linemult,cfg_ifix,cfg_SLHyb_str,cfg_SL_str,cfg_SL_method,cfg_SL_id,cfg_SL_en,vinfo_pass[1:0]};

assign vdata_vc_in = cfg_testpat ? vdata_testpattern : vdata_srgb_out;

assign Sync_o = vdata_vc_out[`VDATA_O_SY_SLICE];
assign AutoFilter_w = cfg_filter == 3'b000;
assign PPUState = {pal_mode,n64_480i,1'b0,cfg_linemult,~cfg_nEN_YPbPr,~cfg_nEN_RGsB,~cfg_nvideblur,~cfg_n15bit_mode,Filter,AutoFilter_w};


// write configuration register
// ----------------------------

always @(posedge VCLK) begin
  cfg_testpat       <=  ConfigSet[`show_testpattern_bit];
  cfg_exchange_rb_o <=  ConfigSet[`Exchange_RB_out_bit];
  cfg_filter        <=  ConfigSet[`FilterSet_slice];
  cfg_nEN_YPbPr     <= ~ConfigSet[`YPbPr_bit];
  cfg_nEN_RGsB      <= ~ConfigSet[`RGsB_bit];
  cfg_gamma         <=  ConfigSet[`gamma_slice];
  cfg_n15bit_mode   <= ~ConfigSet[`n15bit_mode_bit];
  if (!n64_480i) begin
    cfg_nvideblur     <= ~ConfigSet[`videblur_bit];
    cfg_ifix          <= 1'b0;
    if (palmode | !USE_VPLL)
      cfg_linemult      <= {1'b0,^ConfigSet[`v240p_linemult_slice]}; // do not allow LineX3 in PAL mode or if PLL of VCLK (for LineX3) is not locked (or not used)
    else
      cfg_linemult      <= ConfigSet[`v240p_linemult_slice];
    cfg_SLHyb_str     <= ConfigSet[`v240p_SL_hybrid_slice];
    cfg_SL_str        <= ConfigSet[`v240p_SL_str_slice];
    cfg_SL_method     <= ConfigSet[`v240p_SL_method_bit];
    cfg_SL_id         <= ConfigSet[`v240p_SL_ID_bit];
    cfg_SL_en         <= ConfigSet[`v240p_SL_En_bit];
  end else begin
    cfg_nvideblur     <= 1'b1;
    cfg_ifix          <= ConfigSet[`v480i_field_fix_bit];
    cfg_linemult      <= {1'b0,ConfigSet[`v480i_linex2_bit]};
    if (ConfigSet[`v480i_SL_linked_bit]) begin // check if SL mode is linked to 240p
      cfg_SLHyb_str     <= ConfigSet[`v240p_SL_hybrid_slice];
      cfg_SL_str        <= ConfigSet[`v240p_SL_str_slice];
      cfg_SL_id         <= ConfigSet[`v240p_SL_ID_bit];
    end else begin
      cfg_SLHyb_str     <= ConfigSet[`v480i_SL_hybrid_slice];
      cfg_SL_str        <= ConfigSet[`v480i_SL_str_slice];
      cfg_SL_id         <= ConfigSet[`v480i_SL_ID_bit];
    end
    cfg_SL_method     <= 1'b0;
    cfg_SL_en         <= ConfigSet[`v480i_SL_En_bit];
  end
  if (ConfigSet[`show_testpattern_bit]) // overwrite cfg_linemult if testpattern is enabled
    cfg_linemult <= 2'b00;
end



// get vinfo
// =========

n64_vinfo_ext get_vinfo_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .Sync_pre(vdata_r_sy_0),
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
  .demuxparams_i({palmode,cfg_nvideblur,cfg_n15bit_mode}),
  .vdata_r_sy_0(vdata_r_sy_0),
  .vdata_r_1(vdata_r[1])
);


// OSD Menu Injection
// ==================

osd_injection osd_injection_u(
  .OSDCLK(OSDCLK),
  .OSDWrVector(OSDWrVector),
  .OSDInfo(OSDInfo),
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nVRST(nVRST),
  .video_data_i(vdata_r[1]),
  .video_data_o(vdata_r[2])
);


// Post-Processing
// ===============

// Gamma Correction
// ----------------

gamma_module gamma_module_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .gammaparams_i(cfg_gamma),
  .video_data_i(vdata_r[2]),
  .video_data_o(vdata_r[3])
);


// Line Multiplier
// ---------------

linemult linemult_u(
  .VCLK_Rx(VCLK),
  .nVRST_Rx(nVRST),
  .VCLK_Tx(VCLK_Tx),
  .nVRST_Tx(nVRST_Tx),
  .vinfo_mult(vinfo_mult),
  .vdata_i(vdata_r[3]),
  .vdata_o(vdata_srgb_out)
);


// Test Pattern Generator
// ----------------------

testpattern testpattern_u(
  .VCLK(VCLK_Tx),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST_Tx),
  .Sync_in(VD_i[3:0]),
  .vdata_out(vdata_testpattern)
  .palmode(palmode),
);


// Color Transformation
// --------------------

vconv vconv_u(
  .VCLK(VCLK_Tx),
  .nRST(nVRST_Tx),
  .nEN_YPbPr(cfg_nEN_YPbPr),    // enables color transformation on '0'
  .vdata_i(vdata_vc_in),
  .vdata_o(vdata_vc_out)
);


// Part 7: assign final outputs
// ============================

always @(posedge VCLK_Tx or negedge nVRST_Tx)
  if (!nVRST_Tx) begin
    nCSYNC <= 2'b00;
      VD_o <= {3*color_width_o{1'b0}};
  end else begin
  //  nBLANK <= Sync_o[2];
    nCSYNC[1] <= Sync_o[0];
    if (cfg_nEN_RGsB & cfg_nEN_YPbPr)
      nCSYNC[0] <= 1'b0;
    else
      nCSYNC[0] <= Sync_o[0];

    vdata_shifted[1] <= vdata_shifted[0];
    vdata_shifted[0] <= cfg_exchange_rb_o ? {vdata_vc_out[`VDATA_O_BL_SLICE],vdata_vc_out[`VDATA_O_GR_SLICE],vdata_vc_out[`VDATA_O_RE_SLICE]} : vdata_vc_out[`VDATA_O_CO_SLICE];

    if (!cfg_nvideblur && !cfg_testpat)
      VD_o <= vdata_shifted[^cfg_linemult][`VDATA_O_CO_SLICE];
    else
      VD_o <= cfg_exchange_rb_o ? {vdata_vc_out[`VDATA_O_BL_SLICE],vdata_vc_out[`VDATA_O_GR_SLICE],vdata_vc_out[`VDATA_O_RE_SLICE]} : vdata_vc_out[`VDATA_O_CO_SLICE];

  end


// Filter Add On:
// =============================
//
// Filter setting from NIOS II core:
// - 00: Auto
// - 01: 9.5MHz
// - 10: 18.0MHz
// - 11: Bypassed (i.e. 72MHz)
//
// FILTER 1 | FILTER 2 | DESCRIPTION
// ---------+----------+--------------------
//      0   |     0    |  SD filter ( 9.5MHz)
//      0   |     1    |  ED filter (18.0MHz)
//      1   |     0    |  HD filter (36.0MHz)
//      1   |     1    | FHD filter (72.0MHz)
//
// (Bypass SF is hard wired to 1)

always @(posedge VCLK_Tx or negedge nVRST_Tx)
  if (!nVRST_Tx) begin
    nVSYNC_or_F2 <= 1'b0;
    nHSYNC_or_F1 <= 1'b0;
  end else begin
    Filter <= AutoFilter_w ? cfg_linemult : cfg_filter[1:0] - 1'b1;
    if (UseVGA_HVSync) begin
      nVSYNC_or_F2 <= Sync_o[3];
      nHSYNC_or_F1 <= Sync_o[1];
    end else begin
      nVSYNC_or_F2 <= Filter[2];
      nHSYNC_or_F1 <= Filter[1];
    end
  end

endmodule
