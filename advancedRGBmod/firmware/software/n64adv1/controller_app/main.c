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
#include "cfg_int_p.h"
#include "n64.h"
#include "config.h"
#include "menu.h"
#include "vd_driver.h"
#include "flash.h"

#define CTRL_IGNORE_FRAMES 10;

const alt_u8 RW_Message_FontColor[] = {FONTCOLOR_GREEN,FONTCOLOR_RED,FONTCOLOR_MAGENTA};
const char   *RW_Message[] = {"< Success >","< Failed > ","< Aborted >"};

alt_u8 boot_welcome = 0;

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

  configuration_t sysconfig = {
      .cfg_word_def[MISC]  = &cfg_data_misc,
      .cfg_word_def[VIDEO] = &cfg_data_video,
      .cfg_word_def[LINEX] = &cfg_data_linex,
  };

  alt_u8 linex_word_menu, linex_word_n64adv;
  alt_u8 timing_menu, timing_n64adv;

  cfg_clear_words(&sysconfig);

  alt_u8 ctrl_update = 1;
  alt_u32 ctrl_data;
  alt_u8 ctrl_ignore = 0;
  alt_u16 ppu_state;
  alt_u8 palmode, interlaced_mode, linemult_mode;
  alt_u8 vpll_lock_first_boot;
  alt_u8 vpll_state_frame_delay;

  int message_cnt = 0;

  check_filteraddon();

  alt_u8 powercycle_show_menu = 0;
  int load_from_jumperset = check_flash();
  if (use_flash) {
    load_from_jumperset = cfg_load_from_flash(&sysconfig,0);
    if (boot_welcome == 1 || load_from_jumperset != 0) {
      powercycle_show_menu = 1;
      menu = &welcome_screen;
    }
  }

  if (load_from_jumperset != 0) {
    cfg_clear_words(&sysconfig);  // just in case anything went wrong while loading from flash
    cfg_load_jumperset(&sysconfig,0);
    powercycle_show_menu = 1;
//    cfg_save_to_flash(&sysconfig,0);
  }

  alt_u8 use_fallback = get_fallback_mode();
  while (is_fallback_mode_valid() == 0) use_fallback = get_fallback_mode();

  if (use_fallback) {
    cfg_clear_words(&sysconfig);  // just in case anything went wrong while loading from flash
    cfg_load_defaults(&sysconfig,0);
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

  cfg_load_linex_word(&sysconfig,NTSC);
  cfg_load_timing_word(&sysconfig,NTSC_LX2_PR);
  cfg_set_value(&deblur_mode_current,cfg_get_value(&deblur_mode,0));
  cfg_set_value(&mode16bit_current,cfg_get_value(&mode16bit,0));
  cfg_apply_to_logic(&sysconfig);

  vpll_lock_first_boot = 1;
  vpll_state_frame_delay = 0;


  /* Event loop never exits. */
  while (1) {
    if (ctrl_update && !ctrl_ignore) {
      ctrl_data = get_ctrl_data();
      command = ctrl_data_to_cmd(&ctrl_data,0);
    } else {
      ctrl_ignore = ctrl_ignore == 0 ? 0 : ctrl_ignore - 1;
      command = CMD_NON;
    }

    if (cfg_get_value(&pal_awareness,0)) {
      linex_word_n64adv = (ppu_state & PPU_STATE_PALMODE_GETMASK) >> PPU_STATE_PALMODE_OFFSET;
      linex_word_menu = cfg_get_value(&ntsc_pal_selection,0);
    } else {
        linex_word_n64adv = NTSC;
        linex_word_menu = NTSC;
    }
    ppu_state = get_ppu_state();
    palmode = (ppu_state & PPU_STATE_PALMODE_GETMASK) >> PPU_STATE_PALMODE_OFFSET;
    interlaced_mode = (ppu_state & PPU_STATE_480I_GETMASK) >> PPU_STATE_480I_OFFSET;
    linemult_mode = (ppu_state & PPU_STATE_LINEMULT_GETMASK) >> PPU_STATE_LINEMULT_OFFSET;
    if (palmode)
      timing_n64adv = interlaced_mode ? PAL_LX2_INT : PAL_LX2_PR;
    else
      timing_n64adv = interlaced_mode ? NTSC_LX2_INT :
        (linemult_mode == 2) ? NTSC_LX3_PR : NTSC_LX2_PR;

    timing_menu = cfg_get_value(&timing_selection,0);
    if (timing_menu == PPU_CURRENT) timing_menu = (linemult_mode == 0) ? PPU_CURRENT: timing_n64adv;


    if(cfg_get_value(&show_osd,0)) {

      cfg_load_linex_word(&sysconfig,linex_word_menu);
      cfg_load_timing_word(&sysconfig,timing_menu);

      if (message_cnt > 0) {
        if (command != CMD_NON) {
          command = CMD_NON;
          message_cnt = 1;
        }
        if (message_cnt == 1) vd_clear_area(RWM_H_OFFSET,RWM_H_OFFSET+RWM_LENGTH,RWM_V_OFFSET,RWM_V_OFFSET);
        message_cnt--;
      }

      todo = modify_menu(command,&menu,&sysconfig,&ppu_state);

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

      if (menu->type != TEXT) {
        vd_clear_area(0,VD_WIDTH/2,VD_HEIGHT-1,VD_HEIGHT-1);
        if (menu == &home_menu) print_ctrl_data(&ctrl_data);
        else print_current_mode(palmode,linemult_mode,timing_n64adv);
      }
      update_vinfo_screen(menu,&ppu_state);
      update_cfg_screen(menu,linemult_mode,timing_n64adv);
      if (menu->type == CONFIG) {
        cfg_store_linex_word(&sysconfig,linex_word_menu);
        cfg_store_timing_word(&sysconfig,timing_menu);
      }

      if (!cfg_get_value(&pal_awareness,0))
        cfg_set_value(&ntsc_pal_selection,NTSC);

    } else { /* END OF if(cfg_get_value(&show_osd)) */

      if (command == CMD_OPEN_MENU) {
        open_osd_main(&menu);
        ctrl_ignore = CTRL_IGNORE_FRAMES;
      }

      if (cfg_get_value(&igr_deblur,0))
        switch (command) {
          case CMD_DEBLUR_QUICK_ON:
            if (!interlaced_mode) {
              cfg_set_flag(&deblur_mode_current);
            };
            break;
          case CMD_DEBLUR_QUICK_OFF:
            if (!interlaced_mode) {
              cfg_clear_flag(&deblur_mode_current);
            };
            break;
          default:
            break;
        }

      if (cfg_get_value(&igr_15bitmode,0))
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

    cfg_load_linex_word(&sysconfig,linex_word_n64adv);
    cfg_load_timing_word(&sysconfig,timing_n64adv);
    cfg_apply_to_logic(&sysconfig);

    while(!get_osdvsync()){};  /* wait for OSD_VSYNC goes high (OSD vert. active area) */
    while( get_osdvsync()){};  /* wait for OSD_VSYNC goes low  */
    ctrl_update = new_ctrl_available();
    ppu_state = get_ppu_state();
  }

  return 0;
}
