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

static const arrow_t cfg_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (CFG_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (CFG_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_240p480i_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (CFG_VSUB_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_240p_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (CFG_VSUB_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_deblur_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (CFG_DBSUB_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_vpll_opt_arrow = {
    .shape = &optval_arrow,
    .hpos = (CFG_VPLLSUB_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_vpll_sel_arrow = {
    .shape = &selection_arrow,
    .hpos = (CFG_VPLLSUB_VALS_H_OFFSET - 2)
};

static const arrow_t misc_screen_arrow = {
    .shape = &selection_arrow,
    .hpos = (MISC_VALS_H_OFFSET - 2)
};

menu_t home_menu, vinfo_screen, cfg_screen, cfg_240p_opt_subscreen, cfg_480i_opt_subscreen, advdeblur_cfg_subscreen,
       cfg_vpll_subscreen, misc_screen, rwdata_screen, about_screen, thanks_screen, license_screen;

extern config_t vformat, deblur_mode, mode15bit, gamma_lut;
extern config_t deblur_p2p_sense, deblur_frame_cnt_high, deblur_frame_cnt_low, deblur_rst_behavior, deblur_stability;
extern config_t linex_240p, sl_en, sl_method, sl_id, sl_str, slhyb_str;
extern config_t bob_deinter_480i, field_shift_fix_480i, sl_en_480i, sl_link_480i, sl_id_480i, sl_str_480i, slhyb_str_480i;
extern config_t use_vpll;
extern config_t igr_reset, igr_quickchange, filteraddon_cutoff;


menu_t home_menu = {
    .type = HOME,
    .header  = &home_header,
    .overlay = &home_overlay,
    .current_selection = 1,
    .number_selections = 7,
    .leaves = {
        {.id = MAIN2VINFO_V_OFFSET  , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &vinfo_screen},
        {.id = MAIN2CFG_V_OFFSET    , .arrow_desc = &front_sel_arrow, .leavetype = ISUBMENU, .submenu = &cfg_screen},
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

menu_t cfg_screen = {
    .type = CONFIG,
    .header = &cfg_header,
    .overlay = &cfg_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = CFG_240P_SET_V_OFFSET  , .arrow_desc = &cfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &cfg_240p_opt_subscreen},
        {.id = CFG_480I_SET_V_OFFSET  , .arrow_desc = &cfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &cfg_480i_opt_subscreen},
        {.id = CFG_FORMAT_V_OFFSET    , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &vformat},
        {.id = CFG_DEBLURMODE_V_OFFSET, .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_mode},
        {.id = CFG_DEBLURADV_V_OFFSET , .arrow_desc = &cfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &advdeblur_cfg_subscreen},
        {.id = CFG_15BIT_V_OFFSET     , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &mode15bit},
        {.id = CFG_GAMMA_V_OFFSET     , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &gamma_lut}
    }
};

menu_t cfg_240p_opt_subscreen = {
    .type = CONFIG,
    .header = &cfg_240p_opt_header,
    .overlay = &cfg_240p_opt_overlay,
    .parent = &cfg_screen,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = CFG_VSUB_LINEX_V_OFFSET    , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &linex_240p},
        {.id = CFG_VSUB_VPLL_V_OFFSET     , .arrow_desc = &cfg_240p_sel_arrow    , .leavetype = ISUBMENU, .submenu      = &cfg_vpll_subscreen},
        {.id = CFG_VSUB_SL_EN_V_OFFSET    , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_en},
        {.id = CFG_VSUB_SL_METHOD_V_OFFSET, .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_method},
        {.id = CFG_VSUB_SL_ID_V_OFFSET    , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_id},
        {.id = CFG_VSUB_SL_STR_V_OFFSET   , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_str},
        {.id = CFG_VSUB_SLHYB_STR_V_OFFSET, .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &slhyb_str}
    }
};


menu_t cfg_480i_opt_subscreen = {
    .type = CONFIG,
    .header = &cfg_480i_opt_header,
    .overlay = &cfg_480i_opt_overlay,
    .parent = &cfg_screen,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = CFG_VSUB_LINEX_V_OFFSET    , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &bob_deinter_480i},
        {.id = CFG_VSUB_FIELDFIX_V_OFFSET , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &field_shift_fix_480i},
        {.id = CFG_VSUB_SL_EN_V_OFFSET    , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_en_480i},
        {.id = CFG_VSUB_SL_METHOD_V_OFFSET, .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_link_480i},
        {.id = CFG_VSUB_SL_ID_V_OFFSET    , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_id_480i},
        {.id = CFG_VSUB_SL_STR_V_OFFSET   , .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_str_480i},
        {.id = CFG_VSUB_SLHYB_STR_V_OFFSET, .arrow_desc = &cfg_240p480i_opt_arrow, .leavetype = ICONFIG , .config_value = &slhyb_str_480i}
    }
};

menu_t advdeblur_cfg_subscreen = {
    .type = CONFIG,
    .header = &cfg_deblur_opt_header,
    .overlay = &cfg_deblur_opt_overlay,
    .parent = &cfg_screen,
    .current_selection = 0,
    .number_selections = 5,
    .leaves = {
        {.id = CFG_DBSUB_P2P_V_OFFSET    , .arrow_desc = &cfg_deblur_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_p2p_sense},
        {.id = CFG_DBSUB_DBCNT_V_OFFSET  , .arrow_desc = &cfg_deblur_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_frame_cnt_high},
        {.id = CFG_DBSUB_DBCNTDZ_V_OFFSET, .arrow_desc = &cfg_deblur_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_frame_cnt_low},
        {.id = CFG_DBSUB_DBSTAB_V_OFFSET , .arrow_desc = &cfg_deblur_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_stability},
        {.id = CFG_DBSUB_DBRST_V_OFFSET  , .arrow_desc = &cfg_deblur_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur_rst_behavior},
    }
};

menu_t cfg_vpll_subscreen = {
    .type = CONFIG,
    .header = &cfg_vpll_opt_header,
    .overlay = &cfg_vpll_opt_overlay,
    .parent = &cfg_240p_opt_subscreen,
    .current_selection = 0,
    .number_selections = 2,
    .leaves = {
        {.id = CFG_VPLLSUB_TEST_V_OFFSET, .arrow_desc = &cfg_vpll_sel_arrow, .leavetype = IFUNC  , .test_fun     = &run_vpll_test},
        {.id = CFG_VPLLSUB_EN_V_OFFSET  , .arrow_desc = &cfg_vpll_opt_arrow, .leavetype = ICONFIG, .config_value = &use_vpll}
    }
};

menu_t misc_screen = {
    .type = CONFIG,
    .header = &misc_header,
    .overlay = &misc_overlay,
    .parent = &home_menu,
    .current_selection = 0,
    .number_selections = 4,
    .leaves = {
        {.id = MISC_IGR_RESET_V_OFFSET  , .arrow_desc = &misc_screen_arrow, .leavetype = ICONFIG, .config_value = &igr_reset},
        {.id = MISC_IGR_QUICK_V_OFFSET  , .arrow_desc = &misc_screen_arrow, .leavetype = ICONFIG, .config_value = &igr_quickchange},
        {.id = MISC_FILTERADDON_V_OFFSET, .arrow_desc = &misc_screen_arrow, .leavetype = ICONFIG, .config_value = &filteraddon_cutoff},
        {.id = MISC_SHOWTESTPAT_V_OFFSET, .arrow_desc = &front_sel_arrow,   .leavetype = IFUNC,   .test_fun     = &cfg_show_testpattern}
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


inline alt_u8 is_cfg_240p_screen (menu_t *menu)
  {  return (menu == &cfg_240p_opt_subscreen); }
inline alt_u8 is_cfg_vpll_screen (menu_t *menu)
  {  return (menu == &cfg_vpll_subscreen); }
inline alt_u8 is_cfg_480i_screen (menu_t *menu)
  {  return (menu  == &cfg_480i_opt_subscreen); }
inline alt_u8 cfg_480i_sl_are_linked (menu_t *menu)
  {  return ( is_cfg_480i_screen(menu) && cfg_get_value(menu->leaves[3].config_value,0)); }
inline alt_u8 is_deblur_screen (menu_t *menu)
  {  return (menu == &advdeblur_cfg_subscreen); }
inline alt_u8 is_misc_screen (menu_t *menu)
  {  return (menu == &misc_screen); }


updateaction_t modify_menu(cmd_t command, menu_t* *current_menu, configuration_t* sysconfig)
{
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

  if (((*current_menu)->type == TEXT) ||
      ((*current_menu)->type == VINFO)  )
    return NON;

  updateaction_t todo = NON;
  alt_u8 pre_sel = (*current_menu)->current_selection;

  switch (command) {
    case CMD_MENU_DOWN:
      (*current_menu)->current_selection++;
      if ((*current_menu)->current_selection == (*current_menu)->number_selections)
        (*current_menu)->current_selection = 0;
      todo = NEW_SELECTION;
      break;
    case CMD_MENU_UP:
      if ((*current_menu)->current_selection == 0)
        (*current_menu)->current_selection =  (*current_menu)->number_selections - 1;
      else
        (*current_menu)->current_selection--;
      todo = NEW_SELECTION;
      break;
    default:
      break;
  }

  if (is_cfg_vpll_screen(*current_menu)) {
    if (!vpll_lock) (*current_menu)->current_selection = 0;
    else            (*current_menu)->current_selection = 1;
    todo = (pre_sel == (*current_menu)->current_selection) ? NON : NEW_SELECTION;
  }

  alt_u8 sel = (*current_menu)->current_selection;

  if (todo == NEW_SELECTION) {
    if (is_cfg_240p_screen(*current_menu)) {
      if (!cfg_get_value((*current_menu)->leaves[0].config_value,0))
        (*current_menu)->current_selection =                  sel < 2 ? sel :
                                             (command == CMD_MENU_UP) ?   1 : 0;
      else if (!cfg_get_value((*current_menu)->leaves[2].config_value,0))
        (*current_menu)->current_selection =                  sel < 3 ? sel :
                                             (command == CMD_MENU_UP) ?   2 : 0;
    }

    if (is_cfg_480i_screen(*current_menu)) {
      if (!cfg_get_value((*current_menu)->leaves[0].config_value,0))
        (*current_menu)->current_selection = 0;
      else if (!cfg_get_value((*current_menu)->leaves[2].config_value,0))
        (*current_menu)->current_selection =                  sel < 3 ? sel :
                                             (command == CMD_MENU_UP) ?   2 : 0;
    }

    if (is_misc_screen(*current_menu) && sel == 2 && !use_filteraddon)
      // (*current_menu)->current_selection = (command == CMD_MENU_UP) ? 1 : (*current_menu)->number_selections - 1;
      (*current_menu)->current_selection = (command == CMD_MENU_UP) ? 1 : 3;

    return todo;
  }

  if ((*current_menu)->leaves[sel].leavetype == ISUBMENU) {
    switch (command) {
      case CMD_MENU_RIGHT:
      case CMD_MENU_ENTER:
        if ((*current_menu)->leaves[sel].submenu) {
          *current_menu = (*current_menu)->leaves[sel].submenu;
          return NEW_OVERLAY;
        }
        break;
      default:
        break;
    }
  }

  if ((*current_menu)->leaves[sel].leavetype == ICONFIG) {
    switch (command) {
      case CMD_MENU_RIGHT:
        if (cfg_480i_sl_are_linked((*current_menu)) && (sel > 3))
          cfg_inc_value(cfg_240p_opt_subscreen.leaves[sel].config_value);
        else
          cfg_inc_value((*current_menu)->leaves[sel].config_value);
        if (is_deblur_screen((*current_menu)) && (sel == 1 || sel ==2)) cfg_check_deblur_frame_cnt(sel == 1);
        return NEW_CONF_VALUE;
      case CMD_MENU_LEFT:
        if (cfg_480i_sl_are_linked((*current_menu)) && (sel > 3))
          cfg_dec_value(cfg_240p_opt_subscreen.leaves[sel].config_value);
        else
          cfg_dec_value((*current_menu)->leaves[sel].config_value);
        if (is_deblur_screen((*current_menu)) && (sel == 1 || sel ==2)) cfg_check_deblur_frame_cnt(sel == 1);
        return NEW_CONF_VALUE;
      default:
        break;
    }
  }

  if ((*current_menu)->leaves[sel].leavetype == IFUNC) {
    if ((command == CMD_MENU_RIGHT) || (command == CMD_MENU_ENTER)) {
      int retval;

      if (is_misc_screen((*current_menu))) {
        retval = (*current_menu)->leaves[sel].test_fun(sysconfig);
        return NON;
      }

      if (is_cfg_vpll_screen((*current_menu))) {
        retval = (*current_menu)->leaves[sel].test_fun(sysconfig);
        return (retval == 0 ? RW_DONE : RW_FAILED);
      }

      retval = (*current_menu)->leaves[sel].load_fun(sysconfig,1);
      return (retval == 0                     ? RW_DONE  :
              retval == -CFG_FLASH_SAVE_ABORT ? RW_ABORT :
                                                RW_FAILED);
    }
  }

  return NON;
}

inline void print_fw_version()
{
  alt_u16 hdl_fw = get_hdl_version();
  sprintf(szText,"%1d.%02d",((hdl_fw & HDL_FW_GETMAIN_MASK) >> HDL_FW_MAIN_OFFSET),
                         ((hdl_fw & HDL_FW_GETSUB_MASK) >> HDL_FW_SUB_OFFSET));
  vd_print_string(VERSION_H_OFFSET,VERSION_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,&szText[0]);

  sprintf(szText,"%1d.%02d",SW_FW_MAIN,SW_FW_SUB);
  vd_print_string(VERSION_H_OFFSET,VERSION_V_OFFSET+1,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,&szText[0]);
}

void print_overlay(menu_t* current_menu)
{
  alt_u8 h_run;
  alt_u8 overlay_h_offset =       (current_menu->type == TEXT) ? TEXTOVERLAY_H_OFFSET :
                            (is_cfg_vpll_screen(current_menu)) ? TEXTOVERLAY_H_OFFSET : HOMEOVERLAY_H_OFFSET;
  alt_u8 overlay_v_offset = 0;

  VD_CLEAR_SCREEN;

  if (current_menu->header) {
    overlay_v_offset = OVERLAY_V_OFFSET_WH;
    vd_print_string(VD_WIDTH-strlen(*current_menu->header),HEADER_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_DARKORANGE,*current_menu->header);
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
      break;
    default:
      break;
  }
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
  str_select = ((*ppu_state & PPU_STATE_15BIT_MODE_GETMASK) >> PPU_STATE_15BIT_MODE_OFFSET);
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
    str_select = 2;
    vd_print_string(INFO_VALS_H_OFFSET,INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY,DeBlur[str_select]);
  } else {
    str_select = ((*ppu_state & PPU_STATE_DODEBLUR_GETMASK) >> PPU_STATE_DODEBLUR_OFFSET);
    vd_print_string(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,OffOn[str_select]);
    str_select = (*ppu_state & PPU_STATE_DEBLURFORCE_GETMASK) >> PPU_STATE_DEBLURFORCE_OFFSET;
    vd_print_string(INFO_VALS_H_OFFSET + 4,INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,DeBlur[str_select]);
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


int update_cfg_screen(menu_t* current_menu)
{
  if (current_menu->type != CONFIG) return -1;

  alt_u8 h_l_offset;
  alt_u8 v_run, v_offset;
  alt_u8 background_color, font_color;
  alt_u8 val_select, ref_val_select, val_is_ref;
  alt_u8 use_240p_linked_values;

  background_color = BACKGROUNDCOLOR_STANDARD;

  for (v_run = 0; v_run < current_menu->number_selections; v_run++) {
    h_l_offset = current_menu->leaves[v_run].arrow_desc->hpos + 3;
    v_offset   = current_menu->leaves[v_run].id;

    switch (current_menu->leaves[v_run].leavetype) {
      case ISUBMENU:
        font_color = FONTCOLOR_WHITE;
        vd_print_string(h_l_offset,v_offset,background_color,font_color,EnterSubMenu);
        break;
      case IFUNC:
        if (is_cfg_vpll_screen(current_menu)) {
          font_color = vpll_lock ? FONTCOLOR_GREY : FONTCOLOR_WHITE;
          vd_print_string(h_l_offset,v_offset,background_color,font_color,StartTest);
        }
        break;
      case ICONFIG:
        use_240p_linked_values = cfg_480i_sl_are_linked(current_menu) && (v_run > 3);
        if (use_240p_linked_values) {
          val_select     = cfg_get_value(cfg_240p_opt_subscreen.leaves[v_run].config_value,0);
          ref_val_select = cfg_get_value(cfg_240p_opt_subscreen.leaves[v_run].config_value,use_flash);
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
        val_is_ref = (val_select == ref_val_select);
        font_color = val_is_ref ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;

        if (is_cfg_240p_screen(current_menu))
          if ((!cfg_get_value(current_menu->leaves[0].config_value,0) && v_run > 0) ||
              (!cfg_get_value(current_menu->leaves[2].config_value,0) && v_run > 2))
            font_color = val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;

        if (is_cfg_vpll_screen(current_menu)) {
          font_color =  vpll_lock ?     font_color :
                       val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;
        }

        if (is_cfg_480i_screen(current_menu))
          if ((!cfg_get_value(current_menu->leaves[0].config_value,0) && v_run > 0) ||
              (!cfg_get_value(current_menu->leaves[2].config_value,0) && v_run > 2))
            font_color = val_is_ref ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;

        if (v_run == current_menu->current_selection)
          vd_clear_area(h_l_offset,h_l_offset + OPT_WINDOW_WIDTH,v_offset,v_offset);

        if (is_misc_screen(current_menu) && v_run == 2 && !use_filteraddon)
          vd_print_string(h_l_offset-2,v_offset - 1,background_color,FONTCOLOR_GREY,FilterAddOn[5]);
        else if (use_240p_linked_values)
          if (cfg_240p_opt_subscreen.leaves[v_run].config_value->cfg_type == NUMVALUE) {
            cfg_240p_opt_subscreen.leaves[v_run].config_value->val2char_func(val_select);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
          } else {
            vd_print_string(h_l_offset,v_offset,background_color,font_color,cfg_240p_opt_subscreen.leaves[v_run].config_value->value_string[val_select]);
          }
        else
          if (current_menu->leaves[v_run].config_value->cfg_type == NUMVALUE) {
            current_menu->leaves[v_run].config_value->val2char_func(val_select);
            vd_print_string(h_l_offset,v_offset,background_color,font_color,&szText[0]);
          } else {
            vd_print_string(h_l_offset,v_offset,background_color,font_color,current_menu->leaves[v_run].config_value->value_string[val_select]);
          }
        break;
      default:
        break;
    }
  }

  if (is_cfg_240p_screen(current_menu)) {
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
