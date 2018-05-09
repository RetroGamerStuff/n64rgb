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
// Module Name:    n64a_ram2port
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Max10, Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    simple line-multiplying
//
// Revision: 1.0
// Features: ip independent implementation of a ram (two port)
//
//////////////////////////////////////////////////////////////////////////////////


module n64a_ram2port(
  wrCLK,
  wren,
  wraddr,
  wrdata,
  
  rdCLK,
  rden,
  rdaddr,
  rddata
);

parameter ram_depth = 10;
parameter data_width = 32;

input wrCLK;
input wren;
input [ram_depth-1:0] wraddr;
input [data_width-1:0] wrdata;

input rdCLK;
input rden;
input [ram_depth-1:0] rdaddr;
output reg [data_width-1:0] rddata;


reg [data_width-1:0] data_buf[0:(2**ram_depth-1)];


reg                  wren_r   = 1'b0;
reg [ ram_depth-1:0] wraddr_r = {ram_depth{1'b0}};
reg [data_width-1:0] wrdata_r = {data_width{1'b0}};

always @(posedge wrCLK) begin
  wren_r <= wren;
  wraddr_r <= wraddr;
  wrdata_r <= wrdata;

  if (wren_r)
    data_buf[wraddr_r] <= wrdata_r;
end


//reg                  wren_rd_r   = 1'b0;
//reg [ ram_depth-1:0] wraddr_rd_r = {ram_depth{1'b0}};
//reg [data_width-1:0] wrdata_rd_r = {data_width{1'b0}};

reg                 rden_r   = 1'b0;
reg [ram_depth-1:0] rdaddr_r = {ram_depth{1'b0}};

always @(posedge rdCLK) begin
//  wren_rd_r <= wren;
//  wraddr_rd_r <= wraddr;
//  wrdata_rd_r <= wrdata;

  rden_r <= rden;
  rdaddr_r <= rdaddr;

  if (rden_r) begin
//    if (wren_rd_r && (rdaddr_r == wraddr_rd_r))
//      rddata <= wrdata_rd_r;
//    else
      rddata <= data_buf[rdaddr_r];
  end
end

endmodule
