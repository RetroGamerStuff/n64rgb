//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2016-2018 by Peter Bartmann <borti4938@gmail.com>
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
// Company: Circuit-Board.de
// Engineer: borti4938
// (initial design file by Ikari_01)
//
// Module Name:    n64rgbv2_top
// Project Name:   N64 RGB DAC Mod
// Target Devices: several MaxII & MaxV devices
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies: rtl/n64igr.v (Rev. 2.5)
//               rtl/n64_vinfo_ext.v  (Rev. 1.0)
//               rtl/n64_deblur.v     (Rev. 1.1)
//               rtl/n64_vdemux.v     (Rev. 1.0)
//               vh/n64rgb_params.vh
//
// Revision: 2.6
// Features: BUFFERED version (no color shifting around edges)
//           deblur (with heuristic) and 15bit mode (5bit for each color); defaults for IGR can be set as follows:
//             - heuristic deblur:   on (default)                               | off (set pin 1 to GND / short pin 1 & 2)
//             - deblur default:     on (default)                               | off (set pin 91 to GND / short pin 91 & 90)
//               (deblur deafult only comes into account if heuristic is switched off)
//             - 15bit mode default: on (set pin 36 to GND / short pin 36 & 37) | off (default)
//           controller input detection for switching de-blur and 15bit mode
//           resetting N64 using the controller
//           defaults of de-blur and 15bit mode are set on power cycle
//           if de-blur heuristic is overridden by user, it is reset on each power cycle and on each reset
//           selectable installation type
//             - either with IGR or with switches on Reset and Ctrl input
//             - Reset (IGR) and Auto (Switch) have a shared input
//             - Controller (IGR) and Manual (Switch) have a shared input
//             - default for 15bit mode is forwarded to actual setting for the installation with a switch
//
//////////////////////////////////////////////////////////////////////////////////


module n64rgbv2_top (
  // N64 Video Input
  VCLK,
  nDSYNC,
  D_i,

  // Controller and Reset
  nRST_nManualDB,
  CTRL_nAutoDB,
  

  // Jumper
  nSYNC_ON_GREEN,
  install_type, // installation type
                // - 1 -> with IGR functionalities
                // - 0 -> toogle switch for deblur (no IGR fubnctions)
  Default_nForceDeBlur,
  Default_DeBlur,
  Default_n15bit_mode,

  // Video output
  nHSYNC,
  nVSYNC,
  nCSYNC,
  nCLAMP,

  R_o,     // red data vector
  G_o,     // green data vector
  B_o,     // blue data vector

  CLK_ADV712x,
  nCSYNC_ADV712x,
  nBLANK_ADV712x
);

`include "vh/n64rgb_params.vh"

input                   VCLK;
input                   nDSYNC;
input [color_width-1:0] D_i;

inout nRST_nManualDB;
input CTRL_nAutoDB;

input  nSYNC_ON_GREEN;

input install_type; // installation type
                    // - 1 -> with IGR functionalities
                    // - 0 -> toogle switch for deblur (no IGR fubnctions)

input Default_nForceDeBlur;
input Default_DeBlur;
input Default_n15bit_mode;

output reg nHSYNC;
output reg nVSYNC;
output reg nCSYNC;
output reg nCLAMP;

output reg [color_width:0] R_o;
output reg [color_width:0] G_o;
output reg [color_width:0] B_o;

output reg CLK_ADV712x;
output reg nCSYNC_ADV712x;
output reg nBLANK_ADV712x;


`define SWITCH_INSTALL  !install_type
`define IGR_INSTALL      install_type

// start of rtl

// Part 1: connect IGR module
// ==========================

wire DRV_RST;
wire nForceDeBlur_IGR, nDeBlur_IGR, n15bit_mode_IGR;

reg nRST_IGR = 1'b0;

always @(posedge VCLK) begin
  if (`IGR_INSTALL)
    nRST_IGR <= nRST_nManualDB;
  else
    nRST_IGR <= 1'b0;
end


n64_igr igr(
  .VCLK(VCLK),
  .nRST_IGR(nRST_IGR),
  .DRV_RST(DRV_RST),
  .CTRL(CTRL_nAutoDB),
  .Default_nForceDeBlur(Default_nForceDeBlur),
  .Default_DeBlur(Default_DeBlur),
  .Default_n15bit_mode(Default_n15bit_mode),
  .nForceDeBlur(nForceDeBlur_IGR),
  .nDeBlur(nDeBlur_IGR),
  .n15bit_mode(n15bit_mode_IGR)
);

assign nRST_nManualDB = ~install_type ? 1'bz : 
                         DRV_RST      ? 1'b0 : 1'bz;

// Part 2 - 4: RGB Demux with De-Blur Add-On
// =========================================
//
// short description of N64s RGB and sync data demux
// -------------------------------------------------
//
// pulse shapes and their realtion to each other:
// VCLK (~50MHz, Numbers representing posedge count)
// ---. 3 .---. 0 .---. 1 .---. 2 .---. 3 .---
//    |___|   |___|   |___|   |___|   |___|
// nDSYNC (~12.5MHz)                           .....
// -------.       .-------------------.
//        |_______|                   |_______
//
// more info: http://members.optusnet.com.au/eviltim/n64rgb/n64rgb.html
//

// Part 2: get all of the vinfo needed for further processing
// ==========================================================

wire [3:0] vinfo_pass;

n64_vinfo_ext get_vinfo(
  .VCLK(VCLK),
  .nDSYNC(nDSYNC),
  .Sync_pre(vdata_r[0][`VDATA_SY_SLICE]),
  .Sync_cur(D_i[3:0]),
  .vinfo_o(vinfo_pass)
);


// Part 3: DeBlur Management (incl. heuristic)
// ===========================================

reg nForceDeBlur, nDeBlurMan, n15bit_mode, nrst_deblur;

always @(posedge VCLK) begin
  if (`IGR_INSTALL) begin
    nForceDeBlur <= nForceDeBlur_IGR;
    nDeBlurMan   <= nDeBlur_IGR;
    n15bit_mode  <= n15bit_mode_IGR;
    nrst_deblur  <= nRST_nManualDB;
  end else begin
    nForceDeBlur <= (~CTRL_nAutoDB & nRST_nManualDB);
    nDeBlurMan   <=                  nRST_nManualDB;
    n15bit_mode  <=  Default_n15bit_mode;
    nrst_deblur  <= ~CTRL_nAutoDB;
  end
end


wire ndo_deblur;

n64_deblur deblur_management(
  .VCLK(VCLK),
  .nDSYNC(nDSYNC),
  .nRST(nrst_deblur),
  .vdata_pre(vdata_r[0]),
  .D_i(D_i),
  .deblurparams_i({vinfo_pass,nForceDeBlur,nDeBlurMan}),
  .ndo_deblur(ndo_deblur)
);


// Part 4: data demux
// ==================

wire [`VDATA_FU_SLICE] vdata_r[0:1];

n64_vdemux video_demux(
  .VCLK(VCLK),
  .nDSYNC(nDSYNC),
  .D_i(D_i),
  .demuxparams_i({vinfo_pass[3:1],ndo_deblur,n15bit_mode}),
  .vdata_r_0(vdata_r[0]),
  .vdata_r_1(vdata_r[1])
);


// assign final outputs
// --------------------

always @(*) begin
  {nVSYNC,nCLAMP,nHSYNC,nCSYNC} <=  vdata_r[1][`VDATA_SY_SLICE];
   R_o                          <= {vdata_r[1][`VDATA_RE_SLICE],vdata_r[1][3*color_width-1]};
   G_o                          <= {vdata_r[1][`VDATA_GR_SLICE],vdata_r[1][2*color_width-1]};
   B_o                          <= {vdata_r[1][`VDATA_BL_SLICE],vdata_r[1][  color_width-1]};

  CLK_ADV712x    <= VCLK;
  nCSYNC_ADV712x <= nSYNC_ON_GREEN ? 1'b0 : vdata_r[1][vdata_width-4];
  nBLANK_ADV712x <= 1'b1;
end

endmodule
