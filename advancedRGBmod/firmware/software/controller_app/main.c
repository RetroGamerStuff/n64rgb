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
#include "altera_epcq_controller2.h"

#include "cfg_header/cfg_internal_p.h"
#include "cfg_header/cfg_io_p.h"
#include "n64.h"
#include "config.h"
#include "menu.h"
#include "vd_driver.h"
#include "flash.h"


#define DEBLUR_FORCE_OFF 1
#define DEBLUR_FORCE_ON  2

#define RWM_H_OFFSET 5
#define RWM_V_OFFSET (VD_HEIGHT - 3)
#define RWM_LENGTH   10
#define RWM_SHOW_CNT 512

const alt_u8 RW_Message_FontColor[] = {FONTCOLOR_GREEN,FONTCOLOR_RED};
const char   *RW_Message[] = {"< Success >","< Fail >"};


/* ToDo: break up with configuration word general - e.g. externalize non-flaged values for gamma and sl_str */

int main()
{
  cmd_t command;
  updateaction_t todo;
  menu_t *menu = &home_menu;

  alt_epcq_controller2_dev *epcq_controller2_dev;
  set_flash(&epcq_controller2_dev);

  char szText[VD_WIDTH];

  configuration_t sysconfig = {
      .ext_cfg = &cfg_data_general,
      .int_cfg = &cfg_data_internal
  };

  sysconfig.ext_cfg->cfg_word_val = 0;
  sysconfig.int_cfg->cfg_word_val = 0;

  alt_u32 ctrl_data;
  alt_u8  info_data;

  static alt_u8  info_data_pre = 0;
  static alt_u16 cfg_io_word_pre = 0;

  static int message_cnt = 0;

  info_data = get_info_data();

  /* ToDo: replace by load from flash */
  if (info_data & INFO_FALLBACKMODE_GETMASK)
    cfg_load_n64defaults(&sysconfig);
  else
    cfg_load_jumperset(&sysconfig);

  cfg_apply_word(sysconfig.ext_cfg);
  cfg_io_word_pre = sysconfig.ext_cfg->cfg_word_val;

  cfg_load_sysdefaults(&sysconfig);

  /* Event loop never exits. */
  while (1) {
    ctrl_data = get_ctrl_data();
    info_data = get_info_data();

    command = ctrl_data_to_cmd(&ctrl_data);

    if(cfg_get_value(&show_osd)) {

      if (message_cnt > 0) {
        if (message_cnt == 1) vd_clear_area(RWM_H_OFFSET,RWM_H_OFFSET+RWM_LENGTH,RWM_V_OFFSET,RWM_V_OFFSET);
        message_cnt--;
      }

      todo = apply_command(command,&menu,&sysconfig);

      switch (todo) {
        case MENU_CLOSE:
          cfg_clear_flag(&show_osd);
          cfg_apply_word(sysconfig.ext_cfg);
          break;
        case NEW_OVERLAY:
          print_overlay(menu);
          print_selection_arrow(menu);
          message_cnt = 0;
          break;
        case NEW_SELECTION:
          print_selection_arrow(menu);
          break;
        case RW_DONE:
          cfg_apply_word(sysconfig.ext_cfg);
          vd_print_string(RWM_H_OFFSET,RWM_V_OFFSET,BACKGROUNDCOLOR_STANDARD,RW_Message_FontColor[0],RW_Message[0]);
          message_cnt = RWM_SHOW_CNT;
          break;
        case RW_FAILED:
          cfg_apply_word(sysconfig.ext_cfg);
          vd_print_string(RWM_H_OFFSET,RWM_V_OFFSET,BACKGROUNDCOLOR_STANDARD,RW_Message_FontColor[1],RW_Message[1]);
          message_cnt = RWM_SHOW_CNT;
          break;
        default:
          break;
      }

      if ((menu->type == VINFO) &&
          ((info_data_pre != info_data)              ||
           (cfg_io_word_pre != sysconfig.ext_cfg->cfg_word_val) ||
           (todo == NEW_OVERLAY)                     ))
        update_vinfo_screen(menu,&cfg_data_general,info_data);

      if ((menu->type == CONFIG) &&
          ((cfg_io_word_pre != sysconfig.ext_cfg->cfg_word_val) ||
           (todo == NEW_OVERLAY)                     ||
           (todo == NEW_CONF_VALUE)                  ||
           (todo == NEW_SELECTION)                   ))
        update_cfg_screen(menu,sysconfig.ext_cfg);

    } else { /* END OF if(cfg_get_value(&show_osd)) */

      if (command == CMD_OPEN_MENU) {
        cfg_set_flag(&show_osd);
        cfg_apply_word(sysconfig.ext_cfg);
        print_overlay(menu);
        print_selection_arrow(menu);
      }

      if ((cfg_get_value(&igr_quickchange) & CFGI_QUDEBLUR_GETMASK))
        switch (command) {
          case CMD_DEBLUR_QUICK_ON:
            if (!(info_data & INFO_480I_GETMASK)) {
              cfg_set_value(&deblur,DEBLUR_FORCE_ON);
              cfg_apply_value(&deblur);
            };
            break;
          case CMD_DEBLUR_QUICK_OFF:
            if (!(info_data & INFO_480I_GETMASK)) {
              cfg_set_value(&deblur,DEBLUR_FORCE_OFF);
              cfg_apply_value(&deblur);
            };
            break;
          default:
            break;
        }

      if ((cfg_get_value(&igr_quickchange) & CFGI_QU15BITMODE_GETMASK))
          switch (command) {
            case CMD_15BIT_QUICK_ON:
              cfg_set_flag(&mode15bit);
              cfg_apply_value(&mode15bit);
              break;
            case CMD_15BIT_QUICK_OFF:
              cfg_clear_flag(&mode15bit);
              cfg_apply_value(&mode15bit);
              break;
            default:
              break;
          }

    } /* END OF if(!cfg_get_value(&show_osd)) */


    if (menu->type != TEXT) {
      sprintf(szText,"Ctrl.Data: 0x%08x",(uint) ctrl_data);
      vd_print_string(0, VD_HEIGHT-1, BACKGROUNDCOLOR_STANDARD, FONTCOLOR_NAVAJOWHITE, &szText[0]);
    }


    info_data_pre = info_data;
    cfg_io_word_pre = sysconfig.ext_cfg->cfg_word_val;

    /* ToDo: use external interrupt to go on on nVSYNC */
    while(!get_nvsync())                         {};  /* wait for nVSYNC goes high */
    while( get_nvsync() && new_ctrl_available()) {};  /* wait for nVSYNC goes low and
                                                         wait for new controller available */
  }

  return 0;
}
