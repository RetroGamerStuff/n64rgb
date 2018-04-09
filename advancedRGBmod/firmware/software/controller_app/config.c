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
#include "fw.h"


typedef struct {
  alt_u8  vers_cfg_main;
  alt_u8  vers_cfg_sub;
  alt_u8  cfg_words[NUM_CFG_WORDS];
} cfg4flash_t;

static const char *confirm_message = "< Really? >";
extern const char *btn_overlay_1, *btn_overlay_2;

void cfg_inc_value(config_t* cfg_data)
{
  if (cfg_data->cfg_type == FLAG) {
    cfg_toggle_flag(cfg_data);
    return;
  }

  alt_u8 cfg_word = cfg_data->cfg_word->cfg_word_val;
  alt_u8 cur_val = (cfg_word & cfg_data->value_details.getvalue_mask) >> cfg_data->cfg_word_offset;

  cur_val = cur_val == cfg_data->value_details.max_value ? 0 : cur_val + 1;
  cfg_word = (cfg_word & ~cfg_data->value_details.getvalue_mask) | (cur_val << cfg_data->cfg_word_offset);

  cfg_data->cfg_word->cfg_word_val = cfg_word;
};

void cfg_dec_value(config_t* cfg_data)
{
  if (cfg_data->cfg_type == FLAG) {
    cfg_toggle_flag(cfg_data);
    return;
  }

  alt_u8 cfg_word = cfg_data->cfg_word->cfg_word_val;
  alt_u8 cur_val = (cfg_word & cfg_data->value_details.getvalue_mask) >> cfg_data->cfg_word_offset;

  cur_val = cur_val == 0 ? cfg_data->value_details.max_value : cur_val - 1;
  cfg_word = (cfg_word & ~cfg_data->value_details.getvalue_mask) | (cur_val << cfg_data->cfg_word_offset);

  cfg_data->cfg_word->cfg_word_val = cfg_word;
};

alt_u8 cfg_get_value(config_t* cfg_data,alt_u8 get_reference)
{
  alt_u8 cfg_word;
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
    alt_u8 cfg_word = cfg_data->cfg_word->cfg_word_val;
    alt_u8 cur_val = value > cfg_data->value_details.max_value ? 0 : value;

    cfg_word = (cfg_word & ~cfg_data->value_details.getvalue_mask) | (cur_val << cfg_data->cfg_word_offset);

    cfg_data->cfg_word->cfg_word_val = cfg_word;
  }
};

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
-                                                       wait for new controller available  */
    ctrl_data = get_ctrl_data();
    command = ctrl_data_to_cmd(&ctrl_data);

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
  int idx;

  ((cfg4flash_t*) databuf)->vers_cfg_main = CFG_FW_MAIN;
  ((cfg4flash_t*) databuf)->vers_cfg_sub = CFG_FW_SUB;
  for (idx = 0; idx < NUM_CFG_WORDS; idx++) {
    ((cfg4flash_t*) databuf)->cfg_words[idx] = sysconfig->cfg_word_def[idx]->cfg_word_val;
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
  int idx, retval;

  retval = read_flash(USERDATA_OFFSET, PAGESIZE, databuf);

  if (retval != 0) return retval;

  if ((((cfg4flash_t*) databuf)->vers_cfg_main != CFG_FW_MAIN) ||
      (((cfg4flash_t*) databuf)->vers_cfg_sub  != CFG_FW_SUB)   ) return -CFG_VERSION_INVALID;

  for (idx = 0; idx < NUM_CFG_WORDS; idx++) {
    sysconfig->cfg_word_def[idx]->cfg_word_val     = ((cfg4flash_t*) databuf)->cfg_words[idx];
    sysconfig->cfg_word_def[idx]->cfg_ref_word_val = sysconfig->cfg_word_def[idx]->cfg_word_val;
  }

  cfg_update_reference(sysconfig);

  return 0;
};

int cfg_load_n64defaults(configuration_t* sysconfig, alt_u8 need_confirm)
{
  if (need_confirm) {
    alt_u8 abort = confirmation_routine();
    if (abort) return -CFG_N64DEF_LOAD_ABORT;
  }

  cfg_load_jumperset(sysconfig,0); // to get vmode and set filter addon (if applied)

  sysconfig->cfg_word_def[MISC]->cfg_word_val &= N64_MISC_CLR_MASK;
  sysconfig->cfg_word_def[MISC]->cfg_word_val |= N64_DEFAULT_MISC_CFG;
  sysconfig->cfg_word_def[IMAGE2]->cfg_word_val &= N64_IMAGE2_CLR_MASK;
  sysconfig->cfg_word_def[IMAGE2]->cfg_word_val |= N64_DEFAULT_IMAGE2_CFG;
  sysconfig->cfg_word_def[IMAGE1]->cfg_word_val &= N64_IMAGE1_CLR_MASK;
//  sysconfig->cfg_word_def[IMAGE1]->cfg_word_val |= N64_DEFAULT_IMAGE1_CFG;
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val &= N64_VIDEO_CLR_MASK;
//  sysconfig->cfg_word_def[VIDEO]->cfg_word_val |= N64_DEFAULT_VIDEO_CFG;

  return 0;
}

int cfg_load_jumperset(configuration_t* sysconfig, alt_u8 need_confirm)
{
  if (need_confirm) {
    alt_u8 abort = confirmation_routine();
    if (abort) return -CFG_JUMPER_LOAD_ABORT;
  }

  alt_u8 jumper_word = cfgopt_get_jumper();

  sysconfig->cfg_word_def[MISC]->cfg_word_val  &= JUMPER_MISC_CLR_MASK;
  sysconfig->cfg_word_def[IMAGE2]->cfg_word_val &= JUMPER_IMAGE2_CLR_MASK;
  sysconfig->cfg_word_def[IMAGE2]->cfg_word_val |= (N64_DEFAULT_IMAGE2_CFG);
  sysconfig->cfg_word_def[IMAGE1]->cfg_word_val &= JUMPER_IMAGE1_CLR_MASK;
  sysconfig->cfg_word_def[IMAGE1]->cfg_word_val |= ((N64_DEFAULT_IMAGE1_CFG) | CFG_SL_ID_SETMASK | CFG_SL_EN_SETMASK);
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val &= JUMPER_VIDEO_CLR_MASK;
  sysconfig->cfg_word_def[VIDEO]->cfg_word_val |= (jumper_word & JUMPER_VCFG_GETALL_MASK);

  if (use_filteraddon) {
    if (jumper_word & JUMPER_MCFG_FILTER_GETMASK)
      sysconfig->cfg_word_def[MISC]->cfg_word_val |= CFG_FILTER_AUTO_SETMASK;
    else
      sysconfig->cfg_word_def[MISC]->cfg_word_val |= CFG_FILTER_OFF_SETMASK;
  }

  switch ((jumper_word & JUMPER_ICFG_SLSTR_GETMASK) >> JUMPER_SLSTR_OFFSET) {
    case 1:
      sysconfig->cfg_word_def[IMAGE2]->cfg_word_val |= (0x3<<CFG_SLSTR_OFFSET); // 25%
      break;
    case 2:
      sysconfig->cfg_word_def[IMAGE2]->cfg_word_val |= (0x7<<CFG_SLSTR_OFFSET); // 50%
      break;
    case 3:
      sysconfig->cfg_word_def[IMAGE2]->cfg_word_val |= (0xF<<CFG_SLSTR_OFFSET); // 100%
      break;
    default:
      sysconfig->cfg_word_def[IMAGE1]->cfg_word_val &= CFG_SL_EN_CLRMASK;       // 0%
      break;
  }

  return 0;
}

void cfgopt_apply_to_logic(configuration_t* sysconfig)
{
  int idx;
  alt_u32 wr_word = 0;
  for (idx = 0; idx < NUM_OPT_WORDS; idx++)
    wr_word |= ((sysconfig->cfg_word_def[idx]->cfg_word_val & sysconfig->cfg_word_def[idx]->cfg_word_mask) << (8*sysconfig->cfg_word_def[idx]->cfg_word_type));
  IOWR_ALTERA_AVALON_PIO_DATA(CFG_SET_OUT_BASE,wr_word);
}

void cfg_clear_words(configuration_t* sysconfig)
{
  int idx;
  for (idx = 0; idx < NUM_CFG_WORDS; idx++)
    sysconfig->cfg_word_def[idx]->cfg_word_val = 0;
}

void cfgopt_load_from_ios(configuration_t* sysconfig)
{
  int idx;
  alt_u32 rd_word = cfgopt_get_from_logic();
  for (idx = 0; idx < NUM_OPT_WORDS; idx++)
    sysconfig->cfg_word_def[idx]->cfg_word_val = (rd_word >> (8*sysconfig->cfg_word_def[idx]->cfg_word_type)) & sysconfig->cfg_word_def[idx]->cfg_word_mask;
}

void cfg_update_reference(configuration_t* sysconfig)
{
  int idx;
  for (idx = 0; idx < NUM_CFG_WORDS; idx++)
    sysconfig->cfg_word_def[idx]->cfg_ref_word_val = sysconfig->cfg_word_def[idx]->cfg_word_val;
}
