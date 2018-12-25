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
// Module Name:    n64adv2_top
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone 10 LP: 10CL025YE144
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies: 
// (more dependencies may appear in other files)
//
// Revision: 0.01
// Features: see repository readme
//
//////////////////////////////////////////////////////////////////////////////////

module n64adv2_top (
  // System CLK, Controller and Reset
  SCLK_0,
  SCLK_1,
  CTRL_i,
  nRST,

  // N64 Video Input
  VCLK_0,
  VCLK_1,
  nVDSYNC,
  VD_i,
  
  // N64 audio in
  ASCLK_i,
  ASDATA_i,
  ALRCLK_i,

  // Video Output to ADV7513
  VCLK_o,
  VSYNC_o,
  HSYNC_o,
  VD_o,
  
  // I2S audio output for ADV7513
  ASCLK_o,
  ASDATA_o,
  ALRCLK_o,
  
  // I2C, Int
  I2C_SCL,
  I2C_SDA,
  INT_ADV7513,
  

  // LED outs and extended data for misc purpouses
  LED_0,
  LED_1,
  ExtData
);

parameter [3:0] hdl_fw_main = 4'd0;
parameter [7:0] hdl_fw_sub  = 8'd01;

`include "vh/n64adv_vparams.vh"

input SCLK_0;
input SCLK_1;
input CTRL_i;
inout nRST;

input VCLK_0;
input VCLK_1;
input nVDSYNC;
input [color_width_i-1:0] VD_i;

input ASCLK_i;
input ASDATA_i;
input ALRCLK_i;

output VCLK_o;
output VSYNC_o;
output HSYNC_o;
output [3*color_width_o-1:0] VD_o;

output ASCLK_o;
output ASDATA_o;
output ALRCLK_o;

inout I2C_SCL;
inout I2C_SDA;
input INT_ADV7513;

output LED_0;
output LED_1;
output [3:0] ExtData;


// start of rtl


// PLLs

wire [2:0] PLL_LOCKED;

wire CLK_4M, CLK_16k, CLK_25M;

pll4ctrl pll4ctrl_u(
  .inclk0(SCLK_0),
  .c0(CLK_4M),
  .c1(CLK_16k),
  .c2(CLK_25M),
  .locked(PLL_LOCKED[0])
);

wire [2:0] CLKs_controller = {CLK_4M,CLK_16k,CLK_25M};
assign LED_0 = PLL_LOCKED[0];


//wire PxCLK_Tx;
//
//pll4video pll4video_u(
//  .inclk0(SCLK_1),
//  .c0(PxCLK_Tx),
//  .locked(PLL_LOCKED[1])
//);
//
//assign LED_0 = PLL_LOCKED[1];
assign PLL_LOCKED[1] = 1'b0;



wire AMCLK;

pll4audio pll4audio_u(
  .inclk0(SCLK_1),
  .c0(AMCLK),
  .locked(PLL_LOCKED[2])
);

assign LED_1 = PLL_LOCKED[2];


// reset signals

reg nVRST_0_pre = 1'b0;
reg nVRST_0 = 1'b0;

always @(posedge VCLK_0) begin
  nVRST_0 <= nVRST_0_pre;
  nVRST_0_pre <= nRST;
end

reg nVRST_1_pre = 1'b0;
reg nVRST_1 = 1'b0;

always @(posedge VCLK_1) begin
  nVRST_1 <= nVRST_1_pre;
  nVRST_1_pre <= nRST;
end

reg [1:0] nARST_pre = 1'b0;
always @(posedge AMCLK)
  nARST_pre <= {nARST_pre[0],nRST};
wire nARST = PLL_LOCKED[2] ? nARST_pre[1] : 1'b0;

// controller module

wire [ 2:0] InfoSet;
wire [47:0] ConfigSet;
wire [24:0] OSDWrVector;
wire [ 2:0] OSDInfo;

n64adv2_controller #({hdl_fw_main,hdl_fw_sub}) n64adv2_controller_u(
  .CLKs(CLKs_controller),
  .CLKs_valid(PLL_LOCKED[0]),
  .nRST(nRST),
  .CTRL(CTRL_i),
  .nSRST(PLL_LOCKED[0]),
  .I2C_SCL(I2C_SCL),
  .I2C_SDA(I2C_SDA),
  .INT_ADV7513(INT_ADV7513),
  .InfoSet(InfoSet),
  .OutConfigSet(ConfigSet),
  .OSDWrVector(OSDWrVector),
  .OSDInfo(OSDInfo),
  .VCLK(VCLK_0),
  .nVDSYNC(nVDSYNC),
  .VD_VSi(VD_i[3]),
  .nVRST(nVRST_0)
);


// picture processing module

n64adv2_ppu_top n64adv2_ppu_u(
  .VCLK(VCLK_0),
  .nVDSYNC(nVDSYNC),
  .nVRST(nVRST_0),
  .VD_i(VD_i),
  .InfoSet(InfoSet),
  .ConfigSet(ConfigSet),
  .OSDCLK(CLK_25M),
  .OSDWrVector(OSDWrVector),
  .OSDInfo(OSDInfo),
  .VCLK_Tx(VCLK_1),
  .nVRST_Tx(nVRST_1),
  .VSYNC_o(VSYNC_o),
  .HSYNC_o(HSYNC_o),
  .VD_o(VD_o)
);

assign VCLK_o = VCLK_1;


// audio processing module

n64adv2_apu_top n64adv2_apu_u(
  .AMCLK_i(AMCLK),
  .nARST(nARST),
  .ASCLK_i(ASCLK_i),
  .ASDATA_i(ASDATA_i),
  .ALRCLK_i(ALRCLK_i),
  .ASCLK_o(ASCLK_o),
  .ASDATA_o(ASDATA_o),
  .ALRCLK_o(ALRCLK_o)
);


endmodule
