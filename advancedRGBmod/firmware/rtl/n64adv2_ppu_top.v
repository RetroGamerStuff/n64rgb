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
// Module Name:    n64adv2_ppu_top
// Project Name:   N64 Advanced HMDI Mod
// Target Devices: Cyclone 10 LP: 10CL025YE144
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies:
// (more dependencies may appear in other files)
//
// Revision:
// Features: see repository readme
//
//////////////////////////////////////////////////////////////////////////////////




module n64adv2_ppu_top (
  // N64 Video Input
  VCLK,
  nVDSYNC,
  nVRST,
  VD_i,

  // Misc Information Exchange
  InfoSet,
  ConfigSet,

  OSDCLK,
  OSDWrVector,
  OSDInfo,

  VCLK_Tx,
  nVRST_Tx,

  // Video Output to ADV7513
  VSYNC_o,
  HSYNC_o,
  VD_o
);


`include "vh/n64adv_cparams.vh"
`include "vh/n64adv_vparams.vh"

input VCLK;
input nVDSYNC;
input nVRST;
input [color_width_i-1:0] VD_i;

output [ 2:0] InfoSet;
input  [47:0] ConfigSet;

input        OSDCLK;
input [24:0] OSDWrVector;
input [ 1:0] OSDInfo;

input VCLK_Tx;
input nVRST_Tx;
output reg VSYNC_o = 1'b0;
output reg HSYNC_o = 1'b0;
output reg [3*color_width_o-1:0] VD_o = {3*color_width_o{1'b0}};



// start of rtl

wire vmode    = vinfo_pass[1];
wire n64_480i = vinfo_pass[0];

// ConfigSet:
// general structure [47:32] video settings, [31:16] 240p settings, [15:0] 480i settings
// [47:40] {(1bit reserved) ,sl_in_osd,(6bits reserved)}
// [39:32] {gamma (4bits),(1bit reserve),VI-DeBlur (2bit), 15bit mode}
// [31:16] {(2bits reserve),lineX2,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_Method,Sl_ID,Sl_En}
// [15: 0] {(1bits reserve),lineX2 (2bits),Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_link,Sl_ID,Sl_En}
wire       cfg_OSD_SL    =   ConfigSet[   46];
wire [3:0] cfg_gamma     =   ConfigSet[39:36];
wire       nDeBlurMan    =  ~ConfigSet[   34];
wire       nForceDeBlur  = ~|ConfigSet[34:33];
wire       n15bit_mode   =  ~ConfigSet[   32];
wire       cfg_lineX2    =    !n64_480i ? ConfigSet[29   ] : |ConfigSet[14:13];
wire       cfg_lX2_ifix  = !ConfigSet[13];
wire [4:0] cfg_SLHyb_str =    !n64_480i ? ConfigSet[28:24] :
                           ConfigSet[2] ? ConfigSet[28:24] :  ConfigSet[12: 8];
wire [3:0] cfg_SL_str    =    !n64_480i ? ConfigSet[23:20] :
                           ConfigSet[2] ? ConfigSet[23:20] :  ConfigSet[ 7: 4];
wire       cfg_SL_method =    !n64_480i ? ConfigSet[18   ] :  1'b0;
wire       cfg_SL_id     =    !n64_480i ? ConfigSet[17   ] :
                           ConfigSet[2] ? ConfigSet[17   ] :  ConfigSet[ 1   ] ;
wire       cfg_SL_en     =    !n64_480i ? ConfigSet[16   ] :  ConfigSet[ 0   ] ;


wire [`VDATA_I_FU_SLICE] vdata_r[0:3];



// Part 1: get all of the vinfo needed for further processing
// ==========================================================

wire [3:0] vinfo_pass;

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

wire [16:0] vinfo_dbl = {nENABLE_linedbl,cfg_lX2_ifix,cfg_OSD_SL,cfg_SLHyb_str,cfg_SL_str,cfg_SL_method,cfg_SL_id,cfg_SL_en,vinfo_pass[1:0]};

wire [`VDATA_O_FU_SLICE] vdata_srgb_out;

scaler scaler_u(
  .VCLK(VCLK),
  .nRST(nVRST),
  .VCLK_Tx(VCLK_Tx),
  .nRST_Tx(nVRST_Tx),
  .vinfo_dbl(vinfo_dbl),
  .vdata_i(vdata_r[3]),
  .vdata_o(vdata_srgb_out)
);


// Part 6: assign final outputs
// ============================

reg [`VDATA_O_CO_SLICE] vdata_shifted[0:1];

initial begin
  vdata_shifted[0] = {3*color_width_o{1'b0}};
  vdata_shifted[1] = {3*color_width_o{1'b0}};
end

always @(posedge VCLK_Tx) begin
  VSYNC_o <= vdata_srgb_out[3*color_width_o+3];
  HSYNC_o <= vdata_srgb_out[3*color_width_o+1];

  vdata_shifted[1] <= vdata_shifted[0];
  vdata_shifted[0] <= vdata_srgb_out[`VDATA_O_CO_SLICE];

  if (!ndo_deblur)
    VD_o <= vdata_shifted[nENABLE_linedbl];
  else
    VD_o <= vdata_srgb_out[`VDATA_O_CO_SLICE];

  if (!nVRST_Tx) begin
    VSYNC_o <= 1'b0;
    HSYNC_o <= 1'b0;
      VD_o <= {3*color_width_o{1'b0}};

    vdata_shifted[0] <= {3*color_width_o{1'b0}};
    vdata_shifted[1] <= {3*color_width_o{1'b0}};
  end
end

assign InfoSet = {vmode,n64_480i,~ndo_deblur};

endmodule
