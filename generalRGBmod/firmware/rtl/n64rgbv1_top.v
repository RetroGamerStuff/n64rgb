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
// Company: Circuit-Board.de
// Engineer: borti4938
// (initial design file by Ikari_01)
//
// Module Name:    n64rgbv1_top
// Project Name:   N64 RGB DAC Mod
// Target Devices: several MaxII & MaxV devices
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies: rtl/n64_igr.v        (Rev. 2.5)
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

//`define OPTION_INVLPF

module n64rgbv1_top (
  // N64 Video Input
  nCLK,
  nDSYNC,
  D_i,

  // Controller and Reset
  nRST_nManualDB,
  CTRL_nAutoDB,
  

  // Jumper
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

  R_o,
  G_o,
  B_o,

  // Filter control of THS7374
  nTHS7374_LPF_Bypass_p85_i,   // my first prototypes have FIL pad input at pin 85 (MaxV only)
  nTHS7374_LPF_Bypass_p98_i,   // the GitHub final version at pin 98
  THS7374_LPF_Bypass_o         // so simply combine both for same firmware file
);

`include "vh/n64rgb_params.vh"

input                   nCLK;
input                   nDSYNC;
input [color_width-1:0] D_i;

inout nRST_nManualDB;
input CTRL_nAutoDB;

input install_type; // installation type
                    // - 1 -> with IGR functionalities
                    // - 0 -> toogle switch for deblur (no IGR fubnctions)


input Default_nForceDeBlur;
input Default_DeBlur;
input Default_n15bit_mode;

output nHSYNC;
output nVSYNC;
output nCSYNC;
output nCLAMP;

output [color_width-1:0] R_o;
output [color_width-1:0] G_o;
output [color_width-1:0] B_o;

input  nTHS7374_LPF_Bypass_p85_i;   // my first prototypes have FIL pad input at pin 85 (MaxV only)
input  nTHS7374_LPF_Bypass_p98_i;   // the GitHub final version at pin 98
output  THS7374_LPF_Bypass_o;       // so simply combine both for same firmware file


`define SWITCH_INSTALL  !install_type
`define IGR_INSTALL      install_type


// start of rtl

// Part 1: connect IGR module
// ==========================

wire nForceDeBlur_IGR, nDeBlur_IGR, n15bit_mode_IGR;
wire DRV_RST;
`ifdef OPTION_INVLPF
  wire InvLPF;
`endif

reg nRST_IGR;

always @(negedge nCLK) begin
  if (`IGR_INSTALL)
    nRST_IGR <= nRST_nManualDB;
  else
    nRST_IGR <= 1'b0;
end

n64_igr igr(
  .nCLK(nCLK),
  .nRST_IGR(nRST_IGR),
  .DRV_RST(DRV_RST),
  .CTRL(CTRL_nAutoDB),
  .Default_nForceDeBlur(Default_nForceDeBlur),
  .Default_DeBlur(Default_DeBlur),
  .Default_n15bit_mode(Default_n15bit_mode),
  .nForceDeBlur(nForceDeBlur_IGR),
  .nDeBlur(nDeBlur_IGR),
  .n15bit_mode(n15bit_mode_IGR)
`ifdef OPTION_INVLPF
  ,
  .InvLPF(InvLPF)
`endif
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
// nCLK (~50MHz, Numbers representing negedge count)
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
  .nCLK(nCLK),
  .nDSYNC(nDSYNC),
  .Sync_pre(vdata_r[`VDATA_SY_SLICE]),
  .Sync_cur(D_i[3:0]),
  .vinfo_o(vinfo_pass)
);


// Part 3: DeBlur Management (incl. heuristic)
// ===========================================

reg nForceDeBlur, nDeBlurMan, n15bit_mode, nrst_deblur;

always @(negedge nCLK) begin
  if (`IGR_INSTALL) begin
    nForceDeBlur <= nForceDeBlur_IGR;
    nDeBlurMan   <= nDeBlur_IGR;
    n15bit_mode  <= n15bit_mode_IGR;
    nrst_deblur  <= nRST_nManualDB;
  end else begin
    nForceDeBlur <= (~CTRL_nAutoDB & nRST_nManualDB);
    nDeBlurMan   <=                  nRST_nManualDB;
    n15bit_mode  <=  Default_n15bit_mode;
    nrst_deblur  <= (~CTRL_nAutoDB & nRST_nManualDB);
  end
end


wire ndo_deblur;

n64_deblur deblur_management(
  .nCLK(nCLK),
  .nDSYNC(nDSYNC),
  .nRST(nrst_deblur),
  .vdata_pre(vdata_r),
  .D_i(D_i),
  .deblurparams_i({vinfo_pass,nForceDeBlur,nDeBlurMan}),
  .ndo_deblur(ndo_deblur)
);


// Part 4: data demux
// ==================

wire [`VDATA_FU_SLICE] vdata_r;

n64_vdemux video_demux(
  .nCLK(nCLK),
  .nDSYNC(nDSYNC),
  .D_i(D_i),
  .demuxparams_i({vinfo_pass,ndo_deblur,n15bit_mode}),
  .vdata_r_0(vdata_r),
  .vdata_r_1({nVSYNC,nCLAMP,nHSYNC,nCSYNC,R_o,G_o,B_o})
);


// assign final outputs
// --------------------

`ifdef OPTION_INVLPF
  assign THS7374_LPF_Bypass_o = ~(nTHS7374_LPF_Bypass_p85_i & nTHS7374_LPF_Bypass_p98_i) ^ InvLPF;
`else
  assign THS7374_LPF_Bypass_o = ~(nTHS7374_LPF_Bypass_p85_i & nTHS7374_LPF_Bypass_p98_i);
`endif


endmodule
