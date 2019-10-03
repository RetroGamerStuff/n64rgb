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
// Module Name:    n64adv_top
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV:    EP4CE10E22
//                 Cyclone 10 LP: 10CL010YE144
// Tool versions:  Altera Quartus Prime
//
// Revision: 1.33
// Features: see repository readme
//
//////////////////////////////////////////////////////////////////////////////////


module n64adv_top (
  // N64 Video Input
  VCLK,
  nVDSYNC,
  VD_i,

  // System CLK, Controller and Reset
  SYS_CLK,
  CTRL_i,
  nRST,

  // Video Output to ADV712x
     CLK_ADV712x,
  nCSYNC_ADV712x,
//   nBLANK_ADV712x,
  VD_o,     // video component data vector

  // Sync / Debug / Filter AddOn Output
  nCSYNC,
  nVSYNC_or_F2,
  nHSYNC_or_F1,

  // Jumper VGA Sync / Filter AddOn
  UseVGA_HVSync, // (J1) use Filter out if '0'; use /HS and /VS if '1'
  nFilterBypass, // (J1) bypass filter if '0'; set filter as output if '1'
                 //      (only applicable if UseVGA_HVSync is '0')

  // Jumper Video Output Type and Scanlines
  nEN_RGsB,   // (J2) generate RGsB if '0'
  nEN_YPbPr,  // (J2) generate RGsB if '0' (no RGB, no RGsB (overrides nEN_RGsB))
  SL_str,     // (J3) Scanline strength    (only for line multiplication and not for 480i bob-deint.)
  n240p,      // (J4) no linemultiplication for 240p if '0' (beats n480i_bob)
  n480i_bob   // (J4) bob de-interlacing of 480i material if '0'

);

parameter [3:0] hdl_fw_main = 4'd1;
parameter [7:0] hdl_fw_sub  = 8'd33;

`include "vh/n64adv_vparams.vh"

input                     VCLK;
input                     nVDSYNC;
input [color_width_i-1:0] VD_i;

input  SYS_CLK;
input  CTRL_i;
inout  nRST;

output                        CLK_ADV712x;
output                     nCSYNC_ADV712x;
// output                     nBLANK_ADV712x;
output [3*color_width_o-1:0] VD_o;

output nCSYNC;
output nVSYNC_or_F2;
output nHSYNC_or_F1;

input UseVGA_HVSync;
input nFilterBypass;

input       nEN_RGsB;
input       nEN_YPbPr;
input [1:0] SL_str;
input       n240p;
input       n480i_bob;


// start of rtl


// PLLs

wire CLK_4M, CLK_16k, CLK_25M, SYS_PLL_LOCKED;
sys_pll sys_pll_u(
  .inclk0(SYS_CLK),
  .c0(CLK_4M),
  .c1(CLK_16k),
  .c2(CLK_25M),
  .locked(SYS_PLL_LOCKED)
);
wire [2:0] CLKs_controller = {CLK_4M,CLK_16k,CLK_25M};


wire VCLK_50M, VCLK_75M, VIDEO_PLL_LOCKED;
video_pll video_pll_u(
  .inclk0(VCLK),
  .c0(VCLK_50M),
  .c1(VCLK_75M),
  .locked(VIDEO_PLL_LOCKED)
);
wire [1:0] VCLK_Tx = {VCLK_75M,VCLK_50M};

// synchronize resets
wire nSRST_25M;
reset_generator reset_sys_25M_u(
  .clk(CLK_25M),
  .clk_en(SYS_PLL_LOCKED),
  .async_nrst_i(1'b1),      // special situation here; this reset is only used for soft-CPU (NIOS II), which only resets on power cycle
  .rst_o(nSRST_25M)
);
wire nSRST_4M;
reset_generator reset_sys_4M_u(
  .clk(CLK_16k),
  .clk_en(SYS_PLL_LOCKED),
  .async_nrst_i(nRST),
  .rst_o(nSRST_4M)
);


wire [2:0] nSRST = {nSRST_4M,1'b1,nSRST_25M};

wire nVRST;
reset_generator reset_vclk_u(
  .clk(VCLK),
  .clk_en(1'b1),
  .async_nrst_i(nRST),
  .rst_o(nVRST)
);

wire [1:0] nVRST_Tx;
reset_generator reset_vclk_50M__u(
  .clk(VCLK_50M),
  .clk_en(VIDEO_PLL_LOCKED),
  .async_nrst_i(nRST),
  .rst_o(nVRST_Tx[0])
);
reset_generator reset_vclk_75M_u(
  .clk(VCLK_75M),
  .clk_en(VIDEO_PLL_LOCKED),
  .async_nrst_i(nRST),
  .rst_o(nVRST_Tx[1])
);


// controller module

wire [ 3:0] InfoSet;
wire [ 6:0] JumperCfgSet = {nFilterBypass,n240p,~n480i_bob,~SL_str,~nEN_YPbPr,(nEN_YPbPr & ~nEN_RGsB)}; // (~nEN_YPbPr | nEN_RGsB) ensures that not both jumpers are set and passed through the NIOS II
wire [47:0] ConfigSet;
wire [24:0] OSDWrVector;
wire [ 1:0] OSDInfo;

n64adv_controller #({hdl_fw_main,hdl_fw_sub}) n64adv_controller_u(
  .CLKs(CLKs_controller),
  .CLKs_valid(SYS_PLL_LOCKED),
  .nRST(nRST),
  .nSRST(nSRST),
  .CTRL(CTRL_i),
  .InfoSet(InfoSet),
  .JumperCfgSet(JumperCfgSet),
  .OutConfigSet(ConfigSet),
  .OSDWrVector(OSDWrVector),
  .OSDInfo(OSDInfo),
  .VCLK(VCLK),
  .nVDSYNC(nVDSYNC),
  .VD_VSi(VD_i[3]),
  .nVRST(nVRST)
);


// picture processing module

n64adv_ppu_top n64adv_ppu_u(
  .VCLK(VCLK),
  .nVRST(nVRST),
  .nVDSYNC(nVDSYNC),
  .VD_i(VD_i),
  .InfoSet(InfoSet),
  .ConfigSet(ConfigSet),
  .OSDCLK(CLK_25M),
  .OSDWrVector(OSDWrVector),
  .OSDInfo(OSDInfo),
  .VCLK_Tx(VCLK_Tx),
  .nVRST_Tx(nVRST_Tx),
  .VCLK_o(CLK_ADV712x),
//  .nBLANK(nBLANK_ADV712x),
  .VD_o(VD_o),
  .nCSYNC({nCSYNC,nCSYNC_ADV712x}),
  .UseVGA_HVSync(UseVGA_HVSync),
  .nVSYNC_or_F2(nVSYNC_or_F2),
  .nHSYNC_or_F1(nHSYNC_or_F1)
);

endmodule
