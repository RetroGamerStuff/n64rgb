//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2021 by Peter Bartmann <borti4938@gmail.com>
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
// VH-file Name:   n64adv_cparams
// Project Name:   N64 Advanced Mod
// Target Devices: several devices
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


`ifndef _n64adv_cparams_vh_
`define _n64adv_cparams_vh_


  // N64 controller sniffing
  // =======================

  // controller serial data bits:
  //  0: 7 - A, B, Z, St, Du, Dd, Dl, Dr
  //  8:15 - 'Joystick reset', (0), L, R, Cu, Cd, Cl, Cr
  // 16:23 - X axis
  // 24:31 - Y axis
  // 32    - Stop bit

  // define constants
  // don't edit these constants

  `define A  16'h0001 // button A
  `define B  16'h0002 // button B
  `define Z  16'h0004 // trigger Z
  `define St 16'h0008 // Start button

  `define Du 16'h0010 // D-pad up
  `define Dd 16'h0020 // D-pad down
  `define Dl 16'h0040 // D-pad left
  `define Dr 16'h0080 // D-pad right

  `define L  16'h0400 // shoulder button L
  `define R  16'h0800 // shoulder button R

  `define Cu 16'h1000 // C-button up
  `define Cd 16'h2000 // C-button down
  `define Cl 16'h4000 // C-button left
  `define Cr 16'h8000 // C-button right

  // In-game reset command
  // =====================

  `define IGR_RESET (`A + `B + `Z + `St + `R)

  // OSD menu window sizing
  // ======================

  // define font size (every value - 1)
  `define OSD_FONT_WIDTH  7
  `define OSD_FONT_HEIGHT 11

  // define text window size (every value - 1)
  `define MAX_TEXT_ROWS     11
  `define MAX_CHARS_PER_ROW 51

  // positioning of OSD window (not linedoubled)
  `define OSD_WINDOW_VSTART  51
  `define OSD_WINDOW_VSTOP   (`OSD_WINDOW_VSTART + 12 + (`MAX_TEXT_ROWS + 1)*(`OSD_FONT_HEIGHT + 1))
  `define OSD_WINDOW_HSTART  191
  `define OSD_WINDOW_HSTOP   (`OSD_WINDOW_HSTART + 15 + (`MAX_CHARS_PER_ROW + 1)*(`OSD_FONT_WIDTH + 1))


  // define some areas in the OSD windows
  `define OSD_TXT_VSTART (`OSD_WINDOW_VSTART + 8)
  `define OSD_TXT_VSTOP  (`OSD_TXT_VSTART + (`MAX_TEXT_ROWS + 1)*(`OSD_FONT_HEIGHT + 1))
  `define OSD_TXT_HSTART (`OSD_WINDOW_HSTART + 7)
  `define OSD_TXT_HSTOP  (`OSD_TXT_HSTART + (`MAX_CHARS_PER_ROW + 1)*(`OSD_FONT_WIDTH + 1))

  `define OSD_LOGO_VSTART   (`OSD_WINDOW_VSTART + 4)
  `define OSD_LOGO_VSTOP    (`OSD_LOGO_VSTART + 2*8)
  `define OSD_LOGO_HSTART   (`OSD_WINDOW_HSTART + 7)
  `define OSD_LOGO_HSTOP    (`OSD_LOGO_HSTART + 2*128)

  // define OSD background window color (three bits each color)
  `define OSD_BACKGROUND_WHITE    3
  `define OSD_BACKGROUND_GREY     2
  `define OSD_BACKGROUND_BLACK    1
  `define OSD_BACKGROUND_DARKBLUE 0

  `define OSD_WINDOW_BGCOLOR_WHITE    9'b111111111
  `define OSD_WINDOW_BGCOLOR_GREY     9'b010010010
  `define OSD_WINDOW_BGCOLOR_BLACK    9'b000000000
  `define OSD_WINDOW_BGCOLOR_DARKBLUE 9'b000000011

  // define text color
  `define FONTCOLOR_NON          0
  `define FONTCOLOR_BLACK        1
  `define FONTCOLOR_GREY         2
  `define FONTCOLOR_LIGHTGREY    3
  `define FONTCOLOR_WHITE        4
  `define FONTCOLOR_RED          5
  `define FONTCOLOR_GREEN        6
  `define FONTCOLOR_BLUE         7
  `define FONTCOLOR_YELLOW       8
  `define FONTCOLOR_CYAN         9
  `define FONTCOLOR_MAGENTA     10
  `define FONTCOLOR_DARKORANGE  11
  `define FONTCOLOR_TOMATO      12
  `define FONTCOLOR_DARKMAGENTA 13
  `define FONTCOLOR_NAVAJOWHITE 14
  `define FONTCOLOR_DARKGOLD    15

  `define OSD_TXT_COLOR_BLACK       21'h000000
  `define OSD_TXT_COLOR_GREY        21'h07CF9F
  `define OSD_TXT_COLOR_LIGHTGREY   21'h0FDFBF
  `define OSD_TXT_COLOR_WHITE       21'h1FFFFF
  `define OSD_TXT_COLOR_RED         21'h1FC000
  `define OSD_TXT_COLOR_GREEN       21'h003F80
  `define OSD_TXT_COLOR_BLUE        21'h00007F
  `define OSD_TXT_COLOR_YELLOW      21'h1FFF80
  `define OSD_TXT_COLOR_CYAN        21'h003FFF
  `define OSD_TXT_COLOR_MAGENTA     21'h1FC07F
  `define OSD_TXT_COLOR_DARKORANGE  21'h199980
  `define OSD_TXT_COLOR_TOMATO      21'h1DD721
  `define OSD_TXT_COLOR_DARKMAGENTA 21'h114045
  `define OSD_TXT_COLOR_NAVAJOWHITE 21'h1FF7D6
  `define OSD_TXT_COLOR_DARKGOLD    21'h1DEB07

  `define OSD_LOGO_COLOR `OSD_TXT_COLOR_DARKORANGE

`endif