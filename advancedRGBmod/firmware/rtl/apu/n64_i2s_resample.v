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
// Module Name:    n64_i2s_resample.v
// Project Name:   N64 Advanced HMDI Mod
// Target Devices: Cyclone 10 LP: 10CL025YE144
// Tool versions:  Altera Quartus Prime
// Description:
//
// Revision:
// Features: see repository readme
//
//////////////////////////////////////////////////////////////////////////////////




module n64_i2s_resample (
  AMCLK_i,
  nARST,

  // N64 Audio Input
  ASCLK_i,
  ASDATA_i,
  ALRCLK_i,

  // Parallel Output
  APDATA_LEFT_o,
  APDATA_RIGHT_o,
  APDATA_VALID_o
);

input AMCLK_i;
input nARST;

input ASCLK_i;
input ASDATA_i;
input ALRCLK_i;

output reg signed [15:0] APDATA_LEFT_o = 16'h0;
output reg signed [15:0] APDATA_RIGHT_o = 16'h0;
output reg APDATA_VALID_o = 1'b0;



// synchronize inputs with new AMCLK_i

reg [2:0] ASCLK_ibuf = 3'b0;
reg [2:0] ASDATA_ibuf = 3'b0;
reg [2:0] ALRCLK_ibuf = 3'b0;

always @(posedge AMCLK_i) begin
  ASCLK_ibuf <= {ASCLK_ibuf[1:0],ASCLK_i};
  ASDATA_ibuf <= {ASDATA_ibuf[1:0],ASDATA_i};
  ALRCLK_ibuf <= {ALRCLK_ibuf[1:0],ALRCLK_i};

  if (!nARST) begin
    ASCLK_ibuf <= 3'b0;
    ASDATA_ibuf <= 3'b0;
    ALRCLK_ibuf <= 3'b0;
  end
end


// seriell to parallel conversion of inputs

// some information:
// - N64 uses a BU9480F 16bit audio DAC
// - LRCLK -> Left channel data up, right channel down
// - ASDATA in 2'compl., 16bit each channel latches on posedge of ASCLK

wire new_sample = !ALRCLK_ibuf[2] & ALRCLK_ibuf[1];
wire get_sdata = !ASCLK_ibuf[2] & ASCLK_ibuf[1];

reg signed [15:0] audio_lr_tmp [0:1];
initial begin
  audio_lr_tmp[1] = 16'h0;
  audio_lr_tmp[0] = 16'h0;
end
reg apdata_valid_pre = 1'b0;

wire ch_i_sel = ALRCLK_ibuf[1];
reg [3:0] bit_i_sel = 4'd15;
reg ch_rd_done = 1'b0;
wire rst_marker = (!ALRCLK_ibuf[2] &  ALRCLK_ibuf[1]) |
                  ( ALRCLK_ibuf[2] & !ALRCLK_ibuf[1]);

always @(posedge AMCLK_i) begin
  if (get_sdata & !ch_rd_done) begin
    audio_lr_tmp[ch_i_sel][bit_i_sel] <= ASDATA_ibuf[1];
    if (~|bit_i_sel)
      ch_rd_done <= 1'b1;
    else
      bit_i_sel <= bit_i_sel - 1'b1;
  end
  if (rst_marker) begin
    bit_i_sel <= 4'd15;
    ch_rd_done <= 1'b0;
  end
  if (new_sample) begin
    APDATA_LEFT_o <= audio_lr_tmp[1];
    APDATA_RIGHT_o <= audio_lr_tmp[0];
    APDATA_VALID_o <= apdata_valid_pre;
    apdata_valid_pre <= 1'b1;
  end

  if (!nARST) begin
    audio_lr_tmp[1] <= 16'h0;
    audio_lr_tmp[0] <= 16'h0;
    APDATA_LEFT_o <= 16'h0;
    APDATA_RIGHT_o <= 16'h0;
    APDATA_VALID_o <= 1'b0;
    apdata_valid_pre <= 1'b0;
    bit_i_sel <= 4'd15;
    ch_rd_done <= 1'b0;
  end
end

endmodule
