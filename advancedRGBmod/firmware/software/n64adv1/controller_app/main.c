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
 * main.c
 *
 *  Created on: 08.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "alt_types.h"
#include "system.h"

#include "cfg_io_p.h"
#include "n64.h"
#include "config.h"
#include "menu.h"
#include "vd_driver.h"
#include "flash.h"


#define DEBLUR_FORCE_OFF 1
#define DEBLUR_FORCE_ON  2

const alt_u8 RW_Message_FontColor[] = {FONTCOLOR_GREEN,FONTCOLOR_RED,FONTCOLOR_MAGENTA};
const char   *RW_Message[] = {"< Success >","< Failed > ","< Aborted >"};

extern const char *NTSCPAL_SEL[];
config_t ntsc_pal_selection = {
    // .cfg_b32word_t* must be NULL to show that this is a local value
    .cfg_type     = TXTVALUE, // treat as txtvalue for modifying function
    .value_details = {
      .max_value = 1,
    },
    .value_string = &NTSCPAL_SEL
};


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
      .cfg_word_def[MISC]  = &cfg_data_misc,
      .cfg_word_def[VIDEO] = &cfg_data_video,
      .cfg_word_def[IMAGE] = &cfg_data_image,
  };

  cfg_clear_words(&sysconfig);

  alt_u8 ctrl_update = 1;
  alt_u32 ctrl_data;
  alt_u16 ppu_state, ppu_state_pre;
  alt_u8 vpll_lock_first_boot;
  alt_u8 vpll_state_frame_delay;

  int message_cnt = 0;

  check_filteraddon();

  int load_from_jumperset = 1;
  check_flash();
  if (use_flash) {
    load_from_jumperset = cfg_load_from_flash(&sysconfig,0);
  }

  if (load_from_jumperset != 0) {
    cfg_clear_words(&sysconfig);  // just in case anything went wrong while loading from flash
    cfg_load_jumperset(&sysconfig,0);
//    cfg_save_to_flash(&sysconfig,0);
  }

  alt_u8 use_fallback = get_fallback_mode();
  while (is_fallback_mode_valid() == 0) use_fallback = get_fallback_mode();

  if (use_fallback) {
    cfg_clear_words(&sysconfig);  // just in case anything went wrong while loading from flash
    cfg_load_defaults(&sysconfig,0);
    print_overlay(menu);
    cfg_set_flag(&show_logo);
    print_selection_arrow(menu);
    cfg_set_flag(&show_osd);
  } else {
    cfg_clear_flag(&show_osd);
    cfg_clear_flag(&show_logo);
  }
  cfg_clear_flag(&mute_osd_tmp);
  cfg_clear_flag(&show_testpat);
  cfg_clear_flag(&test_vpll);

  cfg_load_image_word(&sysconfig,0);
  cfg_apply_to_logic(&sysconfig);

  vpll_lock_first_boot = 1;
  vpll_state_frame_delay = 0;

  ppu_state = get_ppu_state();
  ppu_state_pre = ~ppu_state;


  /* Event loop never exits. */
  while (1) {
    if (ctrl_update) {
      ctrl_data = get_ctrl_data();
      command = ctrl_data_to_cmd(&ctrl_data,0);
    } else {
      command = CMD_NON;
    }


    if(cfg_get_value(&show_osd,0)) {

      alt_u8 image_word_selection = cfg_get_value(&ntsc_pal_selection,0);
      if (cfg_get_value(&pal_awareness,0)) // show the correct options
        cfg_load_image_word(&sysconfig,image_word_selection);

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
          ((ppu_state_pre != ppu_state)              ||
           (todo == NEW_OVERLAY)                     ))
        update_vinfo_screen(menu,&ppu_state);

      if ((menu->type == CONFIG) && ((todo == NEW_OVERLAY)    ||
                                     (todo == NEW_CONF_VALUE) ||
                                     (todo == NEW_SELECTION)  ))
        update_cfg_screen(menu);

      cfg_store_image_word(&sysconfig,image_word_selection);
      if (!cfg_get_value(&pal_awareness,0))
        cfg_set_value(&ntsc_pal_selection,0);

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
            if (!(ppu_state & PPU_STATE_480I_GETMASK)) {
              cfg_set_value(&deblur_mode,DEBLUR_FORCE_ON);
            };
            break;
          case CMD_DEBLUR_QUICK_OFF:
            if (!(ppu_state & PPU_STATE_480I_GETMASK)) {
              cfg_set_value(&deblur_mode,DEBLUR_FORCE_OFF);
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

    ppu_state_pre = ppu_state;

    vpll_lock = update_vpll_lock_state();
    if (vpll_lock) {
      vpll_lock_first_boot = 0;
      vpll_state_frame_delay = 0;
      cfg_set_flag(&test_vpll);
    } else { // VPLL is not running at this point (lock lost?)
      if (vpll_lock_first_boot) {
        vpll_state_frame_delay++;
        if (vpll_state_frame_delay == VPLL_START_FRAMES) {
          vpll_lock_first_boot = 0;
          vpll_state_frame_delay = VPLL_LOCK_LOST_FRAMES;
        }
      } else {
        if (vpll_state_frame_delay == VPLL_LOCK_LOST_FRAMES) cfg_clear_flag(&use_vpll);
        else                                                 vpll_state_frame_delay++;
      }
    }

    if (cfg_get_value(&pal_awareness,0))
      cfg_load_image_word(&sysconfig,(ppu_state & PPU_STATE_PALMODE_GETMASK) >> PPU_STATE_PALMODE_OFFSET);
    else
      cfg_load_image_word(&sysconfig,0);

    cfg_apply_to_logic(&sysconfig);

    /* ToDo: use external interrupt to go on on nVSYNC */
    while(!get_nvsync()){};  /* wait for nVSYNC goes high */
    while( get_nvsync()){};  /* wait for nVSYNC goes low  */
    ctrl_update = new_ctrl_available();
    ppu_state = get_ppu_state();
  }

  return 0;
}
