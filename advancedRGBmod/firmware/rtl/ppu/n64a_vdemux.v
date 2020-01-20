//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmail.com>
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
// Module Name:    n64a_vdemux
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    demux the video data from the input data stream
//
// Dependencies: vh/n64a_params.vh
//
//////////////////////////////////////////////////////////////////////////////////


module n64a_vdemux(
  VCLK,
  nVDSYNC,
  nRST,

  VD_i,
  demuxparams_i,

  vdata_r_0,
  vdata_r_1
);

`include "vh/n64adv_vparams.vh"

input VCLK;
input nVDSYNC;
input nRST;

input  [color_width_i-1:0] VD_i;
input  [              4:0] demuxparams_i;

output reg [`VDATA_I_FU_SLICE] vdata_r_0 = {vdata_width_i{1'b0}}; // buffer for sync, red, green and blue
output reg [`VDATA_I_FU_SLICE] vdata_r_1 = {vdata_width_i{1'b0}}; // (unpacked array types in ports requires system verilog)


// unpack deblur info

wire [1:0] data_cnt    = demuxparams_i[4:3];
wire       vmode       = demuxparams_i[  2];
wire       ndo_deblur  = demuxparams_i[  1];
wire       n15bit_mode = demuxparams_i[  0];

wire posedge_nCSYNC = !vdata_r_0[3*color_width_i] &  VD_i[0];


// start of rtl

reg nblank_rgb = 1'b1;

always @(posedge VCLK or negedge nRST)
  if (!nRST) begin
    nblank_rgb <= 1'b1;
  end else if (!nVDSYNC) begin
    if (ndo_deblur) begin
      nblank_rgb <= 1'b1;
    end else begin
      if(posedge_nCSYNC) // posedge nCSYNC -> reset blanking
        nblank_rgb <= vmode;
      else
        nblank_rgb <= ~nblank_rgb;
    end
  end

  
always @(posedge VCLK or negedge nRST) // data register management
  if (!nRST) begin
    vdata_r_0 <= {vdata_width_i{1'b0}};
    vdata_r_1 <= {vdata_width_i{1'b0}};
  end else begin
    if (!nVDSYNC) begin
      // shift data to output registers
      vdata_r_1[`VDATA_I_SY_SLICE] <= vdata_r_0[`VDATA_I_SY_SLICE];
      if (nblank_rgb)  // deblur active: pass RGB only if not blanked
        vdata_r_1[`VDATA_I_CO_SLICE] <= vdata_r_0[`VDATA_I_CO_SLICE];

      // get new sync data
      vdata_r_0[`VDATA_I_SY_SLICE] <= VD_i[3:0];
    end else begin
      // demux of RGB
      case(data_cnt)
        2'b01: vdata_r_0[`VDATA_I_RE_SLICE] <= n15bit_mode ? VD_i : {VD_i[6:2], 2'b00};
        2'b10: vdata_r_0[`VDATA_I_GR_SLICE] <= n15bit_mode ? VD_i : {VD_i[6:2], 2'b00};
        2'b11: vdata_r_0[`VDATA_I_BL_SLICE] <= n15bit_mode ? VD_i : {VD_i[6:2], 2'b00};
      endcase
    end
  end

endmodule