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
// Module Name:    n64adv_clk_n_rst_hk
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: universial (PLL and 50MHz clock required)
// Tool versions:  Altera Quartus Prime
// Description:    Housekeeping for clock and reset generation
// Latest change:
//
//////////////////////////////////////////////////////////////////////////////////

module n64adv_clk_n_rst_hk(
  VCLK,
  SYS_CLK,
  nRST,

  nVRST,

  VCLK_Tx,
  nVRST_Tx,
  MANAGE_VPLL,
  VCLK_PLL_LOCKED,

  CLKs_controller,
  nSRST,
  SYS_PLL_LOCKED
);

input VCLK;
input SYS_CLK;
input nRST;

output nVRST;

output [1:0] VCLK_Tx;
output [1:0] nVRST_Tx;
input  [1:0] MANAGE_VPLL;
output       VCLK_PLL_LOCKED;

output [2:0] CLKs_controller;
output [2:0] nSRST;
output       SYS_PLL_LOCKED;


// Video

wire USE_VPLL  = MANAGE_VPLL[1];
wire TEST_VPLL = MANAGE_VPLL[0];
wire nEN_VPLL  = ~(USE_VPLL | TEST_VPLL);

wire VCLK_75M, VCLK_PLL_LOCKED_w;

video_pll video_pll_u(
  .inclk0(VCLK),
  .areset(nEN_VPLL),
  .c0(VCLK_75M),
  .locked(VCLK_PLL_LOCKED_w)
);



reset_generator reset_vclk_u(
  .clk(VCLK),
  .clk_en(1'b1),
  .async_nrst_i(nRST),
  .rst_o(nVRST)
);

wire nVRST_75M_Tx_w;
reset_generator reset_vclk_75M_u(
  .clk(VCLK_75M),
  .clk_en(VCLK_PLL_LOCKED_w),
  .async_nrst_i(nRST),
  .rst_o(nVRST_75M_Tx_w)
);

assign VCLK_PLL_LOCKED = VCLK_PLL_LOCKED_w;


reg nVRST_75M_Tx = 1'b0;
reg [2:0] USE_VPLL_buf = 3'b000;
reg [3:0] hold_nVRST_75M_Tx = 4'h0;

always @(posedge VCLK_75M) begin
  if (~|hold_nVRST_75M_Tx) begin
    nVRST_75M_Tx <= USE_VPLL ? nVRST_75M_Tx_w : 1'b0;
  end else begin
    nVRST_75M_Tx <= 1'b0;
    hold_nVRST_75M_Tx <= hold_nVRST_75M_Tx - 1'b1;
  end
  if (USE_VPLL_buf[2] != USE_VPLL_buf[1])
    hold_nVRST_75M_Tx <= 4'hf;

  USE_VPLL_buf[2:0] <= {USE_VPLL_buf[1:0],USE_VPLL};
end

assign VCLK_Tx  = {VCLK_75M,VCLK};
assign nVRST_Tx = {nVRST_75M_Tx,nVRST};


// system

wire CLK_4M, CLK_16k, CLK_25M, SYS_PLL_LOCKED_w;
sys_pll sys_pll_u(
  .inclk0(SYS_CLK),
  .c0(CLK_4M),
  .c1(CLK_16k),
  .c2(CLK_25M),
  .locked(SYS_PLL_LOCKED_w)
);

assign CLKs_controller = {CLK_4M,CLK_16k,CLK_25M};


reset_generator reset_sys_25M_u(
  .clk(CLK_25M),
  .clk_en(SYS_PLL_LOCKED_w),
  .async_nrst_i(1'b1),      // special situation here; this reset is only used for soft-CPU (NIOS II), which only resets on power cycle
  .rst_o(nSRST[0])
);

reset_generator reset_sys_4M_u(
  .clk(CLK_16k),
  .clk_en(SYS_PLL_LOCKED_w),
  .async_nrst_i(nRST),
  .rst_o(nSRST[2])
);

assign nSRST[1] = 1'b1;
assign SYS_PLL_LOCKED = SYS_PLL_LOCKED_w;



endmodule
