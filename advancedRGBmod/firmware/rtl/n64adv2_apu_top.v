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

  // Audio Output to ADV7513
  AMCLK_o,
  ASCLK_o,
  ASDATA_o,
  ALRCLK_o
);

input AMCLK_i;
input nARST;

input ASCLK_i;
input ASDATA_i;
input ALRCLK_i;

output AMCLK_o;

output reg ASCLK_o = 1'b1;
output reg ASDATA_o = 1'b0;
output reg ALRCLK_o = 1'b0;

// start of rtl

// Part 1: synchronize inputs
// ==========================

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

// Part 2: Seriell to Parallel Conversion of Input
// ===============================================

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
reg signed [15:0] audio_left_in = 16'h0;
reg signed [15:0] audio_right_in = 16'h0;
reg audio_in_valid = 1'b0;

wire ch_i_sel = ALRCLK_ibuf[1];
reg [3:0] bit_i_sel = 4'd15;
wire rst_marker = (!ALRCLK_ibuf[2] &  ALRCLK_ibuf[1]) |
                  ( ALRCLK_ibuf[2] & !ALRCLK_ibuf[1]);

always @(posedge AMCLK_i) begin
  if (get_sdata) begin
    audio_lr_tmp[ch_i_sel][bit_i_sel] <= ASDATA_ibuf[1];
    bit_i_sel <= bit_i_sel - 1'b1;
  end
  if (rst_marker) begin
    bit_i_sel <= 4'd15;
  end
  if (new_sample) begin
    audio_left_in <= audio_lr_tmp[1];
    audio_right_in <= audio_lr_tmp[0];
    audio_in_valid <= 1'b1;
  end

  if (!nARST) begin
    audio_lr_tmp[1] <= 16'h0;
    audio_lr_tmp[0] <= 16'h0;
    audio_left_in <= 16'h0;
    audio_right_in <= 16'h0;
    audio_in_valid <= 1'b0;
    bit_i_sel <= 4'd15;
  end
end


// Part 3: Interpolation
// =====================

wire signed [31:0] audio_left_int;
wire audio_left_int_valid;

fir_audio audio_l_u(
  .clk(AMCLK_i),
  .reset_n(nARST),
  .ast_sink_data(audio_left_in),
  .ast_sink_valid(audio_in_valid),
  .ast_sink_error(2'b00),
  .ast_source_data(audio_left_int),
  .ast_source_valid(audio_left_int_valid)
);

wire signed [31:0] audio_right_int;
wire audio_right_int_valid;

fir_audio audio_r_u(
  .clk(AMCLK_i),
  .reset_n(nARST),
  .ast_sink_data(audio_right_in),
  .ast_sink_valid(audio_in_valid),
  .ast_sink_error(2'b00),
  .ast_source_data(audio_right_int),
  .ast_source_valid(audio_right_int_valid)
);

wire audio_int_valid = audio_left_int_valid & audio_right_int_valid;


// Part 4: Output Parallel to Serial Conversion
// ============================================

// some information:
// - 24.576MHz = 512 x 48kHz audio
// - left justified audio timing
//   o LRCLK up for left channel, down for right channel
//   o 256 MCLKs per section
// - 32bit for each channel
//   o 32 clocks per LRCLK edge change
//   o negedge alligned with LRCLK edge
//   o each bit alligned with negedge, i.e. stable at posedge for receiver
//   o 4 MCLKs per SCLK section

reg init_begin = 1'b1;
reg [7:0] cnt_256x = 8'h00;

reg signed [31:0] audio_lr_o_tmp [0:1];
initial begin
  audio_lr_o_tmp[1] = 32'h0;
  audio_lr_o_tmp[0] = 32'h0;
end

reg ch_o_sel = 1'b0;
reg [4:0] bit_o_sel = 5'd31;


always @(posedge AMCLK_i) begin
  if (~|cnt_256x[1:0])
    ASCLK_o <= ~ASCLK_o;
  if (~|cnt_256x[2:0]) begin
    ASDATA_o <= audio_lr_o_tmp[ch_o_sel][bit_o_sel];
    bit_o_sel <= bit_o_sel - 1'b1;
  end
  if (~|cnt_256x)
    ALRCLK_o <= ~ALRCLK_o;

  if (&cnt_256x) begin
    if (!ch_o_sel) begin
      audio_lr_o_tmp[1] = audio_left_int;
      audio_lr_o_tmp[0] = audio_right_int;
    end
    ch_o_sel <= ~ch_o_sel;
    bit_o_sel <= 5'd31;
  end

  cnt_256x <= cnt_256x + 1'b1;

  if (init_begin) begin
    init_begin <= 1'b0;
    cnt_256x <= 8'h00;

    audio_lr_o_tmp[1] = audio_left_int;
    audio_lr_o_tmp[0] = audio_right_int;
    ch_o_sel <= 1'b1;
    bit_o_sel <= 5'd31;

    ASCLK_o <= 1'b1;
    ASDATA_o <= 1'b0;
    ALRCLK_o <= 1'b0;
  end

  if (!nARST | !audio_int_valid) begin
    init_begin <= 1'b1;

    ASCLK_o <= 1'b1;
    ASDATA_o <= 1'b0;
    ALRCLK_o <= 1'b0;
  end
end

assign AMCLK_o = AMCLK_i;

endmodule
