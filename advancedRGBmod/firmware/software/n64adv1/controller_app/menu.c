/*********************************************************************************
 *
 * This file is part of the N64 RGB/YPbPr DAC project.
 *
 * Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmail.com>
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

menu_t vicfg2_screen = {
    .type = CONFIG,
    .header = &vicfg2_header,
    .overlay = &vicfg2_overlay,
    .parent = &home_menu,
    .current_selection = 6,
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

menu_t vicfg_vpll_subscreen = {
    .type = CONFIG,
    .header = &vicfg_vpll_opt_header,
    .overlay = &vicfg_vpll_opt_overlay,
    .parent = &vicfg_240p_opt_subscreen,
    .current_selection = 0,
    .number_selections = 2,
    .leaves = {
        {.id = VICFG_VPLLSUB_TEST_V_OFFSET, .arrow_desc = &vicfg_vpll_sel_arrow, .leavetype = IFUNC  , .test_fun     = &run_vpll_test},
        {.id = VICFG_VPLLSUB_EN_V_OFFSET  , .arrow_desc = &vicfg_vpll_opt_arrow, .leavetype = ICONFIG, .config_value = &use_vpll}
    }
};

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
        {.id = VICFG_VTIMSUB_RESET_V_OFFSET  , .arrow_desc = &vicfg_timing_sel_arrow  , .leavetype = IFUNC  , .test_fun     = &cfg_reset_timing}
    }
};

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
        {.id = MISC_IGR_15BITMODE_V_OFFSET, .arrow_desc = &misc_opt_arrow, .leavetype = ICONFIG, .config_value = &igr_15bitmode},
        {.id = MISC_FILTERADDON_V_OFFSET, .arrow_desc = &misc_opt_arrow, .leavetype = ICONFIG, .config_value = &filteraddon_cutoff},
        {.id = MISC_SHOWTESTPAT_V_OFFSET, .arrow_desc = &misc_sel_arrow, .leavetype = IFUNC,   .test_fun     = &cfg_show_testpattern}
    }
};

menu_t rwdata_screen = {
    .type = RWDATA,
    .header = &rwdata_header,
    .overlay = &rwdata_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 4,
    .leaves = {
        {.id = RWDATA_SAVE_FL_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .save_fun = &cfg_save_to_flash},
        {.id = RWDATA_LOAD_FL_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .load_fun = &cfg_load_from_flash},
        {.id = RWDATA_LOAD_JS_V_OFFSET , .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .load_fun = &cfg_load_jumperset},
        {.id = RWDATA_LOAD_N64_V_OFFSET, .arrow_desc = &front_sel_arrow, .leavetype = IFUNC, .load_fun = &cfg_load_defaults}
    }
};

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
static inline alt_u8 is_vicfg_vpll_screen (menu_t *menu)
  {  return (menu == &vicfg_vpll_subscreen); }
static inline alt_u8 is_vicfg_timing_screen (menu_t *menu)
  {  return (menu == &vicfg_timing_subscreen); }
static inline alt_u8 is_vicfg_480i_screen (menu_t *menu)
  {  return (menu  == &vicfg_480i_opt_subscreen); }
static inline alt_u8 is_vicfg_480i_sl_are_linked (menu_t *menu)
  {  return ( is_vicfg_480i_screen(menu) && cfg_get_value(menu->leaves[3].config_value,0)); }
static inline alt_u8 is_misc_screen (menu_t *menu)
  {  return (menu == &misc_screen); }


void print_timing_overlay(alt_u8 lx_mode) {
  alt_u8 font_color = lx_mode ? FONTCOLOR_WHITE : FONTCOLOR_GREY;
  vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,VICFG_VTIMSUB_VSHIFT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,font_color,vicfg_timing_opt_overlay0);
}
void print_dejitter_overlay(alt_u8 palmode) {
  alt_u8 font_color = palmode ? FONTCOLOR_WHITE : FONTCOLOR_GREY;
  vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,VICFG_VTIMSUB_PALDEJ_V_OFFSET,BACKGROUNDCOLOR_STANDARD,font_color,vicfg_timing_opt_overlay1);
}


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


updateaction_t modify_menu(cmd_t command, menu_t* *current_menu, configuration_t* sysconfig, alt_u16* ppu_state)
{
  static alt_u8 vicfg_page = 1;

  if (command == CMD_MUTE_MENU) {
    return MENU_MUTE;
  }
  if (command == CMD_UNMUTE_MENU) {
    return MENU_UNMUTE;
  }

  if (command == CMD_CLOSE_MENU) {
    while ((*current_menu)->parent) {
      (*current_menu)->current_selection = 0;
      *current_menu = (*current_menu)->parent;
    }
    (*current_menu)->current_selection = 1;
    return MENU_CLOSE;
  }

  if (command == CMD_MENU_BACK) {
    if ((*current_menu)->parent) {
      (*current_menu)->current_selection = 0;
      *current_menu = (*current_menu)->parent;
      return NEW_OVERLAY;
    } else {
      (*current_menu)->current_selection = 1;
      return MENU_CLOSE;
    }
  }

  if (command == CMD_MENU_PAGE_RIGHT) {
    if (is_vicfg1_screen(*current_menu)) {
      *current_menu = &vicfg2_screen;
      vicfg_page = 2;
      return NEW_OVERLAY;
    }
    if (is_vicfg2_screen(*current_menu)) {
      *current_menu = &vicfg1_screen;
      vicfg_page = 1;
      return NEW_OVERLAY;
    }
    if (is_vicfg_timing_screen(*current_menu)) {
      cfg_inc_value((*current_menu)->leaves[0].config_value);
      return NEW_CONF_VALUE;
    }
  }

  if (command == CMD_MENU_PAGE_LEFT) {
    if (is_vicfg_timing_screen(*current_menu)) {
      cfg_dec_value((*current_menu)->leaves[0].config_value);
      return NEW_CONF_VALUE;
    }
  }

  if (((*current_menu)->type == TEXT) ||
      ((*current_menu)->type == VINFO)  )
    return NON;

  alt_u8 pal_awareness_val = cfg_get_value(&pal_awareness,0);
  alt_u8 palmode = cfg_get_value(&ntsc_pal_selection,0);

  updateaction_t todo = NON;
  alt_u8 pre_sel = (*current_menu)->current_selection;

  switch (command) {
    case CMD_MENU_DOWN:
      (*current_menu)->current_selection++;
      if ((*current_menu)->current_selection == (*current_menu)->number_selections)
        (*current_menu)->current_selection = 0;
      if (((*current_menu)->current_selection == 1) && ((is_vicfg1_screen(*current_menu) && !pal_awareness_val) ||
                                                        (is_vicfg_240p_screen(*current_menu) && palmode)
                                                        ))
        (*current_menu)->current_selection++;
      todo = NEW_SELECTION;
      break;
    case CMD_MENU_UP:
      if ((*current_menu)->current_selection == 0)
        (*current_menu)->current_selection =  (*current_menu)->number_selections - 1;
      else {
        (*current_menu)->current_selection--;
        if (((*current_menu)->current_selection == 1) && ((is_vicfg1_screen(*current_menu) && !pal_awareness_val) ||
                                                          (is_vicfg_240p_screen(*current_menu) && palmode)
                                                         ))
          (*current_menu)->current_selection--;
      }
      todo = NEW_SELECTION;
      break;
    default:
      break;
  }
  if (is_vicfg_timing_screen(*current_menu)) {
    alt_u8 pagesel = cfg_get_value((*current_menu)->leaves[0].config_value,0);
    if (pagesel == PPU_CURRENT) {
      if (((*ppu_state & PPU_STATE_LINEMULT_GETMASK) >> PPU_STATE_LINEMULT_OFFSET) == 0) {
        (*current_menu)->current_selection = 0;
        todo = NON;
      }
      if (((((*ppu_state & PPU_STATE_PALMODE_GETMASK) >> PPU_STATE_PALMODE_OFFSET) == NTSC) && ((*current_menu)->current_selection == 3)) || ((*current_menu)->current_selection == 4))
          (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? 0 : 2;
    } else if ((pagesel < PAL_LX2_PR) && ((*current_menu)->current_selection == 3)) {
        (*current_menu)->current_selection = (command == CMD_MENU_DOWN) ? 4 : 2;
    }
  }

  if (is_vicfg_vpll_screen(*current_menu)) {
    if (!vpll_lock) (*current_menu)->current_selection = 0;
    todo = (pre_sel == (*current_menu)->current_selection) ? NON : NEW_SELECTION;
  }

  alt_u8 sel = (*current_menu)->current_selection;

  if (todo == NEW_SELECTION) {
    if (is_vicfg_240p_screen(*current_menu)) {
      if (!cfg_get_value((*current_menu)->leaves[0].config_value,0))
        (*current_menu)->current_selection =                  sel < 2 ? sel :
                                             (command == CMD_MENU_UP) ?   1 : 0;
      else if (!cfg_get_value((*current_menu)->leaves[2].config_value,0))
        (*current_menu)->current_selection =                  sel < 3 ? sel :
                                             (command == CMD_MENU_UP) ?   2 : 0;
    }

    if (is_vicfg_480i_screen(*current_menu)) {
      if (!cfg_get_value((*current_menu)->leaves[0].config_value,0))
        (*current_menu)->current_selection = 0;
      else if (!cfg_get_value((*current_menu)->leaves[2].config_value,0))
        (*current_menu)->current_selection =                  sel < 3 ? sel :
                                             (command == CMD_MENU_UP) ?   2 : 0;
    }

    if (is_misc_screen(*current_menu) && sel == 4 && !use_filteraddon)
      // (*current_menu)->current_selection = (command == CMD_MENU_UP) ? 1 : (*current_menu)->number_selections - 1;
      (*current_menu)->current_selection = (command == CMD_MENU_UP) ? sel - 1 : sel + 1;

    return todo;
  }

  if ((*current_menu)->leaves[sel].leavetype == ISUBMENU) {
    switch (command) {
      case CMD_MENU_ENTER:
      case CMD_MENU_RIGHT:
        if ((*current_menu)->leaves[sel].submenu) {
          if (is_home_menu(*current_menu) && sel == 1) {
            if (vicfg_page == 1) *current_menu = &vicfg1_screen;
            else *current_menu = &vicfg2_screen;
          } else *current_menu = (*current_menu)->leaves[sel].submenu;
          if (is_vicfg1_screen(*current_menu)) vicfg_page = 1;
          if (is_vicfg2_screen(*current_menu)) vicfg_page = 2;
          return NEW_OVERLAY;
        }
        break;
      default:
        break;
    }
  }

  if ((*current_menu)->leaves[sel].leavetype == ICONFIG) {
    alt_u8 modify_overlay_and_new_cfg = 0;

    switch (command) {
      case CMD_MENU_RIGHT:
        if (is_vicfg_240p_screen((*current_menu)) && palmode && sel == 0 && cfg_get_value((*current_menu)->leaves[sel].config_value,0) == 1)
          cfg_dec_value((*current_menu)->leaves[sel].config_value);
        else if (is_vicfg_480i_sl_are_linked((*current_menu)) && (sel > 3))
          cfg_inc_value(vicfg_240p_opt_subscreen.leaves[sel].config_value);
        else
          cfg_inc_value((*current_menu)->leaves[sel].config_value);
        if (is_vicfg1_screen((*current_menu)) && sel < 2) {
            modify_overlay_and_new_cfg = 1;
            break;
        }
        if (is_vicfg_timing_screen((*current_menu)) && (sel == 1 || sel == 2) && cfg_get_value((*current_menu)->leaves[sel].config_value,0) == 0)
          cfg_inc_value((*current_menu)->leaves[sel].config_value); // all-zero not allowed for vert./hor. shift
        return NEW_CONF_VALUE;
      case CMD_MENU_LEFT:
        if (is_vicfg_240p_screen((*current_menu)) && palmode && sel == 0 && cfg_get_value((*current_menu)->leaves[sel].config_value,0) == 0)
          cfg_inc_value((*current_menu)->leaves[sel].config_value);
        else if (is_vicfg_480i_sl_are_linked((*current_menu)) && (sel > 3))
          cfg_dec_value(vicfg_240p_opt_subscreen.leaves[sel].config_value);
        else
          cfg_dec_value((*current_menu)->leaves[sel].config_value);
        if (is_vicfg1_screen((*current_menu)) && sel < 2) {
            modify_overlay_and_new_cfg = 1;
            break;
        }
        if (is_vicfg_timing_screen((*current_menu)) && (sel == 1 || sel == 2) && cfg_get_value((*current_menu)->leaves[sel].config_value,0) == 0)
          cfg_dec_value((*current_menu)->leaves[sel].config_value); // all-zero not allowed for vert./hor. shift
        return NEW_CONF_VALUE;
      default:
        break;
    }

    if (modify_overlay_and_new_cfg) {
      palmode = cfg_get_value(&pal_awareness,0) * cfg_get_value(&ntsc_pal_selection,0);
      vd_print_string(VD_240P_OVERLAY_H_OFFSET,VD_240P_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,text_240p_288p[palmode]);
      vd_print_string(VD_240P_OVERLAY_H_OFFSET,VD_480I_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,text_480i_576i[palmode]);
      return NEW_CONF_VALUE;
    }
  }

  if ((*current_menu)->leaves[sel].leavetype == IFUNC) {
    if ((command == CMD_MENU_RIGHT) || (command == CMD_MENU_ENTER)) {
      int retval;

      if (is_misc_screen((*current_menu))) {
        retval = (*current_menu)->leaves[sel].test_fun(sysconfig);
        return NON;
      }

      if (is_vicfg_vpll_screen((*current_menu))) {
        retval = (*current_menu)->leaves[sel].test_fun(sysconfig);
        if (retval == 0) {
          (*current_menu)->current_selection = 1;
          print_selection_arrow((*current_menu));
          alt_u8 font_color = (cfg_get_value((*current_menu)->leaves[1].config_value,0) == cfg_get_value((*current_menu)->leaves[1].config_value,use_flash)) ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;
          (*current_menu)->leaves[1].config_value->val2char_func(cfg_get_value((*current_menu)->leaves[1].config_value,0));
          vd_print_string((*current_menu)->leaves[1].arrow_desc->hpos + 3,(*current_menu)->leaves[1].id,BACKGROUNDCOLOR_STANDARD,font_color,&szText[0]);
          return RW_DONE;
        }
        return RW_FAILED;
      }

      retval = (*current_menu)->leaves[sel].load_fun(sysconfig,1);
      return (retval == 0                     ? RW_DONE  :
              retval == -CFG_FLASH_SAVE_ABORT ? RW_ABORT :
                                                RW_FAILED);
    }
  }

  return NON;
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
  alt_u8 palmode = cfg_get_value(&ntsc_pal_selection,0);
  alt_u8 h_run;
  alt_u8 overlay_h_offset =       (current_menu->type == TEXT) ? TEXTOVERLAY_H_OFFSET :
                            (is_vicfg_vpll_screen(current_menu)) ? TEXTOVERLAY_H_OFFSET : HOMEOVERLAY_H_OFFSET;
  alt_u8 overlay_v_offset = 0;

  VD_CLEAR_SCREEN;

  if (current_menu->header) {
    overlay_v_offset = OVERLAY_V_OFFSET_WH;
    vd_print_string(VD_WIDTH-strlen(*current_menu->header),HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,*current_menu->header);
    if (is_vicfg_240p_screen(current_menu))
      vd_print_string(VD_WIDTH-strlen(*current_menu->header)+VD_240P_HEADER_H_OFFSET,HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,text_240p_288p[palmode]);
    if (is_vicfg_480i_screen(current_menu))
      vd_print_string(VD_WIDTH-strlen(*current_menu->header)+VD_240P_HEADER_H_OFFSET,HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,text_480i_576i[palmode]);

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

  if (is_vicfg1_screen(current_menu)) {
    vd_print_string(VD_240P_OVERLAY_H_OFFSET,VD_240P_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,text_240p_288p[palmode]);
    vd_print_string(VD_240P_OVERLAY_H_OFFSET,VD_480I_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,text_480i_576i[palmode]);
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

int update_vinfo_screen(menu_t* current_menu, alt_u16* ppu_state)
{
  if (current_menu->type != VINFO) return -1;

  alt_u8 str_select;

  // Video Input
  str_select = ((*ppu_state & (PPU_STATE_PALMODE_GETMASK | PPU_STATE_480I_GETMASK)) >> PPU_STATE_480I_OFFSET);
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VIN_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VIN_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoMode[str_select]);

  // Video PLL
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VPLL_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VPLL_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoPLL[vpll_lock]);

  // Video Output
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET);
  switch (((*ppu_state & PPU_STATE_PALMODE_GETMASK) >> (PPU_STATE_PALMODE_OFFSET - 2)) | ((*ppu_state & PPU_STATE_LINEMULT_GETMASK) >> PPU_STATE_LINEMULT_OFFSET)) {
  /* order: PAL mode, 2x line mult */
    case 0x1:
      str_select = 4;
      break;
    case 0x2:
      str_select = 6;
      break;
    case 0x5:
      str_select = 5;
      break;
    case 0x0:
    case 0x4:
    default:
      break;
  }
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoMode[str_select]);

  // Color Depth
  str_select = ((*ppu_state & PPU_STATE_16BIT_MODE_GETMASK) >> PPU_STATE_16BIT_MODE_OFFSET);
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_COL_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_COL_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoColor[str_select]);

  // Video Format
  if ((*ppu_state & PPU_STATE_YPBPR_GETMASK) >> PPU_STATE_YPBPR_OFFSET)
    str_select = 2;
  else
    str_select = ((*ppu_state & PPU_STATE_RGSB_GETMASK) >> PPU_STATE_RGSB_OFFSET);
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_FORMAT_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_FORMAT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoFormat[str_select]);

  // 240p DeBlur
  vd_clear_lineend(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET);
  if (*ppu_state & PPU_STATE_480I_GETMASK) {
    vd_print_string(INFO_VALS_H_OFFSET,INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY,text_480i_576i_br);
  } else {
    str_select = ((*ppu_state & PPU_STATE_DODEBLUR_GETMASK) >> PPU_STATE_DODEBLUR_OFFSET);
    vd_print_string(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,OffOn[str_select]);
  }

  // Filter Add-on
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET);
  if (!use_filteraddon)
    vd_print_string(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY, FilterAddOn[CFG_FILTER_NOT_INSTALLED]);
  else {
    str_select = ((*ppu_state & PPU_STATE_FILTER_GETMASK) >> PPU_STATE_FILTER_OFFSET) + 1;
    vd_print_string(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE, FilterAddOn[str_select]);
    if (*ppu_state & PPU_STATE_AUTOFILTER_GETMASK)
      vd_print_string(INFO_VALS_H_OFFSET + 8,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE, FilterAddOn[0]);
  }

  return 0;
}

int update_cfg_screen(menu_t* current_menu, alt_u8 linemode, alt_u8 timing_current)
{
  if (current_menu->type != CONFIG) return -1;

  alt_u8 h_l_offset;
  alt_u8 v_run, v_offset;
  alt_u8 background_color, font_color;
  alt_u8 val_select, ref_val_select, val_is_ref;
  alt_u8 use_240p_linked_values;

  background_color = BACKGROUNDCOLOR_STANDARD;

  alt_u8 timing_pagesel = cfg_get_value(current_menu->leaves[0].config_value,0);


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
          font_color = timing_pagesel ? FONTCOLOR_WHITE : FONTCOLOR_GREY;
          vd_print_string(VICFG_VTIMSUB_OVERLAY_H_OFFSET+3,v_offset,background_color,font_color,vicfg_timing_opt_overlay2);
          vd_print_string(h_l_offset,v_offset,background_color,font_color,LoadTimingDefaults);
        }
        break;
      case ICONFIG:
        use_240p_linked_values = is_vicfg_480i_sl_are_linked(current_menu) && (v_run > 3);
        if (use_240p_linked_values) {
          val_select     = cfg_get_value(vicfg_240p_opt_subscreen.leaves[v_run].config_value,0);
          ref_val_select = cfg_get_value(vicfg_240p_opt_subscreen.leaves[v_run].config_value,use_flash);
        } else {
          val_select     = cfg_get_value(current_menu->leaves[v_run].config_value,0);
          ref_val_select = cfg_get_value(current_menu->leaves[v_run].config_value,use_flash);
        }

//        if (current_menu->current_selection == v_run) {
//          background_color = OPT_WINDOWCOLOR_BG;
//          font_color = OPT_WINDOWCOLOR_FONT;
//        } else {
//          background_color = BACKGROUNDCOLOR_STANDARD;
//          font_color = (val_select == ref_val_select) ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;
//        }
        if (is_vicfg2_screen(current_menu) && (v_run == 2 || v_run == 4)) val_is_ref = 1;
        else val_is_ref = (val_select == ref_val_select);
        font_color = val_is_ref ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;

        if (is_vicfg1_screen(current_menu) && !cfg_get_value(&pal_awareness,0) && v_run == 1) {
          font_color = FONTCOLOR_GREY;
          vd_clear_area(h_l_offset,h_l_offset + OPT_WINDOW_WIDTH,v_offset,v_offset);
          vd_print_string(h_l_offset,v_offset,background_color,font_color,Global);
          break;
        }

        if (is_vicfg_240p_screen(current_menu))
          if ((!cfg_get_value(current_menu->leaves[0].config_value,0) && v_run > 0) ||
              (!cfg_get_value(current_menu->leaves[2].config_value,0) && v_run > 2))
            font_color = val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;

        if (is_vicfg_vpll_screen(current_menu)) {
          font_color =  vpll_lock ?     font_color :
                       val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;
        }

        if (is_vicfg_480i_screen(current_menu))
          if ((!cfg_get_value(current_menu->leaves[0].config_value,0) && v_run > 0) ||
              (!cfg_get_value(current_menu->leaves[2].config_value,0) && v_run > 2))
            font_color = val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;

        if (is_vicfg_timing_screen(current_menu)) {
          if (v_run == 0) {
            if (timing_pagesel > PPU_CURRENT) print_timing_overlay(1);
            else print_timing_overlay(linemode);
          } else if (v_run == 3) {
            print_dejitter_overlay(1);
            switch (timing_pagesel) {
              case PPU_CURRENT:
                if (timing_current > NTSC_LX3_PR) {
                  current_menu->leaves[3].config_value->val2char_func(val_select);
                  break;
                }
              case NTSC_LX2_PR:
              case NTSC_LX2_INT:
              case NTSC_LX3_PR:
                print_dejitter_overlay(0);
                font_color = FONTCOLOR_GREY;
                sprintf(szText,not_available);
                break;
              case PAL_LX2_PR:
              case PAL_LX2_INT:
                current_menu->leaves[3].config_value->val2char_func(val_select);
                break;
              default:
                break;
              }
          } else {
            if ((timing_pagesel == PPU_CURRENT) && (linemode == 0)) {
              font_color = FONTCOLOR_GREY;
              sprintf(szText,not_available);
            } else {
              current_menu->leaves[v_run].config_value->val2char_func(val_select);
            }
          }
          vd_clear_area(h_l_offset,h_l_offset + OPT_WINDOW_WIDTH,v_offset,v_offset);
          vd_print_string(h_l_offset,v_offset,background_color,font_color,Global);
        }

//        if (v_run == current_menu->current_selection)
          vd_clear_area(h_l_offset,h_l_offset + OPT_WINDOW_WIDTH,v_offset,v_offset);

        if (is_misc_screen(current_menu) && v_run == 2) {
          if (current_menu->leaves[v_run].config_value->cfg_type == FLAGTXT ||
              current_menu->leaves[v_run].config_value->cfg_type == NUMVALUE ) {
            current_menu->leaves[v_run].config_value->val2char_func(val_select);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
          } else {
            if (!use_filteraddon) vd_print_string(h_l_offset-7,v_offset-1,background_color,FONTCOLOR_GREY,FilterAddOn[6]);
            else {
              vd_print_string(h_l_offset,v_offset-1,background_color,FONTCOLOR_GREY,FilterAddOn[5]);
              vd_print_string(h_l_offset,v_offset,background_color,font_color,FilterAddOn[val_select]);
            }
          }
        } else if (use_240p_linked_values) {
          if (vicfg_240p_opt_subscreen.leaves[v_run].config_value->cfg_type == NUMVALUE) {
            vicfg_240p_opt_subscreen.leaves[v_run].config_value->val2char_func(val_select);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
          } else {
            vd_print_string(h_l_offset,v_offset,background_color,font_color,vicfg_240p_opt_subscreen.leaves[v_run].config_value->value_string[val_select]);
          }
        } else {
          if (current_menu->leaves[v_run].config_value->cfg_type == FLAGTXT ||
              current_menu->leaves[v_run].config_value->cfg_type == NUMVALUE ) {
            if (!(is_vicfg_timing_screen(current_menu))) current_menu->leaves[v_run].config_value->val2char_func(val_select);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
          } else {
            vd_print_string(h_l_offset,v_offset,background_color,font_color,current_menu->leaves[v_run].config_value->value_string[val_select]);
          }
        }
        break;
      default:
        break;
    }
  }

  if (is_vicfg_240p_screen(current_menu)) {
    if (cfg_get_value(&ntsc_pal_selection,0)) {
      vd_clear_lineend(0,VICFG_VSUB_VPLL_V_OFFSET);
      return 0;
    }
    h_l_offset = OVERLAY_H_OFFSET + 8;
    v_offset = current_menu->leaves[current_menu->number_selections-1].id + 1;
    if (cfg_get_value(current_menu->leaves[0].config_value,0) == 2) { // check for LineX3 value
      font_color = FONTCOLOR_LIGHTGREY;
      if (cfg_get_value(&use_vpll,0) == 0) vd_print_string(VD_WIDTH-strlen(LineX3VPLLHint),v_offset,background_color,font_color,LineX3VPLLHint);
      else                                 vd_print_string(VD_WIDTH-strlen(LineX3Hint),v_offset,background_color,font_color,LineX3Hint);
    } else
      vd_clear_lineend(h_l_offset,v_offset);
  }

  return 0;
}

void print_current_mode(alt_u8 palmode, alt_u8 linemode, alt_u8 timing_current)
{
  if (linemode == 0) sprintf(szText,"Current: %s",VTimingPT[palmode]);
  else               sprintf(szText,"Current: %s",VTimingSel[timing_current]);
  vd_print_string(0, VD_HEIGHT-1, BACKGROUNDCOLOR_STANDARD, FONTCOLOR_NAVAJOWHITE, &szText[0]);
}
