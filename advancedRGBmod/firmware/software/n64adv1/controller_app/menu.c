/*********************************************************************************
 *
 * This file is part of the N64 RGB/YPbPr DAC project.
 *
 * Copyright (C) 2015-2021 by Peter Bartmann <borti4938@gmail.com>
 *
 * N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************************************************************************
 *
 * menu.c
 *
 *  Created on: 09.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "n64.h"
#include "config.h"
#include "menu.h"

#include "textdefs_p.h"
#include "vd_driver.h"
#include "fw.h"

char szText[VD_WIDTH];
extern alt_u8 use_flash;
extern vmode_t vmode_menu, vmode_n64adv;
extern cfg_timing_model_sel_type_t timing_menu, timing_n64adv;


static const arrowshape_t selection_arrow = {
    .left  = ARROW_RIGHT,
    .right = EMPTY
};

static const arrowshape_t optval_arrow = {
    .left  = TRIANGLE_LEFT,
    .right = TRIANGLE_RIGHT
};

static const arrow_t front_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = 1
};

static const arrow_t vicfg_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (VICFG_VALS_H_OFFSET - 2)
};

static const arrow_t vicfg_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (VICFG_VALS_H_OFFSET - 2)
};

static const arrow_t vicfg_240p480i_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (VICFG_VSUB_VALS_H_OFFSET - 2)
};

static const arrow_t vicfg_240p_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (VICFG_VSUB_VALS_H_OFFSET - 2)
};

static const arrow_t vicfg_vpll_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (VICFG_VPLLSUB_VALS_H_OFFSET - 2)
};

static const arrow_t vicfg_vpll_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (VICFG_VPLLSUB_VALS_H_OFFSET - 2)
};

static const arrow_t vicfg_timing_opt_arrow_0 = {
    .shape = &optval_arrow,
    .hpos = (VICFG_VTIMSUB_VALS_H_0_OFFSET - 2)
};

static const arrow_t vicfg_timing_opt_arrow_1 = {
    .shape = &optval_arrow,
    .hpos = (VICFG_VTIMSUB_VALS_H_1_OFFSET - 2)
};

static const arrow_t vicfg_timing_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (VICFG_VTIMSUB_VALS_H_1_OFFSET - 2)
};

static const arrow_t misc_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (MISC_VALS_H_OFFSET - 2)
};

static const arrow_t misc_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (MISC_VALS_H_OFFSET - 2)
};

menu_t home_menu, vinfo_screen, vicfg1_screen, vicfg2_screen, vicfg_240p_opt_subscreen, vicfg_480i_opt_subscreen,
       vicfg_vpll_subscreen, vicfg_timing_subscreen, misc_screen, rwdata_screen, about_screen, thanks_screen, license_screen;

extern config_t deblur_mode_current, mode16bit_current, ntsc_pal_selection;
extern config_t vformat, deblur_mode, gamma_lut, mode16bit, pal_awareness;
extern config_t linex_240p, sl_en, sl_method, sl_id, sl_str, slhyb_str;
extern config_t bob_deinter_480i, field_shift_fix_480i, sl_en_480i, sl_link_480i, sl_id_480i, sl_str_480i, slhyb_str_480i;
extern config_t use_vpll;
extern config_t timing_selection, vert_shift, hor_shift, pal_dejitter;
extern config_t igr_reset, igr_deblur, igr_16bitmode, filteraddon_cutoff, exchange_rb_out;


menu_t home_menu = {
    .type = HOME,
    .header  = &home_header,
    .overlay = &home_overlay,
    .current_selection = 1,
    .number_selections = 7,
    .leaves = {
        {.id = MAIN2VINFO_V_OFFSET  , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &vinfo_screen},
        {.id = MAIN2CFG_V_OFFSET    , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &vicfg1_screen},
        {.id = MAIN2MISC_V_OFFSET   , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &misc_screen},
        {.id = MAIN2SAVE_V_OFFSET   , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &rwdata_screen},
        {.id = MAIN2ABOUT_V_OFFSET  , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &about_screen},
        {.id = MAIN2THANKS_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &thanks_screen},
        {.id = MAIN2LICENSE_V_OFFSET, .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &license_screen}
    }
};

#define VICONFIG_SUBMENU_SELECTION  1
#define MISC_SUBMENU_SELECTION      1


menu_t vinfo_screen = {
    .type = VINFO,
    .header = &vinfo_header,
    .overlay = &vinfo_overlay,
    .parent = &home_menu
};


menu_t vicfg1_screen = {
    .type = CONFIG,
    .header = &vicfg1_header,
    .overlay = &vicfg1_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = VICFG1_NTSC_PAL_AWARENESS_V_OFFSET, .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &pal_awareness},
        {.id = VICFG1_NTSC_PAL_SELECT_V_OFFSET   , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &ntsc_pal_selection},
        {.id = VICFG1_240P_SET_V_OFFSET          , .arrow_desc = &vicfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &vicfg_240p_opt_subscreen},
        {.id = VICFG1_480I_SET_V_OFFSET          , .arrow_desc = &vicfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &vicfg_480i_opt_subscreen},
        {.id = VICFG1_TIMING_V_OFFSET            , .arrow_desc = &vicfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &vicfg_timing_subscreen},
        {.id = VICFG1_GAMMA_V_OFFSET             , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &gamma_lut},
        {.id = VICFG1_PAGE2_V_OFFSET             , .arrow_desc = &vicfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &vicfg2_screen}
    }
};

#define NSTC_PAL_SUB_SELECTION  1
#define V240P_SUB_SELECTION     2
#define V480I_SUB_SELECTION     3


menu_t vicfg2_screen = {
    .type = CONFIG,
    .header = &vicfg2_header,
    .overlay = &vicfg2_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = VICFG2_COLOR_SPACE_V_OFFSET   , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &vformat},
        {.id = VICFG2_EXCH_RB_OUT_V_OFFSET   , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &exchange_rb_out},
        {.id = VICFG2_DEBLURMODE_V_OFFSET    , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_mode_current},
        {.id = VICFG2_DEBLURMODE_DEF_V_OFFSET, .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_mode},
        {.id = VICFG2_16BIT_V_OFFSET         , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &mode16bit_current},
        {.id = VICFG2_16BIT_DEF_V_OFFSET     , .arrow_desc = &vicfg_opt_arrow, .leavetype = ICONFIG , .config_value = &mode16bit},
        {.id = VICFG2_PAGE1_V_OFFSET         , .arrow_desc = &vicfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &vicfg1_screen}
    }
};

#define DEBLUR_CURRENT_SELECTION  2
#define M16BIT_CURRENT_SELECTION  4


menu_t vicfg_240p_opt_subscreen = {
    .type = CONFIG,
    .header = &vicfg_240p_opt_header,
    .overlay = &vicfg_240p_opt_overlay,
    .parent = &vicfg1_screen,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = VICFG_VSUB_LINEX_V_OFFSET    , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &linex_240p},
        {.id = VICFG_VSUB_VPLL_V_OFFSET     , .arrow_desc = &vicfg_240p_sel_arrow    , .leavetype = ISUBMENU, .submenu      = &vicfg_vpll_subscreen},
        {.id = VICFG_VSUB_SL_EN_V_OFFSET    , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_en},
        {.id = VICFG_VSUB_SL_METHOD_V_OFFSET, .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_method},
        {.id = VICFG_VSUB_SL_ID_V_OFFSET    , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_id},
        {.id = VICFG_VSUB_SL_STR_V_OFFSET   , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_str},
        {.id = VICFG_VSUB_SLHYB_STR_V_OFFSET, .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &slhyb_str}
    }
};


menu_t vicfg_480i_opt_subscreen = {
    .type = CONFIG,
    .header = &vicfg_480i_opt_header,
    .overlay = &vicfg_480i_opt_overlay,
    .parent = &vicfg1_screen,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = VICFG_VSUB_LINEX_V_OFFSET    , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &bob_deinter_480i},
        {.id = VICFG_VSUB_FIELDFIX_V_OFFSET , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &field_shift_fix_480i},
        {.id = VICFG_VSUB_SL_EN_V_OFFSET    , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_en_480i},
        {.id = VICFG_VSUB_SL_METHOD_V_OFFSET, .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_link_480i},
        {.id = VICFG_VSUB_SL_ID_V_OFFSET    , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_id_480i},
        {.id = VICFG_VSUB_SL_STR_V_OFFSET   , .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_str_480i},
        {.id = VICFG_VSUB_SLHYB_STR_V_OFFSET, .arrow_desc = &vicfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &slhyb_str_480i}
    }
};

#define LINEX_SELECTION           0
#define VPLL_SUBMENU_SELECTION    1
#define FIELD_SHIFT_FIX_SELECTION 1
#define SL_EN_SELECTION           2
#define SL_METHOD_SELECTION       3
#define SL_LINKED_SELECTION       3


menu_t vicfg_vpll_subscreen = {
    .type = CONFIG,
    .header = &vicfg_vpll_opt_header,
    .overlay = &vicfg_vpll_opt_overlay,
    .parent = &vicfg_240p_opt_subscreen,
    .current_selection = 0,
    .number_selections = 2,
    .leaves = {
        {.id = VICFG_VPLLSUB_TEST_V_OFFSET, .arrow_desc = &vicfg_vpll_sel_arrow, .leavetype = IFUNC  , .sys_fun_0    = &run_vpll_test},
        {.id = VICFG_VPLLSUB_EN_V_OFFSET  , .arrow_desc = &vicfg_vpll_opt_arrow, .leavetype = ICONFIG, .config_value = &use_vpll}
    }
};

#define VPLL_TEST_SELECTION 0
#define VPLL_USE_SELECTION  1


menu_t vicfg_timing_subscreen = {
    .type = CONFIG,
    .header = &vicfg_timing_opt_header,
    .overlay = &vicfg_timing_opt_overlay,
    .parent = &vicfg1_screen,
    .current_selection = 0,
    .number_selections = 5,
    .leaves = {
        {.id = VICFG_VTIMSUB_MODE_V_OFFSET   , .arrow_desc = &vicfg_timing_opt_arrow_0, .leavetype = ICONFIG, .config_value = &timing_selection},
        {.id = VICFG_VTIMSUB_VSHIFT_V_OFFSET , .arrow_desc = &vicfg_timing_opt_arrow_1, .leavetype = ICONFIG, .config_value = &vert_shift},
        {.id = VICFG_VTIMSUB_HSHIFT_V_OFFSET , .arrow_desc = &vicfg_timing_opt_arrow_1, .leavetype = ICONFIG, .config_value = &hor_shift},
        {.id = VICFG_VTIMSUB_PALDEJ_V_OFFSET , .arrow_desc = &vicfg_timing_opt_arrow_1, .leavetype = ICONFIG, .config_value = &pal_dejitter},
        {.id = VICFG_VTIMSUB_RESET_V_OFFSET  , .arrow_desc = &vicfg_timing_sel_arrow  , .leavetype = IFUNC  , .sys_fun_0    = &cfg_reset_timing}
    }
};

#define TIMING_PAGE_SELECTION     0
#define VERTSHIFT_SELECTION       1
#define HORSHIFT_SELECTION        2
#define DEJITTER_SELECTION        3
#define RESET_TIMINGS_SECLECTION  4


menu_t misc_screen = {
    .type = CONFIG,
    .header = &misc_header,
    .overlay = &misc_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 5,
    .leaves = {
        {.id = MISC_IGR_RESET_V_OFFSET    , .arrow_desc = &misc_opt_arrow, .leavetype = ICONFIG, .config_value = &igr_reset},
        {.id = MISC_IGR_DEBLUR_V_OFFSET   , .arrow_desc = &misc_opt_arrow, .leavetype = ICONFIG, .config_value = &igr_deblur},
        {.id = MISC_IGR_16BITMODE_V_OFFSET, .arrow_desc = &misc_opt_arrow, .leavetype = ICONFIG, .config_value = &igr_16bitmode},
        {.id = MISC_FILTERADDON_V_OFFSET  , .arrow_desc = &misc_opt_arrow, .leavetype = ICONFIG, .config_value = &filteraddon_cutoff},
        {.id = MISC_SHOWTESTPAT_V_OFFSET  , .arrow_desc = &misc_sel_arrow, .leavetype = IFUNC,   .sys_fun_0    = &cfg_show_testpattern}
    }
};

#define FILTER_ADDON_SELECTION      3
#define TESTPATTERN_FUNC_SELECTION  4


menu_t rwdata_screen = {
    .type = RWDATA,
    .header = &rwdata_header,
    .overlay = &rwdata_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 4,
    .leaves = {
        {.id = RWDATA_SAVE_FL_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .sys_fun_1 = &cfg_save_to_flash},
        {.id = RWDATA_LOAD_FL_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .sys_fun_1 = &cfg_load_from_flash},
        {.id = RWDATA_LOAD_JS_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .sys_fun_1 = &cfg_load_jumperset},
        {.id = RWDATA_LOAD_N64_V_OFFSET, .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .sys_fun_1 = &cfg_load_defaults}
    }
};

#define RW_SAVE_CFG_SELECTION       0
#define RW_LOAD_CFG_SELECTION       1
#define RW_LOAD_N64_480P_SELECTION  2
#define RW_LOAD_N64_DEF_SELECTION   3


menu_t about_screen = {
   .type = TEXT,
   .overlay = &about_overlay,
   .parent = &home_menu
};

menu_t thanks_screen = {
   .type = TEXT,
   .overlay = &thanks_overlay,
   .parent = &home_menu
};

menu_t license_screen = {
   .type = TEXT,
   .overlay = &license_overlay,
   .parent = &home_menu
};

menu_t welcome_screen = {
   .type = TEXT,
   .header = &welcome_header,
   .overlay = &welcome_overlay,
   .parent = &home_menu
};


static inline alt_u8 is_home_menu (menu_t *menu)
  {  return (menu == &home_menu); }
static inline alt_u8 is_vicfg1_screen (menu_t *menu)
  {  return (menu == &vicfg1_screen); }
static inline alt_u8 is_vicfg2_screen (menu_t *menu)
  {  return (menu == &vicfg2_screen); }
static inline alt_u8 is_vicfg_240p_screen (menu_t *menu)
  {  return (menu == &vicfg_240p_opt_subscreen); }
static inline alt_u8 is_vicfg_480i_screen (menu_t *menu)
  {  return (menu  == &vicfg_480i_opt_subscreen); }
static inline alt_u8 is_vicfg_480i_sl_are_linked (menu_t *menu)
{  return ( is_vicfg_480i_screen(menu) && (cfg_get_value(&sl_link_480i,0) == ON)); }
static inline alt_u8 is_vicfg_vpll_screen (menu_t *menu)
  {  return (menu == &vicfg_vpll_subscreen); }
static inline alt_u8 is_vicfg_timing_screen (menu_t *menu)
  {  return (menu == &vicfg_timing_subscreen); }
static inline alt_u8 is_misc_screen (menu_t *menu)
  {  return (menu == &misc_screen); }


void val2txt_func(alt_u8 v) { sprintf(szText,"%u", v); };
void val2txt_6b_binaryoffset_func(alt_u8 v) { if (v & 0x20) sprintf(szText," %2u", (v&0x1F)); else sprintf(szText,"-%2u", (v^0x1F)+1); };
void val2txt_7b_binaryoffset_half_func(alt_u8 v) {
  alt_u8 val;
  if (v & 0x40) {
    val = v&0x3F;
    sprintf(szText," %2u.%u", (val)/2, 5*(val & 0x1));
  } else {
    val = (v^0x3F)+1;
    sprintf(szText,"-%2u.%u", (val)/2, 5*(val & 0x1));
  }
};
void flag2set_func(alt_u8 v) { sprintf(szText,"[ ]"); if (v) szText[1] = (char) CHECKBOX_TICK; };
void scanline_str2txt_func(alt_u8 v) { v++; sprintf(szText,"%3u.%02u%%", (v*625)/100, 25*(v&3)); };
void scanline_hybrstr2txt_func(alt_u8 v) { sprintf(szText,"%3u.%02u%%", (v*625)/100, 25*(v&3)); };
void gamma2txt_func(alt_u8 v) { sprintf(szText,"%u.%02u", v > 4, 5* v + 75 - (100 * (v > 4))); };


void print_hv_timing_overlay(linemult_t lx_mode) {
  alt_u8 font_color_vshift = lx_mode == PASSTHROUGH ? FONTCOLOR_GREY : FONTCOLOR_WHITE;
  vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,VICFG_VTIMSUB_VSHIFT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,font_color_vshift,vicfg_timing_opt_overlay0v);
  vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,VICFG_VTIMSUB_HSHIFT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,vicfg_timing_opt_overlay0h);
}
void print_dejitter_overlay(vmode_t palmode) {
  alt_u8 font_color = palmode ? FONTCOLOR_WHITE : FONTCOLOR_GREY;
  vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,VICFG_VTIMSUB_PALDEJ_V_OFFSET,BACKGROUNDCOLOR_STANDARD,font_color,vicfg_timing_opt_overlay1);
}

void print_current_timing_mode()
{
  sprintf(szText,"Current: %s",VTimingSel[timing_n64adv]);
  vd_print_string(0, VD_HEIGHT-1, BACKGROUNDCOLOR_STANDARD, FONTCOLOR_NAVAJOWHITE, &szText[0]);
}

void print_ctrl_data() {
  sprintf(szText,"Ctrl.Data: 0x%08x",(uint) ctrl_data);
  vd_print_string(0, VD_HEIGHT-1, BACKGROUNDCOLOR_STANDARD, FONTCOLOR_NAVAJOWHITE, &szText[0]);
}


updateaction_t modify_menu(cmd_t command, menu_t* *current_menu, configuration_t* sysconfig)
{

  updateaction_t todo = NON;

  static alt_u8 vicfg_page = 1;
  cfg_offon_t pal_awareness_val = (cfg_offon_t) cfg_get_value(&pal_awareness,0);

  switch (command) {
    case CMD_MUTE_MENU:
      return MENU_MUTE;
    case CMD_UNMUTE_MENU:
      return MENU_UNMUTE;
    case CMD_CLOSE_MENU:
      while ((*current_menu)->parent) {
        (*current_menu)->current_selection = 0;
        *current_menu = (*current_menu)->parent;
      }
      (*current_menu)->current_selection = 1;
      return MENU_CLOSE;
    case CMD_MENU_BACK:
      if ((*current_menu)->parent) {
        (*current_menu)->current_selection = 0;
        *current_menu = (*current_menu)->parent;
        return NEW_OVERLAY;
      } else {
        (*current_menu)->current_selection = 1;
        return MENU_CLOSE;
      }
    case CMD_MENU_PAGE_RIGHT:
      if (is_vicfg1_screen(*current_menu)) {
        *current_menu = &vicfg2_screen;
        vicfg_page = 2;
        return NEW_OVERLAY;
      }
      if (is_vicfg2_screen(*current_menu)) {
        (*current_menu)->parent->current_selection = VICONFIG_SUBMENU_SELECTION; // tell home that we are now at misc config;
        *current_menu = &misc_screen;
        return NEW_OVERLAY;
      }
      if (is_misc_screen(*current_menu)) {
        (*current_menu)->parent->current_selection = VICONFIG_SUBMENU_SELECTION; // tell home that we are now at VI-config;
        *current_menu = &vicfg1_screen;
        vicfg_page = 1;
        return NEW_OVERLAY;
      }
      if (is_vicfg_240p_screen(*current_menu)) {
        *current_menu = &vicfg_480i_opt_subscreen;
        todo = NEW_OVERLAY;
        break;
      }
      if (is_vicfg_480i_screen(*current_menu)) {
        *current_menu = &vicfg_240p_opt_subscreen;
        if (pal_awareness_val == ON) {
          cfg_inc_value(&ntsc_pal_selection);
          vmode_menu = (vmode_t) cfg_get_value(&ntsc_pal_selection,0);
          cfg_load_linex_word(sysconfig,vmode_menu);
        }
        todo = NEW_OVERLAY;
        break;
      }
      if (is_vicfg_timing_screen(*current_menu)){
        cfg_inc_value(&timing_selection);
        timing_menu = cfg_get_value(&timing_selection,0);
        if (timing_menu == PPU_CURRENT) timing_menu = timing_n64adv;
        cfg_load_timing_word(sysconfig,timing_menu);
        todo = NEW_OVERLAY;
        break;
      }
      break;
    case CMD_MENU_PAGE_LEFT:
      if (is_vicfg1_screen(*current_menu)) {
        (*current_menu)->parent->current_selection = VICONFIG_SUBMENU_SELECTION; // tell home that we are now at misc config;
        *current_menu = &misc_screen;
        return NEW_OVERLAY;
      }
      if (is_vicfg2_screen(*current_menu)) {
        *current_menu = &vicfg1_screen;
        vicfg_page = 1;
        return NEW_OVERLAY;
      }
      if (is_misc_screen(*current_menu)) {
        (*current_menu)->parent->current_selection = VICONFIG_SUBMENU_SELECTION; // tell home that we are now at VI-config;
        *current_menu = &vicfg2_screen;
        vicfg_page = 2;
        return NEW_OVERLAY;
      }
      if (is_vicfg_240p_screen(*current_menu)) {
        *current_menu = &vicfg_480i_opt_subscreen;
        if (pal_awareness_val == ON) {
          cfg_inc_value(&ntsc_pal_selection);
          vmode_menu = (vmode_t) cfg_get_value(&ntsc_pal_selection,0);
          cfg_load_linex_word(sysconfig,vmode_menu);
        }
        todo = NEW_OVERLAY;
        break;
      }
      if (is_vicfg_480i_screen(*current_menu)) {
        *current_menu = &vicfg_240p_opt_subscreen;
        todo = NEW_OVERLAY;
        break;
      }
      if (is_vicfg_timing_screen(*current_menu)){
        cfg_dec_value(&timing_selection);
        timing_menu = cfg_get_value(&timing_selection,0);
        if (timing_menu == PPU_CURRENT) timing_menu = timing_n64adv;
        cfg_load_timing_word(sysconfig,timing_menu);
        todo = NEW_OVERLAY;
        break;
      }
      break;
    default:
      break;
  }

  if (((*current_menu)->type == TEXT) ||
      ((*current_menu)->type == VINFO)  )
    return NON;

  switch (command) {
    case CMD_MENU_DOWN:
      (*current_menu)->current_selection++;
      if ((*current_menu)->current_selection == (*current_menu)->number_selections)
        (*current_menu)->current_selection = 0;
      todo = NEW_SELECTION;
      break;
    case CMD_MENU_UP:
      if ((*current_menu)->current_selection == 0)
        (*current_menu)->current_selection = (*current_menu)->number_selections - 1;
      else
        (*current_menu)->current_selection--;
      todo = NEW_SELECTION;
      break;
    default:
      break;
  }

  alt_u8 current_sel = (*current_menu)->current_selection;
  cfg_offon_t palmodifications_val = (cfg_offon_t) (cfg_get_value(&pal_awareness,0) & cfg_get_value(&ntsc_pal_selection,0)); // pal modifications e.g. skip vpll entry

  // menu specific modifications

  if (is_vicfg1_screen(*current_menu)) {
    // (pal_awareness_val == OFF) -> skip ntsc-pal-selection (always global)
    if (pal_awareness_val == OFF && current_sel == NSTC_PAL_SUB_SELECTION) (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? NSTC_PAL_SUB_SELECTION + 1 : NSTC_PAL_SUB_SELECTION - 1;
  }

  if (is_vicfg_240p_screen(*current_menu)) {
    if ((current_sel == VPLL_SUBMENU_SELECTION)  && (palmodifications_val == ON))
      (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? VPLL_SUBMENU_SELECTION + 1 : VPLL_SUBMENU_SELECTION - 1;
    current_sel = (*current_menu)->current_selection;

    // do not access scanline options (incl. sl_en) if PASSTRHROUGH mode
    if (cfg_get_value(&linex_240p,0) == PASSTHROUGH)
      (*current_menu)->current_selection = (current_sel < SL_EN_SELECTION) ? current_sel :
                                                (command == CMD_MENU_DOWN) ? LINEX_SELECTION :
                                              (palmodifications_val == ON) ? LINEX_SELECTION : VPLL_SUBMENU_SELECTION;
    // do not access scanline options if sl_en is off
    else if (cfg_get_value(&sl_en,0) == OFF)
      (*current_menu)->current_selection = (current_sel < SL_METHOD_SELECTION) ? current_sel :
                                                    (command == CMD_MENU_DOWN) ? LINEX_SELECTION :
                                                                                 SL_EN_SELECTION;
  }

  if (is_vicfg_480i_screen(*current_menu)) {
    // do not access all options except bob-deinter if PASSTRHROUGH mode
    if (cfg_get_value(&bob_deinter_480i,0) == PASSTHROUGH)
      (*current_menu)->current_selection = LINEX_SELECTION;
    // do not access scanline options if sl_en is off
    else if (cfg_get_value(&sl_en_480i,0) == OFF)
      (*current_menu)->current_selection = current_sel < SL_LINKED_SELECTION ? current_sel :
                                                  (command == CMD_MENU_DOWN) ? LINEX_SELECTION :
                                                                               SL_EN_SELECTION;
  }

  if (is_vicfg_vpll_screen(*current_menu)) {
    if (!vpll_lock) (*current_menu)->current_selection = VPLL_TEST_SELECTION;
  }

  if (is_vicfg_timing_screen(*current_menu)) {
    cfg_timing_model_sel_type_t pagesel = (cfg_timing_model_sel_type_t) cfg_get_value(&timing_selection,0);
    switch (pagesel) {
      case PPU_CURRENT: // check ups for PPU_CURRENT ordered by priority
        if (current_sel == RESET_TIMINGS_SECLECTION) // reset timings not allowed in PPU_CURRENT
          current_sel = (command == CMD_MENU_DOWN) ? TIMING_PAGE_SELECTION : DEJITTER_SELECTION;
        if (((ppu_state & PPU_STATE_PALMODE_GETMASK) >> PPU_STATE_PALMODE_OFFSET) == NTSC && current_sel == DEJITTER_SELECTION) // skip DeJitter in NTSC
          current_sel = (command == CMD_MENU_DOWN) ? TIMING_PAGE_SELECTION : HORSHIFT_SELECTION;
        if (((ppu_state & PPU_STATE_LINEMULT_GETMASK) >> PPU_STATE_LINEMULT_OFFSET) == PASSTHROUGH) { // only allow allow to change page and horizontal in passthrough mode
          if (current_sel == VERTSHIFT_SELECTION) current_sel = (command == CMD_MENU_DOWN) ? HORSHIFT_SELECTION : TIMING_PAGE_SELECTION;
          if (current_sel > HORSHIFT_SELECTION)   current_sel = (command == CMD_MENU_DOWN) ? TIMING_PAGE_SELECTION : HORSHIFT_SELECTION;
        }
        (*current_menu)->current_selection = current_sel;
        break;
      case NTSC_PASSTHROUGH:
      case PAL_PASSTHROUGH:
        if (current_sel == VERTSHIFT_SELECTION) // not allowed to access vertical shift in passthrough
          (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? HORSHIFT_SELECTION : TIMING_PAGE_SELECTION;
        if (current_sel == DEJITTER_SELECTION) // not allowed to access dejitter in passthrough
          (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? RESET_TIMINGS_SECLECTION : HORSHIFT_SELECTION;
        break;
      case NTSC_LX2_PR:
      case NTSC_LX2_INT:
      case NTSC_LX3_PR:
        if (current_sel == DEJITTER_SELECTION) // skip DeJitter in NTSC
          (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? RESET_TIMINGS_SECLECTION : HORSHIFT_SELECTION;
        break;
      case PAL_LX2_PR:
      case PAL_LX2_INT:
      default:
        break;
    }
  }

  if (is_misc_screen(*current_menu) && current_sel == FILTER_ADDON_SELECTION && !use_filteraddon) // skip filter addon options if not present
    // (*current_menu)->current_selection = (command == CMD_MENU_UP) ? 1 : (*current_menu)->number_selections - 1;
    (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? FILTER_ADDON_SELECTION + 1 : FILTER_ADDON_SELECTION - 1;



  if (todo == NEW_OVERLAY || todo == NEW_SELECTION) return todo;


  current_sel = (*current_menu)->current_selection;

  if ((*current_menu)->leaves[current_sel].leavetype == ISUBMENU) {
    switch (command) {
      case CMD_MENU_RIGHT:
      case CMD_MENU_ENTER:
        if ((*current_menu)->leaves[current_sel].submenu) { // check for existing submenu
          if (is_home_menu(*current_menu) && current_sel == VICONFIG_SUBMENU_SELECTION) {
            if (vicfg_page == 1) *current_menu = &vicfg1_screen;  // open vi-config page 1
            else *current_menu = &vicfg2_screen;                  // open vi-config page 2
          } else *current_menu = (*current_menu)->leaves[current_sel].submenu;
          if (is_vicfg1_screen(*current_menu)) vicfg_page = 1;    // remember vi-config page 1
          if (is_vicfg2_screen(*current_menu)) vicfg_page = 2;    // remember vi-config page 1
          return NEW_OVERLAY;
        }
        break;
      default:
        break;
    }
  }

  if ((*current_menu)->leaves[current_sel].leavetype == ICONFIG) {
    switch (command) {
      case CMD_MENU_RIGHT:
        if (is_vicfg_240p_screen((*current_menu)) && (palmodifications_val == ON) && (current_sel == LINEX_SELECTION)) // do not allow linex3 for PAL
          cfg_set_value(&linex_240p,cfg_get_value(&linex_240p,0) == 0);
        else if (is_vicfg_480i_sl_are_linked((*current_menu)) && (current_sel > SL_LINKED_SELECTION)) // modify 240p scanline values if linked
          cfg_inc_value(vicfg_240p_opt_subscreen.leaves[current_sel].config_value);
        else
          cfg_inc_value((*current_menu)->leaves[current_sel].config_value);
        todo = NEW_CONF_VALUE;
        break;
      case CMD_MENU_LEFT:
        if (is_vicfg_240p_screen((*current_menu)) && (palmodifications_val == ON) && (current_sel == LINEX_SELECTION)) // do not allow linex3 for PAL
          cfg_set_value(&linex_240p,cfg_get_value(&linex_240p,0) == 0);
        else if (is_vicfg_480i_sl_are_linked((*current_menu)) && (current_sel > SL_LINKED_SELECTION)) // modify 240p scanline values if linked
          cfg_dec_value(vicfg_240p_opt_subscreen.leaves[current_sel].config_value);
        else
          cfg_dec_value((*current_menu)->leaves[current_sel].config_value);
        todo = NEW_CONF_VALUE;
        break;
      default:
        break;
    }
    if (todo == NEW_CONF_VALUE) {
      if (is_vicfg_timing_screen((*current_menu)) &&
          ((current_sel == VERTSHIFT_SELECTION) || (current_sel == HORSHIFT_SELECTION)) &&
          (cfg_get_value((*current_menu)->leaves[current_sel].config_value,0) == 0)) { // all-zero not allowed for vert./hor. shift
        if (command == CMD_MENU_RIGHT) cfg_inc_value((*current_menu)->leaves[current_sel].config_value);
        else                           cfg_dec_value((*current_menu)->leaves[current_sel].config_value);
      }
    }
    return todo;
  }

  if ((*current_menu)->leaves[current_sel].leavetype == IFUNC) {
    if ((command == CMD_MENU_RIGHT) || (command == CMD_MENU_ENTER)) {
      int retval;

      if (is_misc_screen((*current_menu))) {
//        retval = (*current_menu)->leaves[TESTPATTERN_FUNC_SELECTION].sys_fun_0(sysconfig);
        retval = cfg_show_testpattern(sysconfig);
        return NON;
      }

      if (is_vicfg_vpll_screen((*current_menu))) {
//        retval = (*current_menu)->leaves[VPLL_TEST_SELECTION].sys_fun_0(sysconfig);
        retval = run_vpll_test(sysconfig);
        if (retval == 0) {
          (*current_menu)->current_selection = VPLL_USE_SELECTION;
          print_selection_arrow((*current_menu));
          cfg_offon_t use_vpll_val = (cfg_offon_t) cfg_get_value(&use_vpll,0);
          alt_u8 font_color = (use_vpll_val == cfg_get_value(&use_vpll,use_flash)) ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;
          flag2set_func(use_vpll_val);
          vd_print_string((*current_menu)->leaves[VPLL_USE_SELECTION].arrow_desc->hpos + 3,(*current_menu)->leaves[VPLL_USE_SELECTION].id,BACKGROUNDCOLOR_STANDARD,font_color,&szText[0]);
          return RW_DONE;
        }
        return RW_FAILED;
      }

//      if (is_vicfg_timing_screen((*current_menu))) retval = (*current_menu)->leaves[current_sel].sys_fun_0(sysconfig);
      if (is_vicfg_timing_screen((*current_menu))) retval = cfg_reset_timing(sysconfig);
      else                                         retval = (*current_menu)->leaves[current_sel].sys_fun_1(sysconfig,1); // rw screen
      return (retval == 0                     ? RW_DONE  :
              retval == -CFG_FLASH_SAVE_ABORT ? RW_ABORT :
                                                RW_FAILED);
    }
  }

  return todo;
}

void print_fw_version()
{
  alt_u16 hdl_fw = get_hdl_version();
  sprintf(szText,"%1d.%02d",((hdl_fw & HDL_FW_GETMAIN_MASK) >> HDL_FW_MAIN_OFFSET),
                         ((hdl_fw & HDL_FW_GETSUB_MASK) >> HDL_FW_SUB_OFFSET));
  vd_print_string(VERSION_H_OFFSET,VERSION_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,&szText[0]);

  sprintf(szText,"%1d.%02d",SW_FW_MAIN,SW_FW_SUB);
  vd_print_string(VERSION_H_OFFSET,VERSION_V_OFFSET+1,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,&szText[0]);
};

void print_overlay(menu_t* current_menu)
{
  vmode_t palmode_modify = cfg_get_value(&pal_awareness,0) & cfg_get_value(&ntsc_pal_selection,0);
  alt_u8 h_run;
  alt_u8 overlay_h_offset = (current_menu->type == TEXT) ? TEXTOVERLAY_H_OFFSET :
                    (is_vicfg_vpll_screen(current_menu)) ? TEXTOVERLAY_H_OFFSET : HOMEOVERLAY_H_OFFSET;
  alt_u8 overlay_v_offset = 0;

  VD_CLEAR_SCREEN;

  if (current_menu->header) {
    overlay_v_offset = OVERLAY_V_OFFSET_WH;
    vd_print_string(VD_WIDTH-strlen(*current_menu->header),HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,*current_menu->header);
    if (is_vicfg_240p_screen(current_menu))
      vd_print_string(VD_WIDTH-strlen(*current_menu->header)+VD_240P_HEADER_H_OFFSET,HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,text_240p_288p[palmode_modify]);
    if (is_vicfg_480i_screen(current_menu))
      vd_print_string(VD_WIDTH-strlen(*current_menu->header)+VD_240P_HEADER_H_OFFSET,HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,text_480i_576i[palmode_modify]);

    for (h_run = 0; h_run < VD_WIDTH; h_run++)
      vd_print_char(h_run,1,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_NAVAJOWHITE,(char) HEADER_UNDERLINE);
  }
  vd_print_string(overlay_h_offset,overlay_v_offset,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,*current_menu->overlay);

  if (current_menu->type == HOME) vd_print_string(BTN_OVERLAY_H_OFFSET,BTN_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREEN,btn_overlay_0);
  if (current_menu->type == RWDATA) vd_print_string(BTN_OVERLAY_H_OFFSET,BTN_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREEN,btn_overlay_1);

  switch (current_menu->type) {
    case HOME:
    case CONFIG:
    case VINFO:
    case RWDATA:
      vd_print_string(COPYRIGHT_H_OFFSET,COPYRIGHT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,copyright_note);
      vd_print_char(COPYRIGHT_SIGN_H_OFFSET,COPYRIGHT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,(char) COPYRIGHT_SIGN);
      for (h_run = 0; h_run < VD_WIDTH; h_run++)
        vd_print_char(h_run,VD_HEIGHT-2,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_NAVAJOWHITE,(char) HOME_LOWSEC_UNDERLINE);
      break;
    case TEXT:
      if (&(*current_menu->overlay) == &about_overlay)
        print_fw_version();
      if (&(*current_menu->overlay) == &license_overlay)
        vd_print_char(CR_SIGN_LICENSE_H_OFFSET,CR_SIGN_LICENSE_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,(char) COPYRIGHT_SIGN);
      if (&(*current_menu->overlay) == &welcome_overlay)
        for (h_run = 0; h_run < VD_WIDTH; h_run++)
          vd_print_char(h_run,1,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_NAVAJOWHITE,(char) HEADER_UNDERLINE);
      break;
    default:
      break;
  }

  if (is_vicfg_timing_screen(current_menu))
    vd_print_string(VD_WIDTH-strlen(DeJitterHint),VD_HEIGHT-3,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY,DeJitterHint);
}

void print_selection_arrow(menu_t* current_menu)
{
  alt_u8 h_l_offset, h_r_offset;
  alt_u8 v_run, v_offset;

  for (v_run = 0; v_run < current_menu->number_selections; v_run++)
    if (current_menu->leaves[v_run].arrow_desc != NULL) {
      h_l_offset = current_menu->leaves[v_run].arrow_desc->hpos;
      h_r_offset = current_menu->leaves[v_run].arrow_desc->hpos + (current_menu->leaves[v_run].arrow_desc->shape->right != EMPTY);
      v_offset   = current_menu->leaves[v_run].id;
      if (v_run == current_menu->current_selection) {
        vd_print_char(h_r_offset,v_offset,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,(char) current_menu->leaves[v_run].arrow_desc->shape->right);
        vd_print_char(h_l_offset,v_offset,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,(char) current_menu->leaves[v_run].arrow_desc->shape->left);
      } else {
        vd_clear_char(h_l_offset,v_offset);
        vd_clear_char(h_r_offset,v_offset);
      }
    }
}

int update_vinfo_screen(menu_t* current_menu)
{
  if (current_menu->type != VINFO) return -1;

  alt_u8 str_select;

  // Video Input
  str_select = (palmode << 1) | scanmode;;
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VIN_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VIN_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoMode[str_select]);

  // Video PLL
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VPLL_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VPLL_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoPLL[vpll_lock]);

  // Video Output
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET);
  if (linemult_mode > PASSTHROUGH) str_select = (2*(linemult_mode+1) + palmode);  // in passthrough same string as for video input
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoMode[str_select]);

  // Color Depth
  str_select = ((ppu_state & PPU_STATE_16BIT_MODE_GETMASK) >> PPU_STATE_16BIT_MODE_OFFSET);
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_COL_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_COL_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoColor[str_select]);

  // Video Format
  if ((ppu_state & PPU_STATE_YPBPR_GETMASK) >> PPU_STATE_YPBPR_OFFSET)
    str_select = 2;
  else
    str_select = ((ppu_state & PPU_STATE_RGSB_GETMASK) >> PPU_STATE_RGSB_OFFSET);
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_FORMAT_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_FORMAT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoFormat[str_select]);

  // 240p DeBlur
  vd_clear_lineend(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET);
  if (scanmode == INTERLACED) {
    vd_print_string(INFO_VALS_H_OFFSET,INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY,text_480i_576i_br);
  } else {
    str_select = ((ppu_state & PPU_STATE_DODEBLUR_GETMASK) >> PPU_STATE_DODEBLUR_OFFSET);
    vd_print_string(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,OffOn[str_select]);
  }

  // Filter Add-on
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET);
  if (!use_filteraddon)
    vd_print_string(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY, FilterAddOn[CFG_FILTER_NOT_INSTALLED]);
  else {
    str_select = ((ppu_state & PPU_STATE_FILTER_GETMASK) >> PPU_STATE_FILTER_OFFSET) + 1;
    vd_print_string(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE, FilterAddOn[str_select]);
    if (ppu_state & PPU_STATE_AUTOFILTER_GETMASK)
      vd_print_string(INFO_VALS_H_OFFSET + 8,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE, FilterAddOn[CFG_FILTER_AUTO]);
  }

  return 0;
}

int update_cfg_screen(menu_t* current_menu)
{
  if (current_menu->type != CONFIG) return -1;

  alt_u8 h_l_offset;
  alt_u8 v_run, v_offset;
  alt_u8 background_color, font_color;
  alt_u8 val_select, ref_val_select, val_is_ref;
  cfg_offon_t use_240p_linked_values;

  background_color = BACKGROUNDCOLOR_STANDARD;

  cfg_offon_t pal_awareness_val = (cfg_offon_t) cfg_get_value(&pal_awareness,0);
  vmode_t ntsc_pal_selection_val = (vmode_t) cfg_get_value(&ntsc_pal_selection,0);
  linemult_t linex_240p_val = (linemult_t) cfg_get_value(&linex_240p,0);
  cfg_timing_model_sel_type_t timing_pagesel_val = (cfg_timing_model_sel_type_t) cfg_get_value(&timing_selection,0);


  for (v_run = 0; v_run < current_menu->number_selections; v_run++) {
    h_l_offset = current_menu->leaves[v_run].arrow_desc->hpos + 3;
    v_offset   = current_menu->leaves[v_run].id;

    switch (current_menu->leaves[v_run].leavetype) {
      case ISUBMENU:
        font_color = FONTCOLOR_WHITE;
        vd_print_string(h_l_offset,v_offset,background_color,font_color,EnterSubMenu);
        break;
      case IFUNC:
        if (is_vicfg_vpll_screen(current_menu)) {
//          font_color = vpll_lock ? FONTCOLOR_GREY : FONTCOLOR_WHITE;
          font_color = FONTCOLOR_WHITE;
          vd_print_string(h_l_offset,v_offset,background_color,font_color,StartVPLLTest);
        }
        if (is_misc_screen(current_menu)) {
          font_color = FONTCOLOR_WHITE;
          vd_print_string(h_l_offset,v_offset,background_color,font_color,RunTestPattern);
        }
        if (is_vicfg_timing_screen(current_menu)) {
          font_color = timing_pagesel_val > PPU_CURRENT ? FONTCOLOR_WHITE : FONTCOLOR_GREY;
          vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,v_offset,background_color,font_color,vicfg_timing_opt_overlay2);
          vd_print_string(h_l_offset,v_offset,background_color,font_color,LoadTimingDefaults);
        }
        break;
      case ICONFIG:
        val_select = cfg_get_value(current_menu->leaves[v_run].config_value,0);
        ref_val_select = cfg_get_value(current_menu->leaves[v_run].config_value,use_flash);
        use_240p_linked_values = (cfg_offon_t) (is_vicfg_480i_sl_are_linked(current_menu) && (v_run > SL_LINKED_SELECTION)); // use scanline values for 240p in 480i screen if linked

//        if (current_menu->current_selection == v_run) {
//          background_color = OPT_WINDOWCOLOR_BG;
//          font_color = OPT_WINDOWCOLOR_FONT;
//        } else {
//          background_color = BACKGROUNDCOLOR_STANDARD;
//          font_color = (val_select == ref_val_select) ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;
//        }

        // find special cases and modifications
        if (is_vicfg1_screen(current_menu) && (pal_awareness_val == OFF) && v_run == NSTC_PAL_SUB_SELECTION) {  // show text global if pal awareness is off
          font_color = FONTCOLOR_GREY;
          vd_clear_area(h_l_offset,h_l_offset + OPT_WINDOW_WIDTH,v_offset,v_offset);
          vd_print_string(h_l_offset,v_offset,background_color,font_color,Global);
          break;
        }

        if (is_vicfg2_screen(current_menu) && (v_run == DEBLUR_CURRENT_SELECTION || v_run == M16BIT_CURRENT_SELECTION)) val_is_ref = 1; // current options for deblur and 16bit mode do not have reference values
        else val_is_ref = (val_select == ref_val_select);
        font_color = val_is_ref ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;

        // check 240p and 480i screen
        if (is_vicfg_240p_screen(current_menu))
          if (((linex_240p_val == PASSTHROUGH) && v_run > LINEX_SELECTION) || // gray out every option after linex if linex is off
              ((cfg_get_value(&sl_en,0) == OFF) && v_run > SL_EN_SELECTION))  // gray out every option after sl en if scanlines are off
            font_color = val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;
        if (is_vicfg_480i_screen(current_menu))
          if (((cfg_get_value(&bob_deinter_480i,0) == OFF) && v_run > LINEX_SELECTION) || // gray out every option after linex if linex is off
              ((cfg_get_value(&sl_en_480i,0) == OFF) && v_run > SL_EN_SELECTION))         // gray out every option after sl en if scanlines are off
            font_color = val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;

        // check 480i screen
        if (use_240p_linked_values) {
          val_select     = cfg_get_value(vicfg_240p_opt_subscreen.leaves[v_run].config_value,0);
          ref_val_select = cfg_get_value(vicfg_240p_opt_subscreen.leaves[v_run].config_value,use_flash);
          font_color = (val_select == ref_val_select) ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;
          if (vicfg_240p_opt_subscreen.leaves[v_run].config_value->cfg_type == NUMVALUE) {
            vicfg_240p_opt_subscreen.leaves[v_run].config_value->val2char_func(val_select);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
          } else {
            vd_print_string(h_l_offset,v_offset,background_color,font_color,vicfg_240p_opt_subscreen.leaves[v_run].config_value->value_string[val_select]);
          }
          break;
        }

        // check vpll screen
        if (is_vicfg_vpll_screen(current_menu)) {
          font_color =  vpll_lock ?     font_color :
                       val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;
        }

        // check timing screen
        if (is_vicfg_timing_screen(current_menu)) {
          switch (v_run) {
            case TIMING_PAGE_SELECTION:
              if (timing_pagesel_val > PAL_PASSTHROUGH) print_hv_timing_overlay(LINEX2);
              else if (timing_pagesel_val > PPU_CURRENT) print_hv_timing_overlay(PASSTHROUGH);
              else print_hv_timing_overlay(linemult_mode);
              break;
            case VERTSHIFT_SELECTION:
              switch (timing_pagesel_val) {
                case PPU_CURRENT:
                  if (linemult_mode > PASSTHROUGH) {
                    current_menu->leaves[v_run].config_value->val2char_func(val_select);
                    break;
                  }
                case NTSC_PASSTHROUGH:
                case PAL_PASSTHROUGH:
                  font_color = FONTCOLOR_GREY;
                  sprintf(szText,not_available);
                  break;
                default:
                  current_menu->leaves[v_run].config_value->val2char_func(val_select);
                  break;
              }
              break;
            case HORSHIFT_SELECTION:
              current_menu->leaves[v_run].config_value->val2char_func(val_select);
              break;
            case DEJITTER_SELECTION:
              print_dejitter_overlay(PAL);
              switch (timing_pagesel_val) {
                case PAL_LX2_PR:
                case PAL_LX2_INT:
                  flag2set_func(val_select);
                  break;
                case PPU_CURRENT:
                  if (timing_n64adv > NTSC_LX3_PR) {
                    flag2set_func(val_select);
                    break;
                  }
                default:
                  print_dejitter_overlay(NTSC);
                  font_color = FONTCOLOR_GREY;
                  sprintf(szText,not_available);
                  break;
              }
              break;
            default:
              break;
          }
        }

//        if (v_run == current_menu->current_selection)
          vd_clear_area(h_l_offset,h_l_offset + OPT_WINDOW_WIDTH,v_offset,v_offset);

        // check for filter addon in misc screen
        if (is_misc_screen(current_menu) && v_run == FILTER_ADDON_SELECTION) { // check for filter addon here
          if (!use_filteraddon) vd_print_string(h_l_offset,v_offset-1,background_color,FONTCOLOR_GREY,FilterAddOn[CFG_FILTER_NOT_INSTALLED]);
          else {
            vd_print_string(h_l_offset,v_offset-1,background_color,FONTCOLOR_GREY,FilterAddOn[CFG_FILTER_INSTALLED]);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,FilterAddOn[val_select]);
          }
          break;
        }

        if (current_menu->leaves[v_run].config_value->cfg_type == FLAGTXT ||
          current_menu->leaves[v_run].config_value->cfg_type == NUMVALUE ) {
          if (!(is_vicfg_timing_screen(current_menu))) current_menu->leaves[v_run].config_value->val2char_func(val_select); // val2char_func already executed in timing screen
          vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
        } else {
          vd_print_string(h_l_offset,v_offset,background_color,font_color,current_menu->leaves[v_run].config_value->value_string[val_select]);
        }
        break;
      default:
        break;
    }
  }

  // overlay modifications
  pal_awareness_val &= ntsc_pal_selection_val;  // print PAL strings if both is set
  if (is_vicfg1_screen(current_menu)) {
    vd_print_string(VD_240P_OVERLAY_H_OFFSET,VD_240P_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,text_240p_288p[pal_awareness_val]);
    vd_print_string(VD_240P_OVERLAY_H_OFFSET,VD_480I_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,text_480i_576i[pal_awareness_val]);
  }

  if (is_vicfg_240p_screen(current_menu)) {
    if (pal_awareness_val == ON) {  // do not show VPLL submenu in 288p config
      vd_clear_lineend(0,VICFG_VSUB_VPLL_V_OFFSET);
      return 0;
    }
    h_l_offset = OVERLAY_H_OFFSET + 8;
    v_offset = current_menu->leaves[current_menu->number_selections-1].id + 1;
    if (linex_240p_val == LINEX3) { // check for LineX3 value
      font_color = FONTCOLOR_LIGHTGREY;
      if (cfg_get_value(&use_vpll,0) == OFF)           vd_print_string(VD_WIDTH-strlen(LineX3VPLLHint),v_offset,background_color,font_color,LineX3VPLLHint);
      else if (cfg_get_value(&pal_awareness,0) == OFF) vd_print_string(VD_WIDTH-strlen(LineX3Hint),v_offset,background_color,font_color,LineX3Hint);
      else                                             vd_clear_char(VICFG_VSUB_VALS_H_OFFSET+7,VICFG_VSUB_LINEX_V_OFFSET);
    } else
      vd_clear_lineend(h_l_offset,v_offset);
  }

  return 0;
}
