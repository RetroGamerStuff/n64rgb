//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2019 by Peter Bartmann <borti4938@gmail.com>
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
  InfoSet,
  ConfigSet,

  OSDCLK,
  OSDWrVector,
  OSDInfo,

  // possible VCLKs for the output
  VCLK_Tx,
  nVRST_Tx,

  // Video Output
  VCLK_o,
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

input VCLK;
input nVRST;
input nVDSYNC;
input [color_width_i-1:0] VD_i;

output [ 3:0] InfoSet;
input  [47:0] ConfigSet;

input        OSDCLK;
input [24:0] OSDWrVector;
input [ 1:0] OSDInfo;

input [1:0] VCLK_Tx;
input [1:0] nVRST_Tx;

output VCLK_o;
// output reg nBLANK = 1'b0;
output reg [3*color_width_o-1:0] VD_o = {3*color_width_o{1'b0}};
output reg [                1:0] nCSYNC = 2'b00;

input UseVGA_HVSync;
output reg nVSYNC_or_F2 = 1'b0;
output reg nHSYNC_or_F1 = 1'b0;



// start of rtl


wire [3:0] vinfo_pass;
wire pal_mode = vinfo_pass[1];
wire n64_480i = vinfo_pass[0];

// general structure of ConfigSet
// [47:40] {show_testpattern,(2bits reserve),FilterSet (3bits),YPbPr,RGsB}
// [39:32] {gamma (4bits),(1bit reserve),VI-DeBlur (2bit), 15bit mode}
// [31:16] 240p: {(1bit reserve),linemult (2bits),Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_Method,Sl_ID,Sl_En}
// [15: 0] 480i: {(1bit reserve),field_fix,bob_deint.,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_link,Sl_ID,Sl_En}

reg       cfg_testpat      = 1'b0;
reg [2:0] cfg_filter       = 3'b000;
reg       cfg_nEN_YPbPr    = 1'b0;
reg       cfg_nEN_RGsB     = 1'b0;
reg [3:0] cfg_gamma        = 4'b0000;
reg       cfg_ndeblurman   = 1'b0;
reg       cfg_nforcedeblur = 1'b0;
reg       cfg_n15bit_mode  = 1'b0;
reg       cfg_ifix         = 1'b0;
reg [1:0] cfg_linemult     = 2'b00;
reg [4:0] cfg_SLHyb_str    = 5'b00000;
reg [3:0] cfg_SL_str       = 4'b0000;
reg       cfg_SL_method    = 1'b0;
reg       cfg_SL_id        = 1'b0;
reg       cfg_SL_en        = 1'b0;

always @(posedge VCLK) begin
  cfg_testpat      <=  ConfigSet[47];
  cfg_filter       <=  ConfigSet[44:42];
  cfg_nEN_YPbPr    <= ~ConfigSet[41];
  cfg_nEN_RGsB     <= ~ConfigSet[40];
  cfg_gamma        <=  ConfigSet[39:36];
  cfg_ndeblurman   <= ~ConfigSet[34];
  cfg_nforcedeblur <= ~|ConfigSet[34:33];
  cfg_n15bit_mode  <= ~ConfigSet[32];
  if (!n64_480i) begin
    cfg_ifix         <= 1'b0;
    if (pal_mode)
      cfg_linemult     <= {1'b0,^ConfigSet[30:29]}; // do not allow LineX3 in PAL mode
    else
      cfg_linemult     <= ConfigSet[30:29];
    cfg_SLHyb_str    <= ConfigSet[28:24];
    cfg_SL_str       <= ConfigSet[23:20];
    cfg_SL_method    <= ConfigSet[18   ];
    cfg_SL_id        <= ConfigSet[17   ];
    cfg_SL_en        <= ConfigSet[16   ];
  end else begin
    cfg_ifix         <= ConfigSet[14];
    cfg_linemult     <= {1'b0,ConfigSet[13]};
    if (ConfigSet[2]) begin // check if SL mode is linked to 240p
      cfg_SLHyb_str    <= ConfigSet[28:24];
      cfg_SL_str       <= ConfigSet[23:20];
      cfg_SL_id        <= ConfigSet[17   ];
    end else begin
      cfg_SLHyb_str    <= ConfigSet[12: 8];
      cfg_SL_str       <= ConfigSet[ 7: 4];
      cfg_SL_id        <= ConfigSet[ 1   ];
    end
    cfg_SL_method    <= 1'b0;
    cfg_SL_en        <= ConfigSet[ 0   ];
  end
end


wire [`VDATA_I_FU_SLICE] vdata_r[0:3];

// Part 1: get all of the vinfo needed for further processing
// ==========================================================

n64_vinfo_ext get_vinfo_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .Sync_pre(vdata_r[0][`VDATA_I_SY_SLICE]),
  .Sync_cur(VD_i[3:0]),
  .vinfo_o(vinfo_pass)
);


// Part 2: DeBlur Management (incl. heuristic)
// ===========================================

wire ndo_deblur;

n64_deblur deblur_management_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .vdata_pre(vdata_r[0]),
  .VD_i(VD_i),
  .deblurparams_i({vinfo_pass,cfg_nforcedeblur,cfg_ndeblurman}),
  .ndo_deblur(ndo_deblur)
);


// Part 3: data demux
// ==================

n64a_vdemux video_demux_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .VD_i(VD_i),
  .demuxparams_i({vinfo_pass[3:1],ndo_deblur,cfg_n15bit_mode}),
  .vdata_r_0(vdata_r[0]),
  .vdata_r_1(vdata_r[1])
);

// Part 4: OSD Menu Injection
// ==========================

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

// Part 5: Post-Processing
// =======================

// Part 5.1: Gamma Correction
// ==========================

gamma_module gamma_module_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .gammaparams_i(cfg_gamma),
  .video_data_i(vdata_r[2]),
  .video_data_o(vdata_r[3])
);

// Part 5.2: Line Multiplier
// =========================

wire [17:0] vinfo_mult = {cfg_linemult,cfg_ifix,cfg_SLHyb_str,cfg_SL_str,cfg_SL_method,cfg_SL_id,cfg_SL_en,vinfo_pass[1:0]};

wire VCLK_Tx_o_pre, nVRST_Tx_o_pre;
wire [`VDATA_O_FU_SLICE] vdata_srgb_out;

linemult linemult_u(
  .VCLK_Rx(VCLK),
  .nVRST_Rx(nVRST),
  .VCLK_Tx(VCLK_Tx),
  .nVRST_Tx(nVRST_Tx),
  .VCLK_o(VCLK_Tx_o_pre),
  .nVRST_o(nVRST_Tx_o_pre),
  .vinfo_mult(vinfo_mult),
  .vdata_i(vdata_r[3]),
  .vdata_o(vdata_srgb_out)
);


// Part 6: Test Pattern Generator
// ==============================
// (intersects part 5)

wire [`VDATA_O_FU_SLICE] vdata_testpattern;

testpattern testpattern_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .vmode(pal_mode),
  .Sync_in(VD_i[3:0]),
  .vdata_out(vdata_testpattern)
);


// (continue with part 5)
// Part 5.3: Color Transformation
// ==============================

wire VCLK_Tx_o;
vclk_tx_post_testpattern_mux vclk_tx_post_testpattern_mux_u(
  .data1(VCLK),
  .data0(VCLK_Tx_o_pre),
  .sel(cfg_testpat),
  .result(VCLK_Tx_o)
);

reg [2:0] cfg_testpat_buf = 3'b000;
reg [7:0] hold_nVRST_Tx_o = 8'h0;
reg nVRST_Tx_o;
always @(posedge VCLK_Tx_o) begin
  if (~|hold_nVRST_Tx_o) begin
    nVRST_Tx_o <= cfg_testpat ? nVRST : nVRST_Tx_o_pre;
  end else begin
    nVRST_Tx_o <= 1'b0;
    hold_nVRST_Tx_o <= hold_nVRST_Tx_o - 1'b1;
  end
  if (^cfg_testpat_buf[2:1])
    hold_nVRST_Tx_o <= 8'hff;
  cfg_testpat_buf[2] <= cfg_testpat_buf[1];
  cfg_testpat_buf[1] <= cfg_testpat_buf[0];
  cfg_testpat_buf[0] <= cfg_testpat;
end

wire [`VDATA_O_FU_SLICE] vdata_vc_in = cfg_testpat ? vdata_testpattern : vdata_srgb_out;
wire [`VDATA_O_FU_SLICE] vdata_vc_out;

vconv vconv_u(
  .VCLK(VCLK_Tx_o),
  .nRST(nVRST_Tx_o),
  .nEN_YPbPr(cfg_nEN_YPbPr),    // enables color transformation on '0'
  .vdata_i(vdata_vc_in),
  .vdata_o(vdata_vc_out)
);

// Part 7: assign final outputs
// ============================

wire [3:0] Sync_o = vdata_vc_out[`VDATA_O_SY_SLICE];
reg [`VDATA_O_CO_SLICE] vdata_shifted[0:1];
initial begin
  vdata_shifted[0] = {3*color_width_o{1'b0}};
  vdata_shifted[1] = {3*color_width_o{1'b0}};
end

always @(posedge VCLK_Tx_o) begin
//  nBLANK <= Sync_o[2];
  nCSYNC[1] <= Sync_o[0];
  if (cfg_nEN_RGsB & cfg_nEN_YPbPr)
    nCSYNC[0] <= 1'b0;
  else
    nCSYNC[0] <= Sync_o[0];

  vdata_shifted[1] <= vdata_shifted[0];
  vdata_shifted[0] <= vdata_vc_out[`VDATA_O_CO_SLICE];

  if (!ndo_deblur && !cfg_testpat)
    VD_o <= vdata_shifted[^cfg_linemult][`VDATA_O_CO_SLICE];
  else
    VD_o <= vdata_vc_out[`VDATA_O_CO_SLICE];

  if (!nVRST_Tx_o) begin
    nCSYNC <= 2'b00;
      VD_o <= {3*color_width_o{1'b0}};

    vdata_shifted[0] <= {3*color_width_o{1'b0}};
    vdata_shifted[1] <= {3*color_width_o{1'b0}};
  end
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

reg [1:2] Filter;

always @(posedge VCLK_Tx_o) begin
  Filter <= cfg_filter == 3'b000 ? cfg_linemult : cfg_filter[1:0] - 1'b1;
  
  if (UseVGA_HVSync) begin
    nVSYNC_or_F2 <= Sync_o[3];
    nHSYNC_or_F1 <= Sync_o[1];
  end else begin
    nVSYNC_or_F2 <= Filter[2] & !Filter[1];
    nHSYNC_or_F1 <= Filter[1] & !Filter[2];
  end

  if (!nVRST_Tx_o) begin
    nVSYNC_or_F2 <= 1'b0;
    nHSYNC_or_F1 <= 1'b0;
  end
end


// final assignments with register

assign InfoSet = {vinfo_pass[1:0],~ndo_deblur,UseVGA_HVSync};
assign VCLK_o = VCLK_Tx_o;

endmodule
