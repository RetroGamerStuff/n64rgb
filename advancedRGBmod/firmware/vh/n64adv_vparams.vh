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

  `define PIXEL_PER_LINE_NTSC_normal0   772
  `define PIXEL_PER_LINE_NTSC_normal1   773
  `define PIXEL_PER_LINE_NTSC_2x        1546
  `define PIXEL_PER_LINE_NTSC_4x        3093

  `define PIXEL_PER_LINE_PAL_2x_normal  1588 // slighty modified and thus different pattern
  `define PIXEL_PER_LINE_PAL_2x_long    1589 // slighty modified and thus different pattern
  `define PIXEL_PER_LINE_PAL_4x_short   3173 // original
  `define PIXEL_PER_LINE_PAL_4x_normal  3177 // original
  `define PIXEL_PER_LINE_PAL_4x_long0   3181 // original
  `define PIXEL_PER_LINE_PAL_4x_long1   3186 // original
  `define PIXEL_PER_LINE_PAL_4x_long2   3187 // original

  `define PIXEL_PER_LINE_MAX            800
  `define PIXEL_PER_LINE_MAX_2x         1600
  `define PIXEL_PER_LINE_MAX_4x         3200

  `define ACTIVE_PIXEL_PER_LINE     640
  `define ACTIVE_PIXEL_PER_LINE_2x  1280

  `define TOTAL_LINES_NTSC_LX2_0  525
  `define TOTAL_LINES_NTSC_LX2_1  526
  `define TOTAL_LINES_NTSC_LX3_0  789
  `define ACTIVE_LINES_NTSC       240
  `define ACTIVE_LINES_NTSC_LX2   480
  `define ACTIVE_LINES_NTSC_LX3   720

  `define TOTAL_LINES_PAL_LX2_0 625
  `define TOTAL_LINES_PAL_LX2_1 626
  `define ACTIVE_LINES_PAL      288
  `define ACTIVE_LINES_PAL_LX2  576


  `define HSTART_NTSC     108
  `define HSTOP_NTSC      (`HSTART_NTSC + `ACTIVE_PIXEL_PER_LINE)
  `define HSTART_NTSC_2x  (2*`HSTART_NTSC+1)
  `define HSTOP_NTSC_2x   (`HSTART_NTSC_2x + `ACTIVE_PIXEL_PER_LINE_2x)

  `define VSTART_NTSC     18
  `define VSTOP_NTSC      (`VSTART_NTSC + `ACTIVE_LINES_NTSC)
  `define VSTART_NTSC_LX2 (2*`VSTART_NTSC)
  `define VSTOP_NTSC_LX2  (`VSTART_NTSC_LX2 + `ACTIVE_LINES_NTSC_LX2)
  `define VSTART_NTSC_LX3 (3*`VSTART_NTSC)
  `define VSTOP_NTSC_LX3  (`VSTART_NTSC_LX3 + `ACTIVE_LINES_NTSC_LX3)

  `define HS_WIDTH_NTSC_LX2_2x  113
  `define HS_WIDTH_NTSC_LX3_2x  38
  `define VS_WIDTH_NTSC_LX2     2
  `define VS_WIDTH_NTSC_LX3     5

  `define H_SHIFT_NTSC_240P_LX2_2x  0
  `define H_SHIFT_NTSC_480I_LX2_2x  28
  `define H_SHIFT_NTSC_240P_LX3_2x  25
  `define V_SHIFT_NTSC_LX2          0
  `define V_SHIFT_NTSC_LX3          3




  `define HSTART_PAL      128
  `define HSTOP_PAL       (`HSTART_PAL + `ACTIVE_PIXEL_PER_LINE)
  `define HSTART_PAL_2x   (2*`HSTART_PAL+1)
  `define HSTOP_PAL_2x    (`HSTART_PAL_2x + `ACTIVE_PIXEL_PER_LINE_2x)

  `define VSTART_PAL      24
  `define VSTOP_PAL       (`VSTART_PAL + `ACTIVE_LINES_PAL)
  `define VSTART_PAL_LX2  (2*`VSTART_PAL)
  `define VSTOP_PAL_LX2   (`VSTART_PAL_LX2 + `ACTIVE_LINES_PAL_LX2)

  `define HS_WIDTH_PAL_LX2_2x 123
  `define VS_WIDTH_PAL_LX2    5

  `define H_SHIFT_PAL_288P_LX2_2x 0
  `define H_SHIFT_PAL_576I_LX2_2x 28
  `define V_SHIFT_PAL_LX2         0


  `define BUF_NUM_OF_PAGES    4
  `define BUF_DEPTH_PER_PAGE  `ACTIVE_PIXEL_PER_LINE

`endif