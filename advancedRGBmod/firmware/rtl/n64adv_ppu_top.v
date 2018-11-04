//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2016-2018 by Peter Bartmann <borti4938@gmx.de>
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

input                     VCLK;
input                     nVRST;
input                     nVDSYNC;
input [color_width_i-1:0] VD_i;

output [ 3:0] InfoSet;
input  [47:0] ConfigSet;

input        OSDCLK;
input [24:0] OSDWrVector;
input [ 2:0] OSDInfo;

// output nBLANK;
output reg [3*color_width_o-1:0] VD_o = {3*color_width_o{1'b0}};
output [ 1:0] nCSYNC;

input UseVGA_HVSync;
output nVSYNC_or_F2;
output nHSYNC_or_F1;



// start of rtl


wire [3:0] vinfo_pass;
wire vmode = vinfo_pass[1];
wire n64_480i = vinfo_pass[0];

// general structure of ConfigSet
// [47:40] {show_testpattern,(3bits reserve),FilterSet (2bit),YPbPr,RGsB}
// [39:32] {gamma (4bits),(1bit reserve),VI-DeBlur (2bit), 15bit mode}
// [31:16] {(2bits reserve),lineX2,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_Method,Sl_ID,Sl_En}
// [15: 0] {(2bits reserve),lineX2,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_link,Sl_ID,Sl_En}

wire       cfg_testpat   =  ConfigSet[47];
wire       cfg_OSD_SL    =  ConfigSet[46];
wire [1:0] FilterSetting =  ConfigSet[43:42];
wire       cfg_nEN_YPbPr = ~ConfigSet[41];
wire       cfg_nEN_RGsB  = ~ConfigSet[40];
wire [3:0] cfg_gamma     =  ConfigSet[39:36];
wire       nDeBlurMan    = ~ConfigSet[34];
wire       nForceDeBlur  = ~|ConfigSet[34:33];
wire       n15bit_mode   = ~ConfigSet[32];
wire       cfg_lineX2    =    ~n64_480i ? ConfigSet[29   ] : ConfigSet[13   ];
wire [4:0] cfg_SLHyb_str =    ~n64_480i ? ConfigSet[28:24] :
                           ConfigSet[2] ? ConfigSet[28:24] : ConfigSet[12: 8];
wire [3:0] cfg_SL_str    =    ~n64_480i ? ConfigSet[23:20] :
                           ConfigSet[2] ? ConfigSet[23:20] : ConfigSet[ 7: 4];
wire       cfg_SL_method =    ~n64_480i ? ConfigSet[18   ] : 1'b0;
wire       cfg_SL_id     =    ~n64_480i ? ConfigSet[17   ] :
                           ConfigSet[2] ? ConfigSet[17   ] : ConfigSet[ 1   ] ;
wire       cfg_SL_en     =    ~n64_480i ? ConfigSet[16   ] : ConfigSet[ 0   ] ;


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
  .deblurparams_i({vinfo_pass,nForceDeBlur,nDeBlurMan}),
  .ndo_deblur(ndo_deblur)
);


// Part 3: data demux
// ==================

n64a_vdemux video_demux_u(
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .nRST(nVRST),
  .VD_i(VD_i),
  .demuxparams_i({vinfo_pass[3:1],ndo_deblur,n15bit_mode}),
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

wire nENABLE_linedbl = ~cfg_lineX2 | ~nVRST;

wire [15:0] vinfo_dbl = {nENABLE_linedbl,cfg_OSD_SL,cfg_SLHyb_str,cfg_SL_str,cfg_SL_method,cfg_SL_id,cfg_SL_en,vinfo_pass[1:0]};

wire [`VDATA_O_FU_SLICE] vdata_srgb_out;

linedoubler linedoubler_u(
  .VCLK(VCLK),
  .nRST(nVRST),
  .vinfo_dbl(vinfo_dbl),
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
  .vmode(vmode),
  .Sync_in(VD_i[3:0]),
  .vdata_out(vdata_testpattern)
);


// (continue with part 5)
// Part 5.3: Color Transformation
// ==============================

wire [`VDATA_O_FU_SLICE] vdata_vc_in = cfg_testpat ? vdata_testpattern : vdata_srgb_out;
wire [`VDATA_O_FU_SLICE] vdata_vc_out;

vconv vconv_u(
  .VCLK(VCLK),
  .nRST(nVRST),
  .nEN_YPbPr(cfg_nEN_YPbPr),    // enables color transformation on '0'
  .vdata_i(vdata_vc_in),
  .vdata_o(vdata_vc_out)
);

// Part 7: assign final outputs
// ============================

reg [3:0] Sync_o = 4'b0;
reg [`VDATA_O_CO_SLICE] vdata_shifted[0:1];
initial begin
  vdata_shifted[0] = {3*color_width_o{1'b0}};
  vdata_shifted[1] = {3*color_width_o{1'b0}};
end

always @(posedge VCLK) begin
  Sync_o <= vdata_vc_out[`VDATA_O_SY_SLICE];

  vdata_shifted[1] <= vdata_shifted[0];
  vdata_shifted[0] <= vdata_vc_out[`VDATA_O_CO_SLICE];

  if (!ndo_deblur && !cfg_testpat)
    VD_o <= vdata_shifted[nENABLE_linedbl][`VDATA_O_CO_SLICE];
  else
    VD_o <= vdata_vc_out[`VDATA_O_CO_SLICE];

  if (!nVRST) begin
    Sync_o <= 4'b0;
      VD_o <= {3*color_width_o{1'b0}};

    vdata_shifted[0] <= {3*color_width_o{1'b0}};
    vdata_shifted[1] <= {3*color_width_o{1'b0}};
  end
end

assign InfoSet = {vinfo_pass[1:0],~ndo_deblur,UseVGA_HVSync};

// assign nBLANK = Sync_o[2];
wire nCSYNC_ADV712x = cfg_nEN_RGsB & cfg_nEN_YPbPr ? 1'b0  : Sync_o[0];
assign nCSYNC       = {Sync_o[0],nCSYNC_ADV712x};

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

always @(posedge VCLK)
  Filter <= FilterSetting   == 2'b11 ? 2'b11 :        // bypassed
            FilterSetting   == 2'b10 ? 2'b01 :        // 18.0MHz
            FilterSetting   == 2'b01 ? 2'b00 :        // 9.5MHz
            nENABLE_linedbl == 1'b0  ? 2'b01 : 2'b00; // Auto (18.0MHz in LineX2 and 9.5MHz else)


assign nVSYNC_or_F2 = UseVGA_HVSync ? Sync_o[3] : Filter[2];
assign nHSYNC_or_F1 = UseVGA_HVSync ? Sync_o[1] : Filter[1];

endmodule
