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

#include "n64.h"
#include "config.h"
#include "menu.h"
#include "vd_driver.h"
#include "flash.h"

#define CTRL_IGNORE_FRAMES 10;


const alt_u8 RW_Message_FontColor[] = {FONTCOLOR_GREEN,FONTCOLOR_RED,FONTCOLOR_MAGENTA};
const char   *RW_Message[] = {"< Success >","< Failed > ","< Aborted >"};

alt_u8 boot_welcome = 0;
vmode_t vmode_menu, vmode_n64adv;
cfg_timing_model_sel_type_t timing_menu, timing_n64adv;

/* ToDo's:
 * - Display warning messages
 * - Additional windows (Ctrl. input, Video Output as OSD without menu)
 */

void open_osd_main(menu_t **menu)
{
  print_overlay(*menu);
  cfg_set_flag(&show_logo);
  print_selection_arrow(*menu);
  cfg_set_flag(&show_osd);
  cfg_clear_flag(&mute_osd_tmp);
}

int main()
{
  cmd_t command;
  updateaction_t todo;
  menu_t *menu = &home_menu;

  alt_u8 ctrl_update = 1;
  alt_u8 ctrl_ignore = 0;
  cfg_offon_t vpll_lock_first_boot;
  alt_u8 vpll_state_frame_delay;

  int message_cnt = 0;

  alt_u8 powercycle_show_menu = 0;
  int load_from_jumperset = check_flash();

  check_filteraddon();
  cfg_clear_words();

  if (use_flash) {
    load_from_jumperset = cfg_load_from_flash(0);
    if (boot_welcome == 1 || load_from_jumperset != 0) {
      powercycle_show_menu = 1;
      menu = &welcome_screen;
    }
  }

  if (load_from_jumperset != 0) {
    cfg_clear_words();  // just in case anything went wrong while loading from flash
    cfg_load_jumperset(0);
    powercycle_show_menu = 1;
//    cfg_save_to_flash(0);
  }

  cfg_offon_t use_fallback = get_fallback_mode();
  while (is_fallback_mode_valid() == 0) use_fallback = get_fallback_mode();

  if (use_fallback) {
    cfg_clear_words();  // just in case anything went wrong while loading from flash
    cfg_load_defaults(0);
    powercycle_show_menu = 1;
  }

  if (powercycle_show_menu) {
    open_osd_main(&menu);
  } else {
    cfg_clear_flag(&show_osd);
    cfg_clear_flag(&show_logo);
  }
  cfg_clear_flag(&show_testpat);
  cfg_clear_flag(&test_vpll);

  cfg_load_linex_word(NTSC);
  cfg_load_timing_word(NTSC_LX2_PR);
  cfg_set_value(&deblur_mode_current,cfg_get_value(&deblur_mode,0));
  cfg_set_value(&mode16bit_current,cfg_get_value(&mode16bit,0));
  cfg_apply_to_logic();

  vpll_lock_first_boot = 1;
  vpll_state_frame_delay = 0;


  /* Event loop never exits. */
  while (1) {
    ctrl_update = new_ctrl_available();
    update_ppu_state(); // also update commonly used ppu states (palmode, scanmode, linemult_mode)

    if (ctrl_update && !ctrl_ignore) {
      update_ctrl_data();
      command = ctrl_data_to_cmd(0);
    } else {
      ctrl_ignore = ctrl_ignore == 0 ? 0 : ctrl_ignore - 1;
      command = CMD_NON;
    }

    if (cfg_get_value(&pal_awareness,0)) {
      vmode_n64adv = palmode;
      vmode_menu = (vmode_t) cfg_get_value(&ntsc_pal_selection,0);
    } else {
      vmode_n64adv = NTSC;
      vmode_menu = NTSC;
    }
    if (linemult_mode > PASSTHROUGH) {
      if (palmode)
        timing_n64adv = scanmode ? PAL_LX2_INT : PAL_LX2_PR;
      else
        timing_n64adv = scanmode ? NTSC_LX2_INT :
            (linemult_mode == 2) ? NTSC_LX3_PR : NTSC_LX2_PR;
    } else {
      timing_n64adv = (cfg_timing_model_sel_type_t) palmode;
      timing_n64adv++;
    }
    timing_menu = cfg_get_value(&timing_selection,0);
    if (timing_menu == PPU_CURRENT) timing_menu = timing_n64adv;

    if(cfg_get_value(&show_osd,0)) {

      cfg_load_linex_word(vmode_menu);
      cfg_load_timing_word(timing_menu);

      if (message_cnt > 0) {
        if (command != CMD_NON) {
          command = CMD_NON;
          message_cnt = 1;
        }
        if (message_cnt == 1) vd_clear_area(RWM_H_OFFSET,RWM_H_OFFSET+RWM_LENGTH,RWM_V_OFFSET,RWM_V_OFFSET);
        message_cnt--;
      }

      todo = modify_menu(command,&menu);

      switch (todo) {
        case MENU_CLOSE:
          cfg_clear_flag(&show_osd);
          break;
        case MENU_MUTE:
          cfg_set_flag(&mute_osd_tmp);
          break;
        case MENU_UNMUTE:
          cfg_clear_flag(&mute_osd_tmp);
          break;
        case NEW_OVERLAY:
          print_overlay(menu);
          if (menu->header) cfg_set_flag(&show_logo);
          else              cfg_clear_flag(&show_logo);
          message_cnt = 0;
          /* no break */
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

      if (menu->type != TEXT) {
        vd_clear_area(0,VD_WIDTH/2,VD_HEIGHT-1,VD_HEIGHT-1);
        if (menu == &home_menu || menu == home_menu.leaves[0].submenu) print_ctrl_data();
        else print_current_timing_mode();
      }
      if (menu->type == VINFO) update_vinfo_screen(menu);
      if (menu->type == CONFIG) {
        update_cfg_screen(menu);
        cfg_store_linex_word(vmode_menu);
        cfg_store_timing_word(timing_menu);
      }

    } else { /* END OF if(cfg_get_value(&show_osd)) */

      if (command == CMD_OPEN_MENU) {
        open_osd_main(&menu);
        ctrl_ignore = CTRL_IGNORE_FRAMES;
      }

      if (cfg_get_value(&igr_deblur,0))
        switch (command) {
          case CMD_DEBLUR_QUICK_ON:
            if (scanmode == PROGRESSIVE) {
              cfg_set_flag(&deblur_mode_current);
            };
            break;
          case CMD_DEBLUR_QUICK_OFF:
            if (scanmode == PROGRESSIVE) {
              cfg_clear_flag(&deblur_mode_current);
            };
            break;
          default:
            break;
        }

      if (cfg_get_value(&igr_16bitmode,0))
          switch (command) {
            case CMD_16BIT_QUICK_ON:
              cfg_set_flag(&mode16bit_current);
              break;
            case CMD_16BIT_QUICK_OFF:
              cfg_clear_flag(&mode16bit_current);
              break;
            default:
              break;
          }

    } /* END OF if(!cfg_get_value(&show_osd)) */

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

    cfg_load_linex_word(vmode_n64adv);
    cfg_load_timing_word(timing_n64adv);
    cfg_apply_to_logic();

    while(!get_osdvsync()){};  /* wait for OSD_VSYNC goes high (OSD vert. active area) */
    while( get_osdvsync()){};  /* wait for OSD_VSYNC goes low  */
  }

  return 0;
}
