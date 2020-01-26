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
 * config.c
 *
 *  Created on: 11.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"

#include "n64.h"
#include "config.h"
#include "menu.h"
#include "flash.h"
#include "vd_driver.h"
#include "fw.h"


#define CFG2FLASH_WORD_FACTOR   4
typedef struct {
  alt_u8  vers_cfg_main;
  alt_u8  vers_cfg_sub;
  alt_u8  cfg_words[CFG2FLASH_WORD_FACTOR*NUM_CFG_B32WORDS];
} cfg4flash_t;

static const char *confirm_message = "< Really? >";
static const char *running_message = "< Running >";
extern const char *btn_overlay_1, *btn_overlay_2;

extern cfg_b32word_t cfg_data_video;
extern config_t deblur_frame_cnt_high, deblur_frame_cnt_low;

alt_u8 use_filteraddon;

void cfg_inc_value(config_t* cfg_data)
{
  if (cfg_data->cfg_type == FLAG) {
    cfg_toggle_flag(cfg_data);
    return;
  }

  alt_u32 *cfg_word = &(cfg_data->cfg_word->cfg_word_val);
  alt_u8 cur_val = (*cfg_word & cfg_data->value_details.getvalue_mask) >> cfg_data->cfg_word_offset;

  cur_val = cur_val == cfg_data->value_details.max_value ? 0 : cur_val + 1;
  *cfg_word = (*cfg_word & ~cfg_data->value_details.getvalue_mask) | (cur_val << cfg_data->cfg_word_offset);
};

void cfg_dec_value(config_t* cfg_data)
{
  if (cfg_data->cfg_type == FLAG) {
    cfg_toggle_flag(cfg_data);
    return;
  }

  alt_u32 *cfg_word = &(cfg_data->cfg_word->cfg_word_val);
  alt_u8 cur_val = (*cfg_word & cfg_data->value_details.getvalue_mask) >> cfg_data->cfg_word_offset;

  cur_val = cur_val == 0 ? cfg_data->value_details.max_value : cur_val - 1;
  *cfg_word = (*cfg_word & ~cfg_data->value_details.getvalue_mask) | (cur_val << cfg_data->cfg_word_offset);
};

void cfg_check_deblur_frame_cnt(alt_u8 set_low) {
  alt_u8 cur_val_high = cfg_get_value(&deblur_frame_cnt_high,0);
  alt_u8 cur_val_low = cfg_get_value(&deblur_frame_cnt_low,0);
  if (cur_val_low >= cur_val_high + 1) {
    if (set_low) {
      cur_val_low = cur_val_high;
      cfg_data_video.cfg_word_val = (cfg_data_video.cfg_word_val & ~deblur_frame_cnt_low.value_details.getvalue_mask) | (cur_val_low << deblur_frame_cnt_low.cfg_word_offset);
    } else {
      cur_val_high = cur_val_low;
      cfg_data_video.cfg_word_val = (cfg_data_video.cfg_word_val & ~deblur_frame_cnt_high.value_details.getvalue_mask) | (cur_val_high << deblur_frame_cnt_high.cfg_word_offset);
    }
  }
};

alt_u8 cfg_get_value(config_t* cfg_data, alt_u8 get_reference)
{
  alt_u32 cfg_word;
  if (!get_reference) cfg_word = cfg_data->cfg_word->cfg_word_val;
  else                cfg_word = cfg_data->cfg_word->cfg_ref_word_val;

  if (cfg_data->cfg_type == FLAG) return ((cfg_word & cfg_data->flag_masks.setflag_mask)     >> cfg_data->cfg_word_offset);
  else                            return ((cfg_word & cfg_data->value_details.getvalue_mask) >> cfg_data->cfg_word_offset);
};

void cfg_set_value(config_t* cfg_data, alt_u8 value)
{
  if (cfg_data->cfg_type == FLAG) {
    if (value) cfg_set_flag(cfg_data);
    else       cfg_clear_flag(cfg_data);
  } else {
    alt_u32 *cfg_word = cfg_data->cfg_word->cfg_word_val;
    alt_u32 cur_val = value > cfg_data->value_details.max_value ? 0 : value;

    *cfg_word = (*cfg_word & ~cfg_data->value_details.getvalue_mask) | (cur_val << cfg_data->cfg_word_offset);
  }
};


int cfg_show_testpattern(configuration_t* sysconfig)
{
  extern config_t show_testpat;

  cfg_set_flag(&show_testpat);
  cfg_apply_to_logic(sysconfig);

  cmd_t command;
  alt_u32 ctrl_data = get_ctrl_data();

  while(1) {
    while(!get_nvsync()){};                          // wait for nVSYNC goes high
    while( get_nvsync() && new_ctrl_available()){};  // wait for nVSYNC goes low and
                                                     // wait for new controller available
    ctrl_data = get_ctrl_data();
    command = ctrl_data_to_cmd(&ctrl_data,1);
    if (command == CMD_MENU_BACK) break;
  }

  cfg_clear_flag(&show_testpat);
  cfg_apply_to_logic(sysconfig);

  return 0;
}

alt_u8 confirmation_routine()
{
  cmd_t command;
  alt_u8 abort = 0;
  alt_u32 ctrl_data = get_ctrl_data();

  vd_print_string(RWM_H_OFFSET,RWM_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_NAVAJOWHITE,confirm_message);
  vd_print_string(BTN_OVERLAY_H_OFFSET,BTN_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREEN,btn_overlay_2);

  while(1) {
    print_ctrl_data(&ctrl_data);

    while(!get_nvsync()){};                          /* wait for nVSYNC goes high */
    while( get_nvsync() && new_ctrl_available()){};  /* wait for nVSYNC goes low and
                                                        wait for new controller available  */
    ctrl_data = get_ctrl_data();
    command = ctrl_data_to_cmd(&ctrl_data,1);

    if ((command == CMD_MENU_ENTER) || (command == CMD_MENU_RIGHT)) break;
    if ((command == CMD_MENU_BACK)  || (command == CMD_MENU_LEFT))  {abort = 1; break;};
  }
  vd_clear_lineend (BTN_OVERLAY_H_OFFSET,BTN_OVERLAY_V_OFFSET);
  vd_clear_lineend (BTN_OVERLAY_H_OFFSET,BTN_OVERLAY_V_OFFSET+1);
  vd_print_string(BTN_OVERLAY_H_OFFSET,BTN_OVERLAY_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_GREEN,btn_overlay_1);
  return abort;
}

int cfg_save_to_flash(configuration_t* sysconfig, alt_u8 need_confirm)
{
  if (!use_flash) return -CFG_FLASH_NOT_USED;

  if (need_confirm) {
    alt_u8 abort = confirmation_routine();
    if (abort) return -CFG_FLASH_SAVE_ABORT;
  }

  alt_u8 databuf[PAGESIZE];
  int idx, jdx;

  ((cfg4flash_t*) databuf)->vers_cfg_main = CFG_FW_MAIN;
  ((cfg4flash_t*) databuf)->vers_cfg_sub = CFG_FW_SUB;
  for (idx = 0; idx < NUM_CFG_B32WORDS; idx++) {
    for (jdx = 0; jdx < CFG2FLASH_WORD_FACTOR; jdx++)
      ((cfg4flash_t*) databuf)->cfg_words[CFG2FLASH_WORD_FACTOR*idx+jdx] = ((0xFF << (8*jdx) & sysconfig->cfg_word_def[idx]->cfg_word_val) >> (8*jdx));
    sysconfig->cfg_word_def[idx]->cfg_ref_word_val = sysconfig->cfg_word_def[idx]->cfg_word_val;
  }

  return write_flash_page((alt_u8*) databuf, sizeof(cfg4flash_t), USERDATA_OFFSET/PAGESIZE);
};

int cfg_load_from_flash(configuration_t* sysconfig, alt_u8 need_confirm)
{
  if (!use_flash) return -CFG_FLASH_NOT_USED;

  if (need_confirm) {
    alt_u8 abort = confirmation_routine();
    if (abort) return -CFG_FLASH_LOAD_ABORT;
  }

  alt_u8 databuf[PAGESIZE];
  int idx, jdx, retval;

  retval = read_flash(USERDATA_OFFSET, PAGESIZE, databuf);

  if (retval != 0) return retval;

  if ((((cfg4flash_t*) databuf)->vers_cfg_main != CFG_FW_MAIN) ||
      (((cfg4flash_t*) databuf)->vers_cfg_sub  != CFG_FW_SUB)   ) return -CFG_VERSION_INVALID;

  for (idx = 0; idx < NUM_CFG_B32WORDS; idx++) {
	  sysconfig->cfg_word_def[idx]->cfg_word_val = 0;
    for (jdx = 0; jdx < CFG2FLASH_WORD_FACTOR; jdx++)
      sysconfig->cfg_word_def[idx]->cfg_word_val |= (((cfg4flash_t*) databuf)->cfg_words[CFG2FLASH_WORD_FACTOR*idx + jdx] << (8*jdx));
  }

  cfg_update_reference(sysconfig);

  return 0;
};

int cfg_load_defaults(configuration_t* sysconfig, alt_u8 need_confirm)
{
  if (need_confirm) {
    alt_u8 abort = confirmation_routine();
    if (abort) return -CFG_DEF_LOAD_ABORT;
  }

  sysconfig->cfg_word_def[MISC]->cfg_word_val &= CFG_MISC_GET_NODEFAULTS;
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val &= CFG_VIDEO_GET_NODEFAULTS;
  sysconfig->cfg_word_def[IMAGE]->cfg_word_val &= CFG_IMAGE_GET_NODEFAULTS;

  sysconfig->cfg_word_def[MISC]->cfg_word_val |= CFG_MISC_DEFAULT;
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val |= CFG_VIDEO_DEFAULT;
  sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= CFG_IMAGE_DEFAULT;

  return 0;
}

int cfg_load_jumperset(configuration_t* sysconfig, alt_u8 need_confirm)
{
  if (need_confirm) {
    alt_u8 abort = confirmation_routine();
    if (abort) return -CFG_JUMPER_LOAD_ABORT;
  }

  cfg_load_defaults(sysconfig,0); // get N64 defaults (incl. video format)

  alt_u8 jumper_word = cfg_get_jumper();

  if (~(jumper_word & JUMPER_BYPASS_FILTER_GETMASK)) {
    sysconfig->cfg_word_def[VIDEO]->cfg_word_val |= CFG_FILTER_AUTO_SETMASK;
  }
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val &= CFG_DEBLUR_MODE_RSTMASK;
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val |= CFG_DEBLUR_MODE_AUTO_SETMASK;

  if (jumper_word & JUMPER_LINEX2_GETMASK) {
    sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= (1 << CFG_240P_LINEX_OFFSET);
    if (jumper_word & JUMPER_480I_BOB_DEINTER_GETMASK)
      sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= (CFG_480I_FIELDFIX_SETMASK | CFG_480I_BOB_DEINTER_SETMASK);
  }

  sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= (CFG_240P_SL_ID_SETMASK | CFG_240P_SL_EN_SETMASK);
  switch ((jumper_word & JUMPER_SLSTR_GETMASK) >> JUMPER_SLSTR_OFFSET) {
    case 1:
      sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= ((0x3<<CFG_240P_SLSTR_OFFSET) | (0x3<<CFG_480I_SLSTR_OFFSET)); // 25%
      break;
    case 2:
      sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= ((0x7<<CFG_240P_SLSTR_OFFSET) | (0x7<<CFG_480I_SLSTR_OFFSET)); // 50%
      break;
    case 3:
      sysconfig->cfg_word_def[IMAGE]->cfg_word_val |= ((0xF<<CFG_240P_SLSTR_OFFSET) | (0xF<<CFG_480I_SLSTR_OFFSET)); // 100%
      break;
    default:
      sysconfig->cfg_word_def[IMAGE]->cfg_word_val &= (CFG_240P_SL_EN_CLRMASK);                                      // 0%
      break;
  }

  sysconfig->cfg_word_def[VIDEO]->cfg_word_val &= CFG_VFORMAT_CLRMASK;
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val |= (((jumper_word & JUMPER_SOG_GETMASK) >> JUMPER_VFORMAT_OFFSET) << CFG_VFORMAT_OFFSET);

  return 0;
}

void cfg_apply_to_logic(configuration_t* sysconfig)
{
  IOWR_ALTERA_AVALON_PIO_DATA(CFG_MISC_OUT_BASE,sysconfig->cfg_word_def[MISC]->cfg_word_val);
  IOWR_ALTERA_AVALON_PIO_DATA(CFG_VIDEO_OUT_BASE,sysconfig->cfg_word_def[VIDEO]->cfg_word_val);
  IOWR_ALTERA_AVALON_PIO_DATA(CFG_IMAGE_OUT_BASE,sysconfig->cfg_word_def[IMAGE]->cfg_word_val);
}

void cfg_read_from_logic(configuration_t* sysconfig)
{
  sysconfig->cfg_word_def[MISC]->cfg_word_val  = (IORD_ALTERA_AVALON_PIO_DATA(CFG_MISC_OUT_BASE)  & sysconfig->cfg_word_def[MISC]->cfg_word_mask);
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val = (IORD_ALTERA_AVALON_PIO_DATA(CFG_VIDEO_OUT_BASE) & sysconfig->cfg_word_def[VIDEO]->cfg_word_mask);
  sysconfig->cfg_word_def[IMAGE]->cfg_word_val = (IORD_ALTERA_AVALON_PIO_DATA(CFG_IMAGE_OUT_BASE) & sysconfig->cfg_word_def[IMAGE]->cfg_word_mask);
}

void cfg_clear_words(configuration_t* sysconfig)
{
  int idx;
  for (idx = 0; idx < NUM_CFG_B32WORDS; idx++)
    sysconfig->cfg_word_def[idx]->cfg_word_val = 0;
}

void cfg_update_reference(configuration_t* sysconfig)
{
  int idx;
  for (idx = 0; idx < NUM_CFG_B32WORDS; idx++)
    sysconfig->cfg_word_def[idx]->cfg_ref_word_val = sysconfig->cfg_word_def[idx]->cfg_word_val;
}

void enable_vpll_test()
{
  alt_u32 cfg_word = IORD_ALTERA_AVALON_PIO_DATA(CFG_MISC_OUT_BASE) | CFG_TEST_VPLL_SETMASK;
  IOWR_ALTERA_AVALON_PIO_DATA(CFG_MISC_OUT_BASE,cfg_word);
}

void disable_vpll_test()
{
  alt_u32 cfg_word = IORD_ALTERA_AVALON_PIO_DATA(CFG_MISC_OUT_BASE) & CFG_TEST_VPLL_CLRMASK;
  IOWR_ALTERA_AVALON_PIO_DATA(CFG_MISC_OUT_BASE,cfg_word);
}

int run_vpll_test(configuration_t* sysconfig)
{
  alt_u8 vpll_lock = update_vpll_lock_state();
  if (vpll_lock) return 0;

  vd_print_string(RWM_H_OFFSET,RWM_V_OFFSET,BACKGROUNDCOLOR_STANDARD,FONTCOLOR_YELLOW,running_message);
  enable_vpll_test();
  int i;
  for ( i = 0; i < VPLL_START_FRAMES; i++) { /* wait for five frames for PLL */
    while(!get_nvsync()){};  /* wait for nVSYNC goes high */
    while( get_nvsync()){};  /* wait for nVSYNC goes low  */
    vpll_lock = update_vpll_lock_state();
    if (vpll_lock) return 0;
  }
  disable_vpll_test();
  return -VPLL_TEST_FAILED;
}
