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
 * config.h
 *
 *  Created on: 11.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"


#ifndef CONFIG_H_
#define CONFIG_H_

typedef enum {
  MISC_MENU = 0,
  VIDEO,
  IMAGE_240P,
  IMAGE_480I,
} cfg_word_type_t;

#define NUM_CFG_WORDS 4

typedef struct {
  const alt_u16         cfg_word_mask;
  alt_u16               cfg_word_val;
  alt_u16               cfg_ref_word_val;
} cfg_word_t;

typedef struct {
  cfg_word_t* cfg_word_def[NUM_CFG_WORDS];
} configuration_t;

typedef enum {
  FLAG,
  VALUE
} config_type_t;

typedef struct {
  alt_u16 setflag_mask;
  alt_u16 clrflag_mask;
} config_flag_t;

typedef struct {
  alt_u16 max_value;
  alt_u16 getvalue_mask;
} config_value_t;

typedef struct {
  cfg_word_t          *cfg_word;
  const alt_u8        cfg_word_offset;
  const config_type_t cfg_type;
  union {
    const config_flag_t  flag_masks;
    const config_value_t value_details;
  };
  const char*         *value_string;
} config_t;

#define CFG_VERSION_INVALID   100
#define CFG_FLASH_NOT_USED    101
#define CFG_FLASH_SAVE_ABORT  102
#define CFG_FLASH_LOAD_ABORT  CFG_FLASH_SAVE_ABORT
#define CFG_N64DEF_LOAD_ABORT CFG_FLASH_SAVE_ABORT

// the overall masks

#define CFG_MISC_GETALL_MASK      0x0F07
#define CFG_VIDEO_GETALL_MASK     0x00F7
#define CFG_IMAGE240P_GETALL_MASK 0x3FF7
#define CFG_IMAGE480I_GETALL_MASK CFG_IMAGE240P_GETALL_MASK

// misc
#define CFG_SLOSD_OFFSET        11
#define CFG_SHOWLOGO_OFFSET     10
#define CFG_SHOWOSD_OFFSET       9
#define CFG_MUTEOSDTMP_OFFSET    8
#define CFG_USEIGR_OFFSET        2
#define CFG_QUICKCHANGE_OFFSET   0
  #define CFG_QU15BITMODE_OFFSET   1
  #define CFG_QUDEBLUR_OFFSET      0

#define CFG_SLOSD_GETMASK         (1<<CFG_SLOSD_OFFSET)
#define CFG_SLOSD_SETMASK         (1<<CFG_SLOSD_OFFSET)
#define CFG_SLOSD_CLRMASK         (CFG_MISC_GETALL_MASK & ~CFG_SLOSD_SETMASK)
#define CFG_SHOWLOGO_GETMASK       (1<<CFG_SHOWLOGO_OFFSET)
#define CFG_SHOWLOGO_SETMASK       (1<<CFG_SHOWLOGO_OFFSET)
#define CFG_SHOWLOGO_CLRMASK       (CFG_MISC_GETALL_MASK & ~CFG_SHOWLOGO_SETMASK)
#define CFG_SHOWOSD_GETMASK       (1<<CFG_SHOWOSD_OFFSET)
#define CFG_SHOWOSD_SETMASK       (1<<CFG_SHOWOSD_OFFSET)
#define CFG_SHOWOSD_CLRMASK       (CFG_MISC_GETALL_MASK & ~CFG_SHOWOSD_SETMASK)
#define CFG_MUTEOSDTMP_GETMASK    (1<<CFG_MUTEOSDTMP_OFFSET)
#define CFG_MUTEOSDTMP_SETMASK    (1<<CFG_MUTEOSDTMP_OFFSET)
#define CFG_MUTEOSDTMP_CLRMASK    (CFG_MISC_GETALL_MASK & ~CFG_MUTEOSDTMP_SETMASK)
#define CFG_USEIGR_GETMASK      (1<<CFG_USEIGR_OFFSET)
#define CFG_USEIGR_SETMASK      (1<<CFG_USEIGR_OFFSET)
#define CFG_USEIGR_CLRMASK      (CFG_MISC_GETALL_MASK & ~CFG_USEIGR_SETMASK)
#define CFG_QUICKCHANGE_GETMASK (0x3<<CFG_QUICKCHANGE_OFFSET)
  #define CFG_QUICKCHANGE_RSTMASK (CFG_MISC_GETALL_MASK & ~CFG_QUICKCHANGE_GETMASK)
  #define CFG_QUDEBLUR_SETMASK    (1<<CFG_QUDEBLUR_OFFSET)
  #define CFG_QUDEBLUR_GETMASK    (1<<CFG_QUDEBLUR_OFFSET)
  #define CFG_QUDEBLUR_CLRMASK    (CFG_MISC_GETALL_MASK & ~CFG_QUDEBLUR_SETMASK)
  #define CFG_QU15BITMODE_SETMASK (1<<CFG_QU15BITMODE_OFFSET)
  #define CFG_QU15BITMODE_GETMASK (1<<CFG_QU15BITMODE_OFFSET)
  #define CFG_QU15BITMODE_CLRMASK (CFG_MISC_GETALL_MASK & ~CFG_QU15BITMODE_SETMASK)


// video

#define CFG_GAMMA_OFFSET         4
#define CFG_DEBLUR_OFFSET        1
#define CFG_15BITMODE_OFFSET     0

#define CFG_GAMMA_GETMASK         (0xF<<CFG_GAMMA_OFFSET)
  #define CFG_GAMMASEL_RSTMASK      (CFG_VIDEO_GETALL_MASK & ~CFG_GAMMA_GETMASK)
#define CFG_GAMMA_CLRMASK         (CFG_VIDEO_GETALL_MASK & ~CFG_GAMMA_GETMASK)
#define CFG_DEBLUR_GETMASK        (3<<CFG_DEBLUR_OFFSET)
  #define CFG_DEBLUR_RSTMASK        (CFG_VIDEO_GETALL_MASK & ~CFG_DEBLUR_GETMASK)
  #define CFG_DEBLUR_ON_SETMASK     (2<<CFG_DEBLUR_OFFSET)
  #define CFG_DEBLUR_OFF_SETMASK    (1<<CFG_DEBLUR_OFFSET)
  #define CFG_DEBLUR_AUTO_SETMASK   (0<<CFG_DEBLUR_OFFSET)
#define CFG_DEBLUR_CLRMASK        (CFG_VIDEO_GETALL_MASK & ~CFG_DEBLUR_GETMASK)
#define CFG_15BITMODE_GETMASK     (1<<CFG_15BITMODE_OFFSET)
#define CFG_15BITMODE_SETMASK     (1<<CFG_15BITMODE_OFFSET)
#define CFG_15BITMODE_CLRMASK     (CFG_VIDEO_GETALL_MASK & ~CFG_15BITMODE_SETMASK)

// image for 240p
#define CFG_LINEX2_OFFSET       13
#define CFG_SLHYBDEP_OFFSET      8
  #define CFG_SLHYBDEPMSB_OFFSET  12
  #define CFG_SLHYBDEPLSB_OFFSET  8
#define CFG_SLSTR_OFFSET         4
  #define CFG_SLSTRMSB_OFFSET      7
  #define CFG_SLSTRLSB_OFFSET      4
#define CFG_SL_METHOD_OFFSET    2
#define CFG_SL_ID_OFFSET        1
#define CFG_SL_EN_OFFSET        0

#define CFG_LINEX2_GETMASK        (1<<CFG_LINEX2_OFFSET)
#define CFG_LINEX2_SETMASK        (1<<CFG_LINEX2_OFFSET)
#define CFG_LINEX2_CLRMASK        (CFG_IMAGE240P_GETALL_MASK & ~CFG_LINEX2_SETMASK)
#define CFG_SLHYBDEP_GETMASK      (0x1F<<CFG_SLHYBDEP_OFFSET)
  #define CFG_SLHYBDEP_RSTMASK      (CFG_IMAGE240P_GETALL_MASK & ~CFG_SLHYBDEP_GETMASK)
#define CFG_SLHYBDEP_CLRMASK        (CFG_IMAGE240P_GETALL_MASK & ~CFG_SLHYBDEP_GETMASK)
#define CFG_SLSTR_GETMASK         (0xF<<CFG_SLSTR_OFFSET)
  #define CFG_SLSTR_RSTMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_SLSTR_GETMASK)
#define CFG_SLSTR_CLRMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_SLSTR_GETMASK)
#define CFG_SL_METHOD_GETMASK     (1<<CFG_SL_METHOD_OFFSET)
#define CFG_SL_METHOD_SETMASK     (1<<CFG_SL_METHOD_OFFSET)
#define CFG_SL_METHOD_CLRMASK     (CFG_IMAGE240P_GETALL_MASK & ~CFG_SL_METHOD_SETMASK)
#define CFG_SL_ID_GETMASK         (1<<CFG_SL_ID_OFFSET)
#define CFG_SL_ID_SETMASK         (1<<CFG_SL_ID_OFFSET)
#define CFG_SL_ID_CLRMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_SL_ID_SETMASK)
#define CFG_SL_EN_GETMASK         (1<<CFG_SL_EN_OFFSET)
#define CFG_SL_EN_SETMASK         (1<<CFG_SL_EN_OFFSET)
#define CFG_SL_EN_CLRMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_SL_EN_SETMASK)


// image for 480i

#define CFG_SL_LINK240P_OFFSET  2

#define CFG_SL_LINK240P_GETMASK (1<<CFG_SL_LINK240P_OFFSET)
#define CFG_SL_LINK240P_SETMASK (1<<CFG_SL_LINK240P_OFFSET)
#define CFG_SL_LINK240P_CLRMASK (CFG_IMAGE480I_GETALL_MASK & ~CFG_SL_LINK240P_OFFSET)


// some max values
#define CFG_QUICKCHANGE_MAX_VALUE  3
#define CFG_GAMMA_MAX_VALUE        8
#define CFG_SLSTR_MAX_VALUE       15
#define CFG_SLHYBDEP_MAX_VALUE    24
#define CFG_SL_ID_MAX_VALUE        3
#define CFG_VFORMAT_MAX_VALUE      2
#define CFG_DEBLUR_MAX_VALUE       2
#define CFG_FILTER_MAX_VALUE       3

// some default values
#define CFG_GAMMA_DEFAULTVAL      5
#define CFG_GAMMA_DEFAULT_SETMASK (CFG_GAMMA_DEFAULTVAL<<CFG_GAMMA_OFFSET)


// now the N64 default
#define N64_MISC_CLR_MASK       (CFG_USEIGR_CLRMASK      & CFG_QUDEBLUR_CLRMASK & \
                                 CFG_QU15BITMODE_CLRMASK )
#define N64_VIDEO_CLR_MASK      (CFG_GAMMA_CLRMASK       & CFG_DEBLUR_CLRMASK   & \
                                 CFG_15BITMODE_CLRMASK   )
#define N64_IMAGE240P_CLR_MASK  (CFG_LINEX2_CLRMASK)
#define N64_IMAGE480I_CLR_MASK  (CFG_LINEX2_CLRMASK)

#define N64_DEFAULT_VIDEO_CFG   (CFG_GAMMA_DEFAULT_SETMASK | CFG_DEBLUR_OFF_SETMASK)


#define RWM_H_OFFSET 5
#define RWM_V_OFFSET (VD_HEIGHT - 3)
#define RWM_LENGTH   10
#define RWM_SHOW_CNT 256



inline void cfg_toggle_flag(config_t* cfg_data)
  {  if (cfg_data->cfg_type == FLAG) cfg_data->cfg_word->cfg_word_val ^= cfg_data->flag_masks.setflag_mask;  };
inline void cfg_set_flag(config_t* cfg_data)
  {  if (cfg_data->cfg_type == FLAG) cfg_data->cfg_word->cfg_word_val |= cfg_data->flag_masks.setflag_mask;  };
inline void cfg_clear_flag(config_t* cfg_data)
  {  if (cfg_data->cfg_type == FLAG) cfg_data->cfg_word->cfg_word_val &= cfg_data->flag_masks.clrflag_mask;  };
void cfg_inc_value(config_t* cfg_data);
void cfg_dec_value(config_t* cfg_data);
alt_u16 cfg_get_value(config_t* cfg_data,alt_u16 get_reference);
void cfg_set_value(config_t* cfg_data, alt_u16 value);
int cfg_save_to_flash(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_from_flash(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_n64defaults(configuration_t* sysconfig, alt_u8 need_confirm);
void cfg_apply_to_logic(configuration_t* sysconfig);
void cfg_read_from_logic(configuration_t* sysconfig);
void cfg_clear_words(configuration_t* sysconfig);
void cfg_update_reference(configuration_t* sysconfig);

#endif /* CONFIG_H_ */
