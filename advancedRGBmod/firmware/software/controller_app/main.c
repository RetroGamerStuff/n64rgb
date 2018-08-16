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
 * main.c
 *
 *  Created on: 08.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "system.h"

#include "cfg_header/cfg_io_p.h"
#include "n64.h"
#include "config.h"
#include "menu.h"
#include "vd_driver.h"
#include "flash.h"


#define DEBLUR_FORCE_OFF 1
#define DEBLUR_FORCE_ON  2

const alt_u8 RW_Message_FontColor[] = {FONTCOLOR_GREEN,FONTCOLOR_RED,FONTCOLOR_MAGENTA};
const char   *RW_Message[] = {"< Success >","< Failed >","< Aborted >"};


/* ToDo's:
 * - Display warning messages
 * - Additional windows (Ctrl. input, Video Output as OSD without menu)
 */

int main()
{
  cmd_t command;
  updateaction_t todo;
  menu_t *menu = &home_menu;

  configuration_t sysconfig = {
      .cfg_word_def[VIDEO]  = &cfg_data_video,
      .cfg_word_def[IMAGE1] = &cfg_data_image1,
      .cfg_word_def[IMAGE2] = &cfg_data_image2,
      .cfg_word_def[MISC]   = &cfg_data_misc,
      .cfg_word_def[MENU]   = &cfg_data_menu
  };

  cfg_clear_words(&sysconfig);
  check_flash();

  alt_u32 ctrl_data;
  alt_u8  info_data;

  static alt_u8 ctrl_update = 1;
  static alt_u8 info_data_pre = 0;

  static int message_cnt = 0;

  info_data = get_info_data();
  check_filteraddon(info_data);

  int load_from_jumperset = 1;

  if (use_flash) {
    load_from_jumperset = cfg_load_from_flash(&sysconfig,0);
    cfg_clear_flag(&show_osd);
  }

  if (info_data & INFO_FALLBACKMODE_GETMASK)
    cfg_load_n64defaults(&sysconfig,0);
  else if (load_from_jumperset != 0) {
    cfg_clear_words(&sysconfig);  // just in case anything went wrong while loading from flash
    cfg_load_jumperset(&sysconfig,0);
//    cfg_save_to_flash(&sysconfig,0);
  }

  cfgopt_apply_to_logic(&sysconfig);

  /* Event loop never exits. */
  while (1) {
    if (ctrl_update) {
      ctrl_data = get_ctrl_data();
      command = ctrl_data_to_cmd(&ctrl_data,0);
    } else {
      command = CMD_NON;
    }

    info_data = get_info_data();


    if(cfg_get_value(&show_osd,0)) {

      if (message_cnt > 0) {
        if (command != CMD_NON) {
          command = CMD_NON;
          message_cnt = 1;
        }
        if (message_cnt == 1) vd_clear_area(RWM_H_OFFSET,RWM_H_OFFSET+RWM_LENGTH,RWM_V_OFFSET,RWM_V_OFFSET);
        message_cnt--;
      }

      todo = modify_menu(command,&menu,&sysconfig);

      switch (todo) {
        case MENU_MUTE:
          cfg_set_flag(&mute_osd_tmp);
          break;
        case MENU_UNMUTE:
          cfg_clear_flag(&mute_osd_tmp);
          break;
        case MENU_CLOSE:
          cfg_clear_flag(&show_osd);
          break;
        case NEW_OVERLAY:
          print_overlay(menu);
          if (menu->header) cfg_set_flag(&show_logo);
          else              cfg_clear_flag(&show_logo);
          print_selection_arrow(menu);
          message_cnt = 0;
          break;
        case NEW_SELECTION:
          print_selection_arrow(menu);
          break;
        case RW_DONE:
        case RW_FAILED:
        case RW_ABORT:
          vd_print_string(RWM_H_OFFSET,RWM_V_OFFSET,BACKGROUNDCOLOR_STANDARD,RW_Message_FontColor[todo-RW_DONE],RW_Message[todo-RW_DONE]);
          message_cnt = RWM_SHOW_CNT;
          break;
        default:
          break;
      }

      if ((menu->type == VINFO) &&
          ((info_data_pre != info_data)              ||
           (todo == NEW_OVERLAY)                     ))
        update_vinfo_screen(menu,&sysconfig,info_data);

      if ((menu->type == CONFIG) && ((todo == NEW_OVERLAY)    ||
                                     (todo == NEW_CONF_VALUE) ||
                                     (todo == NEW_SELECTION)  ))
        update_cfg_screen(menu);

    } else { /* END OF if(cfg_get_value(&show_osd)) */

      if (command == CMD_OPEN_MENU) {
        print_overlay(menu);
        cfg_set_flag(&show_logo);
        print_selection_arrow(menu);
        cfg_set_flag(&show_osd);
        cfg_clear_flag(&mute_osd_tmp);
      }

      if ((cfg_get_value(&igr_quickchange,0) & CFG_QUDEBLUR_GETMASK))
        switch (command) {
          case CMD_DEBLUR_QUICK_ON:
            if (!(info_data & INFO_480I_GETMASK)) {
              cfg_set_value(&deblur,DEBLUR_FORCE_ON);
            };
            break;
          case CMD_DEBLUR_QUICK_OFF:
            if (!(info_data & INFO_480I_GETMASK)) {
              cfg_set_value(&deblur,DEBLUR_FORCE_OFF);
            };
            break;
          default:
            break;
        }

      if ((cfg_get_value(&igr_quickchange,0) & CFG_QU15BITMODE_GETMASK))
          switch (command) {
            case CMD_15BIT_QUICK_ON:
              cfg_set_flag(&mode15bit);
              break;
            case CMD_15BIT_QUICK_OFF:
              cfg_clear_flag(&mode15bit);
              break;
            default:
              break;
          }

    } /* END OF if(!cfg_get_value(&show_osd)) */


    if (menu->type != TEXT) print_ctrl_data(&ctrl_data);

    info_data_pre = info_data;

    cfgopt_apply_to_logic(&sysconfig);
    cfgmenu_apply_to_logic(&sysconfig);

    /* ToDo: use external interrupt to go on on nVSYNC */
    while(!get_nvsync()){};  /* wait for nVSYNC goes high */
    while( get_nvsync()){};  /* wait for nVSYNC goes low  */
    ctrl_update = new_ctrl_available();
  }

  return 0;
}
