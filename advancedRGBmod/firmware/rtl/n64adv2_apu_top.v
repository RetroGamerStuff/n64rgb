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
// Module Name:    n64adv2_apu_top
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




module n64adv2_apu_top (
  AMCLK_i,
  nARST,

  // N64 Audio Input
  ASCLK_i,
  ASDATA_i,
  ALRCLK_i,

  // Audio Output
  ASCLK_o,
  ASDATA_o,
  ALRCLK_o
);

input AMCLK_i;
input nARST;

input ASCLK_i;
input ASDATA_i;
input ALRCLK_i;

output ASCLK_o;
output ASDATA_o;
output ALRCLK_o;


// parallization

wire [15:0] APDATA [0:1];
wire APDATA_VALID;

n64_sample_i2s i2s_rx_u(
  .AMCLK_i(AMCLK_i),
  .nARST(nARST),
  .ASCLK_i(ASCLK_i),
  .ASDATA_i(ASDATA_i),
  .ALRCLK_i(ALRCLK_i),
  .APDATA_LEFT_o(APDATA[1]),
  .APDATA_RIGHT_o(APDATA[0]),
  .APDATA_VALID_o(APDATA_VALID)
);


// interpolation

wire signed [23:0] APDATA_INT [0:1];
wire [1:0] APDATA_INT_VALID;

fir_audio audio_l_u(
  .clk(AMCLK_i),
  .reset_n(nARST),
  .ast_sink_data(APDATA[1]),
  .ast_sink_valid(APDATA_VALID),
  .ast_sink_error(2'b00),
  .ast_source_data(APDATA_INT[1]),
  .ast_source_valid(APDATA_INT_VALID[1])
);

fir_audio audio_r_u(
  .clk(AMCLK_i),
  .reset_n(nARST),
  .ast_sink_data(APDATA[0]),
  .ast_sink_valid(APDATA_VALID),
  .ast_sink_error(2'b00),
  .ast_source_data(APDATA_INT[0]),
  .ast_source_valid(APDATA_INT_VALID[0])
);


// seriellization

i2s_leftjustified_tx i2s_tx(
  .AMCLK_i(AMCLK_i),
  .nARST(nARST),
  .APSDATA_LEFT_i(APDATA_INT[1]),
  .APSDATA_RIGHT_i(APDATA_INT[0]),
  .APDATA_VALID_i(APDATA_INT_VALID),
  .ASCLK_o(ASCLK_o),
  .ASDATA_o(ASDATA_o),
  .ALRCLK_o(ALRCLK_o)
);


endmodule
