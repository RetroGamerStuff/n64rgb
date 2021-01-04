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
// VH-file Name:   n64adv_ppuconfig
// Project Name:   N64 Advanced Mod
// Target Devices: several devices
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


`ifndef _n64adv_ppuconfig_vh_
`define _n64adv_ppuconfig_vh_

  // configuration as defined in n64adv_controller.v (must match software)
  //  wire [31:0] SysConfigSet2;
  //    [31:24] {(8bits reserve)}
  //    [23:16] {(8bits reserve)}
  //    [15: 8] {(2bits reserve),use_vpll,test_vpll,show_testpattern,show_osd_logo,show_osd,mute_osd}
  //    [ 7: 0] {igr for reset,igr for 16bit mode and deblur (not used in logic),pal_awareness (not used in logic),Exchange_RB_out,FilterSet (3bits)}
  //  wire [31:0] SysConfigSet1;
  //    [31:24] {(7bits reserved), dejitter}
  //    [23:16] {(1bit reserved), LineX H-Shift (7bits)}
  //    [15: 8] {(2bits reserved),LineX V-Shift (6bits)}
  //    [ 7: 0] {YPbPr, RGsB, gamma (4bits),VI-DeBlur,16bit mode}
  //  wire [31:0] SysConfigSet0;
  //    general structure [31:16] 240p settings, [15:0] 480i settings
  //    [31:16] 240p: {(1bit reserve),linemult (2bits),Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_Method,Sl_ID,Sl_En}
  //    [15: 0] 480i: {(1bit reserve),field_fix,bob_deint.,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_link,Sl_ID,Sl_En}

  `define use_vpll_bit                13
  `define test_vpll_bit               12
  `define show_testpattern_direct_bit 11
  `define show_osd_logo_bit           10
  `define show_osd_bit                 9
  `define mute_osd_bit                 8
  `define igr_reset_enable_bit         7
  `define Exchange_RB_out_direct_bit   3
  `define FilterSet_slice_direct       2 : 0

  `define SysConfigSet2_Offset  64
  `define show_testpattern_bit  11 + `SysConfigSet2_Offset - 7
  `define Exchange_RB_out_bit    3 + `SysConfigSet2_Offset
  `define FilterSet_slice        2 + `SysConfigSet2_Offset :  0 + `SysConfigSet2_Offset

  `define SysConfigSet1_Offset  32
  `define pal_dejitter_bit      24 + `SysConfigSet1_Offset
  `define linex_hshift_slice    22 + `SysConfigSet1_Offset : 16 + `SysConfigSet1_Offset
  `define linex_vshift_slice    13 + `SysConfigSet1_Offset :  8 + `SysConfigSet1_Offset
  `define YPbPr_bit              7 + `SysConfigSet1_Offset
  `define RGsB_bit               6 + `SysConfigSet1_Offset
  `define gamma_slice            5 + `SysConfigSet1_Offset :  2 + `SysConfigSet1_Offset
  `define videblur_bit           1 + `SysConfigSet1_Offset
  `define n16bit_mode_bit        0 + `SysConfigSet1_Offset

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
  `define v480i_SL_str_slice     7: 4
  `define v480i_SL_linked_bit    2
  `define v480i_SL_ID_bit        1
  `define v480i_SL_En_bit        0
  
  `define GAMMA_TABLE_OFF   4'b0101

`endif