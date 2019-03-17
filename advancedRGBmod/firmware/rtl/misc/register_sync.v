//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2019 by Peter Bartmann <borti4938@gmx.de>
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
// Module Name:    register_sync
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: universial
// Tool versions:  Altera Quartus Prime
// Description:    generates a reset signal (low-active by default) with duration of
//                 two clock cycles
//
//////////////////////////////////////////////////////////////////////////////////


module register_sync(
  clk,
  clk_en,
  nrst,
  reg_i,
  reg_o
);


parameter reg_width = 16;
parameter reg_preset = {reg_width{1'b0}};

input clk;
input clk_en;
input nrst;

input [reg_width-1:0] reg_i;
output reg [reg_width-1:0] reg_o = reg_preset;


reg [reg_width-1:0] reg_o_pre = reg_preset;

always @(posedge clk or negedge nrst) begin
  if (!nrst) begin
    reg_o   <= reg_preset;
    reg_o_pre <= reg_preset;
  end else if (clk_en) begin
    reg_o   <= reg_o_pre;
    reg_o_pre <= reg_i;
  end
end

endmodule
