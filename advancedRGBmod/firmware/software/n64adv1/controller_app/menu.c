/*********************************************************************************
 *
 * This file is part of the N64 RGB/YPbPr DAC project.
 *
 * Copyright (C) 2016-2018 by Peter Bartmann <borti4938@gmx.de>
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

static const arrow_t selection_arrow = {
    .arrowshape_left  = SUBMENU_ARROW_L,
    .arrowshape_right = SUBMENU_ARROW_L,
    .larrow_hpos = 1,
    .rarrow_hpos = 1
};

static const arrow_t cfg_opt_arrow = {
    .arrowshape_left  = OPT_ARROW_L,
    .arrowshape_right = OPT_ARROW_R,
    .larrow_hpos = (CFG_VALS_H_OFFSET - 2),
    .rarrow_hpos = (CFG_VALS_H_OFFSET + OPT_WINDOW_WIDTH - 2)
};

static const arrow_t cfg_sel_arrow = {
    .arrowshape_left  = SUBMENU_ARROW_L,
    .arrowshape_right = SUBMENU_ARROW_L,
    .larrow_hpos = (CFG_VALS_H_OFFSET - 2),
    .rarrow_hpos = (CFG_VALS_H_OFFSET - 2)
};

static const arrow_t cfg_240p_opt_arrow = {
    .arrowshape_left  = OPT_ARROW_L,
    .arrowshape_right = OPT_ARROW_R,
    .larrow_hpos = (CFG_VSUB_VALS_H_OFFSET - 2),
    .rarrow_hpos = (CFG_VSUB_VALS_H_OFFSET + OPT_WINDOW_WIDTH - 3)
};

static const arrow_t misc_screen_arrow = {
    .arrowshape_left  = OPT_ARROW_L,
    .arrowshape_right = OPT_ARROW_R,
    .larrow_hpos = (MISC_VALS_H_OFFSET - 2),
    .rarrow_hpos = (MISC_VALS_H_OFFSET + OPT_WINDOW_WIDTH - 2)
};

menu_t home_menu, vinfo_screen, cfg_screen, cfg_240p_opt_subscreen, cfg_480i_opt_subscreen, misc_screen,
       rwdata_screen, about_screen, thanks_screen, license_screen;

extern config_t vformat, deblur, mode15bit, gamma_lut;
extern config_t linex2, sl_en, sl_method, sl_id, sl_str, slhyb_str, show_sl_in_osd;
extern config_t linex2_480i, sl_en_480i, sl_link_480i, sl_id_480i, sl_str_480i, slhyb_str_480i;
extern config_t igr_reset, igr_quickchange, filteraddon_cutoff;

menu_t home_menu = {
    .type = HOME,
    .header  = &home_header,
    .overlay = &home_overlay,
    .current_selection = 1,
    .number_selections = 7,
    .leaves = {
        {.id = MAIN2VINFO_V_OFFSET  , .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &vinfo_screen},
        {.id = MAIN2CFG_V_OFFSET    , .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &cfg_screen},
        {.id = MAIN2MISC_V_OFFSET   , .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &misc_screen},
        {.id = MAIN2SAVE_V_OFFSET   , .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &rwdata_screen},
        {.id = MAIN2ABOUT_V_OFFSET  , .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &about_screen},
        {.id = MAIN2THANKS_V_OFFSET , .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &thanks_screen},
        {.id = MAIN2LICENSE_V_OFFSET, .arrow_desc = &selection_arrow, .leavetype = ISUBMENU, .submenu = &license_screen}
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
    .number_selections = 6,
    .leaves = {
        {.id = CFG_240P_SET_V_OFFSET, .arrow_desc = &cfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &cfg_240p_opt_subscreen},
        {.id = CFG_480I_SET_V_OFFSET, .arrow_desc = &cfg_sel_arrow, .leavetype = ISUBMENU, .submenu      = &cfg_480i_opt_subscreen},
        {.id = CFG_FORMAT_V_OFFSET  , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &vformat},
        {.id = CFG_DEBLUR_V_OFFSET  , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &deblur},
        {.id = CFG_15BIT_V_OFFSET   , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &mode15bit},
        {.id = CFG_GAMMA_V_OFFSET   , .arrow_desc = &cfg_opt_arrow, .leavetype = ICONFIG , .config_value = &gamma_lut}
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
        {.id = CFG_VSUB_LINEX2_V_OFFSET   , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &linex2},
        {.id = CFG_VSUB_SL_EN_V_OFFSET    , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_en},
        {.id = CFG_VSUB_SL_METHOD_V_OFFSET, .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_method},
        {.id = CFG_VSUB_SL_ID_V_OFFSET    , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_id},
        {.id = CFG_VSUB_SL_STR_V_OFFSET   , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_str},
        {.id = CFG_VSUB_SLHYB_STR_V_OFFSET, .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &slhyb_str},
        {.id = CFG_VSUB_SLOSD_V_OFFSET    , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &show_sl_in_osd}
    }
};


menu_t cfg_480i_opt_subscreen = {
    .type = CONFIG,
    .header = &cfg_480i_opt_header,
    .overlay = &cfg_240p_opt_overlay,
    .parent = &cfg_screen,
    .current_selection = 0,
    .number_selections = 7,
    .leaves = {
        {.id = CFG_VSUB_LINEX2_V_OFFSET   , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &linex2_480i},
        {.id = CFG_VSUB_SL_EN_V_OFFSET    , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_en_480i},
        {.id = CFG_VSUB_SL_METHOD_V_OFFSET, .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_link_480i},
        {.id = CFG_VSUB_SL_ID_V_OFFSET    , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_id_480i},
        {.id = CFG_VSUB_SL_STR_V_OFFSET   , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &sl_str_480i},
        {.id = CFG_VSUB_SLHYB_STR_V_OFFSET, .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &slhyb_str_480i},
        {.id = CFG_VSUB_SLOSD_V_OFFSET    , .arrow_desc = &cfg_240p_opt_arrow, .leavetype = ICONFIG , .config_value = &show_sl_in_osd}
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
        {.id = MISC_SHOWTESTPAT_V_OFFSET, .arrow_desc = &selection_arrow,   .leavetype = IFUNC, .test_fun = &cfg_show_testpattern}
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
        {.id = RWDATA_SAVE_FL_V_OFFSET , .arrow_desc = &selection_arrow, .leavetype = IFUNC, .save_fun = &cfg_save_to_flash},
        {.id = RWDATA_LOAD_FL_V_OFFSET , .arrow_desc = &selection_arrow, .leavetype = IFUNC, .load_fun = &cfg_load_from_flash},
        {.id = RWDATA_LOAD_JS_V_OFFSET , .arrow_desc = &selection_arrow, .leavetype = IFUNC, .load_fun = &cfg_load_jumperset},
        {.id = RWDATA_LOAD_N64_V_OFFSET, .arrow_desc = &selection_arrow, .leavetype = IFUNC, .load_fun = &cfg_load_n64defaults}
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


inline alt_u8 is_cfg_240p_screen (menu_t *menu)  /* ugly hack (ToDo on updates: check for validity, i.e. is this property still unique
                                                               (this one shall be valid for 240p and 480i sub screen!!!)) */
  {  return (menu->leaves[0].config_value->flag_masks.clrflag_mask == CFG_LINEX2_CLRMASK); }
inline alt_u8 cfg_480i_sl_are_linked (menu_t *menu)  /* ugly hack (ToDo on updates: check for validity, i.e. is this property still unique) */
  {  return ( is_cfg_240p_screen (menu) && menu->leaves[0].config_value == &linex2_480i && cfg_get_value(menu->leaves[2].config_value,0)); }
inline alt_u8 is_misc_screen (menu_t *menu) /* ugly hack (ToDo on updates: check for validity, i.e. is this property still unique) */
  {  return (menu->leaves[0].config_value->flag_masks.clrflag_mask == CFG_USEIGR_CLRMASK); }
inline alt_u8 is_sl_str_val (config_t *config_value) /* ugly hack (ToDo on updates: check for validity, i.e. is this property still unique) */
  {  return (config_value->value_details.max_value == CFG_SLSTR_MAX_VALUE); }


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

  alt_u8 sel = (*current_menu)->current_selection;

  if (todo == NEW_SELECTION) {
    if (is_cfg_240p_screen(*current_menu)) {
      if (!cfg_get_value((*current_menu)->leaves[0].config_value,0))
        (*current_menu)->current_selection = 0;
      else if (!cfg_get_value((*current_menu)->leaves[1].config_value,0))
        (*current_menu)->current_selection = (*current_menu)->current_selection < 2 ? (*current_menu)->current_selection :
            (command == CMD_MENU_UP) ? 1 : 0;
    }

    if (is_misc_screen(*current_menu) && sel == 2 && !use_filteraddon)
      (*current_menu)->current_selection = (command == CMD_MENU_UP) ? 1 : 0;

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
	if (cfg_480i_sl_are_linked((*current_menu)) && (sel > 2))
	  cfg_inc_value(cfg_240p_opt_subscreen.leaves[sel].config_value);
	else
          cfg_inc_value((*current_menu)->leaves[sel].config_value);
        return NEW_CONF_VALUE;
      case CMD_MENU_LEFT:
	if (cfg_480i_sl_are_linked((*current_menu)) && (sel > 2))
	  cfg_dec_value(cfg_240p_opt_subscreen.leaves[sel].config_value);
	else
          cfg_dec_value((*current_menu)->leaves[sel].config_value);
        return NEW_CONF_VALUE;
      default:
        break;
    }
  }

  if ((*current_menu)->leaves[sel].leavetype == IFUNC) {
    if ((command == CMD_MENU_RIGHT) || (command == CMD_MENU_ENTER)) {
      if (is_misc_screen((*current_menu))) {
        (*current_menu)->leaves[sel].test_fun(sysconfig);
        return 0;
      } else {
        int retval = (*current_menu)->leaves[sel].load_fun(sysconfig,1);
        return (retval == 0                     ? RW_DONE  :
                retval == -CFG_FLASH_SAVE_ABORT ? RW_ABORT :
                                                  RW_FAILED);
      }
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
  alt_u8 overlay_h_offset = (current_menu->type == TEXT) ? TEXTOVERLAY_H_OFFSET : HOMEOVERLAY_H_OFFSET;
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
      h_l_offset = current_menu->leaves[v_run].arrow_desc->larrow_hpos;
      h_r_offset = current_menu->leaves[v_run].arrow_desc->rarrow_hpos;
      v_offset   = current_menu->leaves[v_run].id;
      if (v_run == current_menu->current_selection) {
        vd_print_char(h_l_offset,v_offset,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,(char) current_menu->leaves[v_run].arrow_desc->arrowshape_left);
        vd_print_char(h_r_offset,v_offset,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,(char) current_menu->leaves[v_run].arrow_desc->arrowshape_right);
      } else {
        vd_clear_char(h_l_offset,v_offset);
        vd_clear_char(h_r_offset,v_offset);
      }
    }
}

int update_vinfo_screen(menu_t* current_menu, configuration_t* sysconfig, alt_u8 info_data)
{
  if (current_menu->type != VINFO) return -1;

  alt_u8 str_select;
  static alt_u8 video_sd_ed;

  // Video Input
  str_select = ((info_data & (INFO_480I_GETMASK | INFO_VMODE_GETMASK)) >> INFO_480I_OFFSET);
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VIN_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VIN_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoMode[str_select]);

  // Video Output
  switch(((((sysconfig->cfg_word_def[IMAGE_240P]->cfg_word_val & CFG_LINEX2_GETMASK) >> CFG_LINEX2_OFFSET) << 3) |
          (((sysconfig->cfg_word_def[IMAGE_480I]->cfg_word_val & CFG_LINEX2_GETMASK) >> CFG_LINEX2_OFFSET) << 2) | str_select) & 0xF) {
   /* order: lineX2_240p, linex2_480i, pal, 480i */
    case 0xE: /* 1110 */
    case 0xA: /* 1010 */
    case 0xF: /* 1111 */
    case 0x7: /* 0111 */
      str_select  = 5;
      video_sd_ed = 1;
      break;
    case 0xC: /* 1100 */
    case 0x8: /* 1000 */
    case 0xD: /* 1101 */
    case 0x5: /* 0101 */
      str_select  = 4;
      video_sd_ed = 1;
      break;
    default :
      video_sd_ed = 0;
      break;
  }
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_VOUT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoMode[str_select]);

  // Color Depth
  str_select = (sysconfig->cfg_word_def[VIDEO]->cfg_word_val & CFG_15BITMODE_GETMASK) >> CFG_15BITMODE_OFFSET;
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_COL_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_COL_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoColor[str_select]);

  // Video Format
  if (sysconfig->cfg_word_def[VIDEO]->cfg_word_val & CFG_YPBPR_GETMASK)
    str_select = 2;
  else
    str_select = (sysconfig->cfg_word_def[VIDEO]->cfg_word_val & CFG_RGSB_GETMASK) >> CFG_RGSB_OFFSET;
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_FORMAT_V_OFFSET);
  vd_print_string(INFO_VALS_H_OFFSET,INFO_FORMAT_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,VideoFormat[str_select]);

  // 240p DeBlur
  vd_clear_lineend(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET);
  if (info_data & INFO_480I_GETMASK) {
    str_select = 2;
    vd_print_string(INFO_VALS_H_OFFSET,INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY,DeBlur[str_select]);
  } else {
    str_select = (info_data & INFO_DODEBLUR_GETMASK) >> INFO_DODEBLUR_OFFSET;
    vd_print_string(INFO_VALS_H_OFFSET, INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,OffOn[str_select]);
    str_select = (((sysconfig->cfg_word_def[VIDEO]->cfg_word_val & CFG_DEBLUR_GETMASK) >> CFG_DEBLUR_OFFSET) > 0);
    vd_print_string(INFO_VALS_H_OFFSET + 4,INFO_DEBLUR_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE,DeBlur[str_select]);
  }

  // Filter Add-on
  vd_clear_lineend(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET);
  if (!use_filteraddon)
    vd_print_string(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREY, FilterAddOn[4]);
  else {
    str_select = ((sysconfig->cfg_word_def[VIDEO]->cfg_word_val & CFG_FILTERADDON_GETMASK) >> CFG_FILTERADDON_OFFSET);
    if (str_select == 0) str_select = video_sd_ed + 1;
    vd_print_string(INFO_VALS_H_OFFSET,INFO_FAO_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_WHITE, FilterAddOn[str_select]);
  }

  return 0;
}


int update_cfg_screen(menu_t* current_menu)
{
  if (current_menu->type != CONFIG) return -1;

  alt_u8 h_l_offset, h_r_offset;
  alt_u8 v_run, v_offset;
  alt_u8 background_color, font_color;
  alt_u8 val_select, ref_val_select;

  for (v_run = 0; v_run < current_menu->number_selections; v_run++) {
    h_l_offset = current_menu->leaves[v_run].arrow_desc->larrow_hpos + 2;
    h_r_offset = current_menu->leaves[v_run].arrow_desc->rarrow_hpos - 2;
    v_offset   = current_menu->leaves[v_run].id;

    if (current_menu->leaves[v_run].leavetype == ISUBMENU) {
      font_color = FONTCOLOR_WHITE;
      vd_print_string(h_l_offset,v_offset,background_color,font_color,EnterSubMenu);
    }
    else if (current_menu->leaves[v_run].leavetype == ICONFIG) {
      alt_u8 use_240p_linked_values = cfg_480i_sl_are_linked(current_menu) && (v_run > 2);
      if (use_240p_linked_values) {
        val_select     = cfg_get_value(cfg_240p_opt_subscreen.leaves[v_run].config_value,0);
        ref_val_select = cfg_get_value(cfg_240p_opt_subscreen.leaves[v_run].config_value,use_flash);
      } else {
        val_select     = cfg_get_value(current_menu->leaves[v_run].config_value,0);
        ref_val_select = cfg_get_value(current_menu->leaves[v_run].config_value,use_flash);
      }

//      if (current_menu->current_selection == v_run) {
//        background_color = OPT_WINDOWCOLOR_BG;
//        font_color = OPT_WINDOWCOLOR_FONT;
//      } else {
        background_color = BACKGROUNDCOLOR_STANDARD;
        font_color = (val_select == ref_val_select) ? FONTCOLOR_WHITE : FONTCOLOR_YELLOW;
//      }

      if (is_cfg_240p_screen(current_menu))
	if ((!cfg_get_value(current_menu->leaves[0].config_value,0) && v_run > 0) ||
	    (!cfg_get_value(current_menu->leaves[1].config_value,0) && v_run > 1))
          font_color = (val_select == ref_val_select) ? FONTCOLOR_GREY : FONTCOLOR_DARKGOLD;

      if (v_run == current_menu->current_selection)
        vd_clear_area(h_l_offset,h_r_offset,v_offset,v_offset);

      if (is_sl_str_val(current_menu->leaves[v_run].config_value)) val_select++;

      if (is_misc_screen(current_menu) && v_run == 2 && !use_filteraddon)
        vd_print_string(h_l_offset-2,v_offset - 1,background_color,FONTCOLOR_GREY,FilterAddOn[4]);
      else if (use_240p_linked_values)
	vd_print_string(h_l_offset,v_offset,background_color,font_color,cfg_240p_opt_subscreen.leaves[v_run].config_value->value_string[val_select]);
      else
        vd_print_string(h_l_offset,v_offset,background_color,font_color,current_menu->leaves[v_run].config_value->value_string[val_select]);
    }
  }

  return 0;
}
