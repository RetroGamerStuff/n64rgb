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
// Company: Circuit-Board.de
// Engineer: borti4938
//
// VH-file Name:   n64adv2_vparams
// Project Name:   N64 Advanced HDMI Mod
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

  `define HSTART_NTSC 11'd0225
  `define HSTOP_NTSC  11'd1500
  `define HSTART_PAL  11'd0275
  `define HSTOP_PAL   11'd1550

  `define BUF_NUM_OF_PAGES    6
  `define BUF_DEPTH_PER_PAGE  (`HSTOP_NTSC - `HSTART_NTSC + 1)/2
  
  `define PIXEL_PER_LINE_2x_NTSC  1545
  `define PIXEL_PER_LINE_2x_PAL   1587
  `define PIXEL_PER_LINE_2x_MAX   1600

  `define HS_WIDTH_NTSC_240P  7'd115
  `define HS_WIDTH_NTSC_480I  7'd111
  `define HS_WIDTH_PAL_288P   7'd124
  `define HS_WIDTH_PAL_576I   7'd121

  `define VS_WIDTH  4'd3

  // Testpattern
  // ===========

  `define TESTPAT_HSTART      10'd060
  `define TESTPAT_HSTOP       10'd701
  `define TESTPAT_VSTART_NTSC  9'd005
  `define TESTPAT_VSTOP_NTSC   9'd247
  `define TESTPAT_VSTART_PAL   9'd006
  `define TESTPAT_VSTOP_PAL    9'd296

`endif