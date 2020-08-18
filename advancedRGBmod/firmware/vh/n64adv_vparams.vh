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
// Company: Circuit-Board.de
// Engineer: borti4938
//
// VH-file Name:   n64adv_vparams
// Project Name:   N64 Advanced Mod
// Target Devices: several devices
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


parameter color_width_i = 7;
parameter color_width_o = 8;

parameter vdata_width_i = 4 + 3*color_width_i;
parameter vdata_width_o = 4 + 3*color_width_o;


`ifndef _n64adv2_vparams_vh_
`define _n64adv2_vparams_vh_

  // video vector slices and frame definitions
  // =========================================

  `define VDATA_I_FU_SLICE    vdata_width_i-1:0               // full slice
  `define VDATA_I_SY_SLICE  3*color_width_i+3:3*color_width_i // slice sync
  `define VDATA_I_CO_SLICE  3*color_width_i-1:0               // slice color
  `define VDATA_I_RE_SLICE  3*color_width_i-1:2*color_width_i // slice red
  `define VDATA_I_GR_SLICE  2*color_width_i-1:  color_width_i // slice green
  `define VDATA_I_BL_SLICE    color_width_i-1:0               // slice blue

  `define VDATA_O_FU_SLICE    vdata_width_o-1:0
  `define VDATA_O_SY_SLICE  3*color_width_o+3:3*color_width_o 
  `define VDATA_O_CO_SLICE  3*color_width_o-1:0
  `define VDATA_O_RE_SLICE  3*color_width_o-1:2*color_width_o
  `define VDATA_O_GR_SLICE  2*color_width_o-1:  color_width_o
  `define VDATA_O_BL_SLICE    color_width_o-1:0

  `define GAMMA_TABLE_OFF   4'b0101

  `define ACTIVE_PIXEL_PER_LINE     640
  `define ACTIVE_PIXEL_PER_LINE_2x  1280

  `define HSTART_NTSC     10'd0111
  `define HSTOP_NTSC      (`HSTART_NTSC + `ACTIVE_PIXEL_PER_LINE - 10'd1)
  `define HSTART_PAL      10'd0136
  `define HSTOP_PAL       (`HSTART_PAL + `ACTIVE_PIXEL_PER_LINE - 10'd1)
  `define HSTART_NTSC_2x  11'd0223
  `define HSTOP_NTSC_2x   (`HSTART_NTSC_2x + `ACTIVE_PIXEL_PER_LINE_2x - 11'd1)
  `define HSTART_PAL_2x   11'd0273
  `define HSTOP_PAL_2x    (`HSTART_PAL_2x + `ACTIVE_PIXEL_PER_LINE_2x - 11'd1)

  `define BUF_NUM_OF_PAGES    4
//  `define BUF_DEPTH_PER_PAGE  (`HSTOP_NTSC - `HSTART_NTSC + 1)/2
  `define BUF_DEPTH_PER_PAGE  `ACTIVE_PIXEL_PER_LINE
  
  `define PIXEL_PER_LINE_NTSC_2x  1545
  `define PIXEL_PER_LINE_PAL_2x   1587
  `define PIXEL_PER_LINE_MAX_2x   1600

  `define HS_WIDTH_NTSC_LX2     7'd113
  `define H_SHIFT_NTSC_240P_LX2 5'b00010
  `define H_SHIFT_NTSC_480I_LX2 5'b11110
  `define HS_WIDTH_NTSC_LX3     7'd38
  `define H_SHIFT_NTSC_240P_LX3 5'b11011

  `define HS_WIDTH_PAL_LX2      7'd123
  `define H_SHIFT_PAL_288P_LX2  5'b00001
  `define H_SHIFT_PAL_576I_LX2  5'b11110

  `define VS_WIDTH_NTSC_LX2  4'd2
  `define VS_WIDTH_NTSC_LX3  4'd5
  `define VS_WIDTH_PAL_LX2  4'd5

  // Testpattern
  // ===========

  `define TESTPAT_HSTART      10'd060
  `define TESTPAT_HSTOP       10'd701
  `define TESTPAT_VSTART_NTSC  9'd005
  `define TESTPAT_VSTOP_NTSC   9'd247
  `define TESTPAT_VSTART_PAL   9'd006
  `define TESTPAT_VSTOP_PAL    9'd296

`endif