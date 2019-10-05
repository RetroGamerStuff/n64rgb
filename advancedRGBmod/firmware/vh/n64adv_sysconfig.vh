//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2019 by Peter Bartmann <borti4938@gmail.com>
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
// VH-file Name:   n64adv_sysconfig
// Project Name:   N64 Advanced Mod
// Target Devices: several devices
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


`ifndef _n64adv_sysconfig_vh_
`define _n64adv_sysconfig_vh_

  // configuration as defined in n64adv_controller.v (must match software)
  //  wire [31:0] SysConfigSet0;
  //    general structure [31:16] misc, [15:0] video
  //    [31:24] {(5bits reserve),show_osd_logo,show_osd,mute_osd}
  //    [23:16] {(5bits reserve),use_igr,igr for 15bit mode and deblur (not used in logic)}
  //    [15: 8] {show_testpattern,(2bits reserve)FilterSet (3bits),YPbPr,RGsB}
  //    [ 7: 0] {gamma (4bits),(1bit reserve),VI-DeBlur (2bit), 15bit mode}
  //  wire [31:0] SysConfigSet1;
  //    general structure [31:16] 240p settings, [15:0] 480i settings
  //    [31:16] 240p: {(1bit reserve),linemult (2bits),Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_Method,Sl_ID,Sl_En}
  //    [15: 0] 480i: {(1bit reserve),field_fix,bob_deint.,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_link,Sl_ID,Sl_En}
  // later
  //  OutConfigSet <= {SysConfigSet0[15:0],SysConfigSet1};

  `define SysConfigSet0_Offset  32
  `define show_testpattern_bit  15 + `SysConfigSet0_Offset
  `define FilterSet_slice       12 + `SysConfigSet0_Offset : 10 + `SysConfigSet0_Offset
  `define YPbPr_bit              9 + `SysConfigSet0_Offset
  `define RGsB_bit               8 + `SysConfigSet0_Offset
  `define gamma_slice            7 + `SysConfigSet0_Offset :  4 + `SysConfigSet0_Offset
  `define ndeblurman_bit         2 + `SysConfigSet0_Offset
  `define nforcedeblur_bit       1 + `SysConfigSet0_Offset
  `define n15bit_mode_bit        0 + `SysConfigSet0_Offset

  `define v240p_linemult_slice  30:29
    `define v240p_linex3_bit      30
    `define v240p_linex2_bit      29
  `define v240p_SL_hybrid_slice 28:24
  `define v240p_SL_str_slice    23:20
  `define v240p_SL_method_bit   18
  `define v240p_SL_ID_bit       17
  `define v240p_SL_En_bit       16

  `define v480i_field_fix_bit   14
  `define v480i_linex2_bit      13
  `define v480i_SL_hybrid_slice 12: 8
  `define v480i_SL_str_slice     7:4
  `define v480i_SL_linked_bit    2
  `define v480i_SL_ID_bit        1
  `define v480i_SL_En_bit        0

`endif