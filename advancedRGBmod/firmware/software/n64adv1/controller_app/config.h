/*********************************************************************************
 *
 * This file is part of the N64 RGB/YPbPr DAC project.
 *
 * Copyright (C) 2015-2019 by Peter Bartmann <borti4938@gmail.com>
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

typedef alt_u8 manage_vpll;

#define VPLL_TEST_FAILED       10
#define CFG_VERSION_INVALID   100
#define CFG_FLASH_NOT_USED    101
#define CFG_FLASH_SAVE_ABORT  102
#define CFG_FLASH_LOAD_ABORT  CFG_FLASH_SAVE_ABORT
#define CFG_N64DEF_LOAD_ABORT CFG_FLASH_SAVE_ABORT
#define CFG_JUMPER_LOAD_ABORT CFG_FLASH_SAVE_ABORT

// the overall masks

#define CFG_MISC_GETALL_MASK          0x1F07
#define CFG_VIDEO_GETALL_MASK         0x9FF7
#define CFG_IMAGE240P_GETALL_MASK     0x7FF7
#define CFG_IMAGE480I_GETALL_MASK     0x7FF7

// misc
#define CFG_USE_VPLL_OFFSET     12
#define CFG_TEST_VPLL_OFFSET    11
#define CFG_SHOWLOGO_OFFSET     10
#define CFG_SHOWOSD_OFFSET       9
#define CFG_MUTEOSDTMP_OFFSET    8
#define CFG_USEIGR_OFFSET        2
#define CFG_QUICKCHANGE_OFFSET   0
  #define CFG_QU15BITMODE_OFFSET   1
  #define CFG_QUDEBLUR_OFFSET      0

#define CFG_USE_VPLL_GETMASK       (1<<CFG_USE_VPLL_OFFSET)
  #define CFG_USE_VPLL_SETMASK       (1<<CFG_USE_VPLL_OFFSET)
  #define CFG_USE_VPLL_CLRMASK       (CFG_MISC_GETALL_MASK & ~CFG_USE_VPLL_SETMASK)
#define CFG_TEST_VPLL_GETMASK       (1<<CFG_TEST_VPLL_OFFSET)
  #define CFG_TEST_VPLL_SETMASK       (1<<CFG_TEST_VPLL_OFFSET)
  #define CFG_TEST_VPLL_CLRMASK       (CFG_MISC_GETALL_MASK & ~CFG_TEST_VPLL_SETMASK)
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

#define CFG_SHOW_TESTPAT_OFFSET 15
#define CFG_FILTERADDON_OFFSET  10
#define CFG_VFORMAT_OFFSET       8
  #define CFG_YPBPR_OFFSET         9
  #define CFG_RGSB_OFFSET          8
#define CFG_GAMMA_OFFSET         4
#define CFG_DEBLUR_OFFSET        1
#define CFG_15BITMODE_OFFSET     0

#define CFG_SHOW_TESTPAT_GETMASK  (1<<CFG_SHOW_TESTPAT_OFFSET)
  #define CFG_SHOW_TESTPAT_SETMASK  (1<<CFG_SHOW_TESTPAT_OFFSET)
  #define CFG_SHOW_TESTPAT_CLRMASK  (CFG_VIDEO_GETALL_MASK & ~CFG_SHOW_TESTPAT_SETMASK)
#define CFG_FILTERADDON_GETMASK   (7<<CFG_FILTERADDON_OFFSET)
  #define CFG_FILTER_RSTMASK        (CFG_VIDEO_GETALL_MASK & ~CFG_FILTERADDON_GETMASK)
  #define CFG_FILTER_OFF_SETMASK    (CFG_VIDEO_GETALL_MASK & (4<<CFG_FILTERADDON_OFFSET))
  #define CFG_FILTER_AUTO_SETMASK   (CFG_VIDEO_GETALL_MASK & (0<<CFG_FILTERADDON_OFFSET))
#define CFG_FILTERADDON_CLRMASK   (CFG_VIDEO_GETALL_MASK & ~CFG_FILTERADDON_GETMASK)
#define CFG_VFORMAT_GETMASK       (3<<CFG_VFORMAT_OFFSET)
  #define CFG_VFORMAT_RSTMASK       (CFG_VIDEO_GETALL_MASK & ~CFG_VFORMAT_GETMASK)
  #define CFG_VFORMAT_CLRMASK       (CFG_VIDEO_GETALL_MASK & ~CFG_VFORMAT_GETMASK)
    #define CFG_YPBPR_GETMASK         (1<<CFG_YPBPR_OFFSET)
    #define CFG_YPBPR_SETMASK         (1<<CFG_YPBPR_OFFSET)
    #define CFG_YPBPR_CLRMASK         (CFG_VIDEO_GETALL_MASK & ~CFG_YPBPR_SETMASK)
    #define CFG_RGSB_GETMASK          (1<<CFG_RGSB_OFFSET)
    #define CFG_RGSB_SETMASK          (1<<CFG_RGSB_OFFSET)
    #define CFG_RGSB_CLRMASK          (CFG_GETALL_MASK & ~CFG_RGSB_SETMASK)
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
#define CFG_240P_LINEX_OFFSET       13
#define CFG_240P_SLHYBDEP_OFFSET     8
  #define CFG_240P_SLHYBDEPMSB_OFFSET 12
  #define CFG_240P_SLHYBDEPLSB_OFFSET  8
#define CFG_240P_SLSTR_OFFSET        4
  #define CFG_240P_SLSTRMSB_OFFSET     7
  #define CFG_240P_SLSTRLSB_OFFSET     4
#define CFG_240P_SL_METHOD_OFFSET    2
#define CFG_240P_SL_ID_OFFSET        1
#define CFG_240P_SL_EN_OFFSET        0

#define CFG_240P_LINEX_GETMASK         (3<<CFG_240P_LINEX_OFFSET)
  #define CFG_240P_LINEX_RSTMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_LINEX_GETMASK)
#define CFG_240P_SLHYBDEP_GETMASK      (0x1F<<CFG_240P_SLHYBDEP_OFFSET)
  #define CFG_240P_SLHYBDEP_RSTMASK      (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SLHYBDEP_GETMASK)
  #define CFG_240P_SLHYBDEP_CLRMASK      (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SLHYBDEP_GETMASK)
#define CFG_240P_SLSTR_GETMASK         (0xF<<CFG_240P_SLSTR_OFFSET)
  #define CFG_240P_SLSTR_RSTMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SLSTR_GETMASK)
#define CFG_240P_SLSTR_CLRMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SLSTR_GETMASK)
#define CFG_240P_SL_METHOD_GETMASK     (1<<CFG_240P_SL_METHOD_OFFSET)
  #define CFG_240P_SL_METHOD_SETMASK     (1<<CFG_240P_SL_METHOD_OFFSET)
  #define CFG_240P_SL_METHOD_CLRMASK     (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SL_METHOD_SETMASK)
#define CFG_240P_SL_ID_GETMASK         (1<<CFG_240P_SL_ID_OFFSET)
  #define CFG_240P_SL_ID_SETMASK         (1<<CFG_240P_SL_ID_OFFSET)
  #define CFG_240P_SL_ID_CLRMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SL_ID_SETMASK)
#define CFG_240P_SL_EN_GETMASK         (1<<CFG_240P_SL_EN_OFFSET)
  #define CFG_240P_SL_EN_SETMASK         (1<<CFG_240P_SL_EN_OFFSET)
  #define CFG_240P_SL_EN_CLRMASK         (CFG_IMAGE240P_GETALL_MASK & ~CFG_240P_SL_EN_SETMASK)


// image for 480i
#define CFG_480I_FIELDFIX_OFFSET    14
#define CFG_480I_BOB_DEINTER_OFFSET 13
#define CFG_480I_SLHYBDEP_OFFSET     8
  #define CFG_480I_SLHYBDEPMSB_OFFSET 12
  #define CFG_480I_SLHYBDEPLSB_OFFSET  8
#define CFG_480I_SLSTR_OFFSET        4
  #define CFG_480I_SLSTRMSB_OFFSET     7
  #define CFG_480I_SLSTRLSB_OFFSET     4
#define CFG_480I_SL_LINK240P_OFFSET  2
#define CFG_480I_SL_ID_OFFSET        1
#define CFG_480I_SL_EN_OFFSET        0

#define CFG_480I_FIELDFIX_GETMASK     (1<<CFG_480I_FIELDFIX_OFFSET)
  #define CFG_480I_FIELDFIX_SETMASK     (1<<CFG_480I_FIELDFIX_OFFSET)
  #define CFG_480I_FIELDFIX_CLRMASK     (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_FIELDFIX_GETMASK)
#define CFG_480I_BOB_DEINTER_GETMASK  (1<<CFG_480I_BOB_DEINTER_OFFSET)
  #define CFG_480I_BOB_DEINTER_SETMASK  (1<<CFG_480I_BOB_DEINTER_OFFSET)
  #define CFG_480I_BOB_DEINTER_CLRMASK  (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_BOB_DEINTER_GETMASK)
#define CFG_480I_SLHYBDEP_GETMASK     (0x1F<<CFG_480I_SLHYBDEP_OFFSET)
  #define CFG_480I_SLHYBDEP_RSTMASK     (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SLHYBDEP_GETMASK)
#define CFG_480I_SLHYBDEP_CLRMASK       (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SLHYBDEP_GETMASK)
#define CFG_480I_SLSTR_GETMASK        (0xF<<CFG_480I_SLSTR_OFFSET)
  #define CFG_480I_SLSTR_RSTMASK        (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SLSTR_GETMASK)
  #define CFG_480I_SLSTR_CLRMASK        (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SLSTR_GETMASK)
#define CFG_480I_SL_LINK240P_GETMASK  (1<<CFG_480I_SL_LINK240P_OFFSET)
  #define CFG_480I_SL_LINK240P_SETMASK  (1<<CFG_480I_SL_LINK240P_OFFSET)
  #define CFG_480I_SL_LINK240P_CLRMASK  (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SL_LINK240P_GETMASK)
#define CFG_480I_SL_ID_GETMASK        (1<<CFG_480I_SL_ID_OFFSET)
  #define CFG_480I_SL_ID_SETMASK        (1<<CFG_480I_SL_ID_OFFSET)
  #define CFG_480I_SL_ID_CLRMASK        (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SL_ID_SETMASK)
#define CFG_480I_SL_EN_GETMASK        (1<<CFG_480I_SL_EN_OFFSET)
  #define CFG_480I_SL_EN_SETMASK        (1<<CFG_480I_SL_EN_OFFSET)
  #define CFG_480I_SL_EN_CLRMASK        (CFG_IMAGE480I_GETALL_MASK & ~CFG_480I_SL_EN_SETMASK)





// some max values
#define CFG_QUICKCHANGE_MAX_VALUE    3
#define CFG_GAMMA_MAX_VALUE          8
#define CFG_240P_LINEX_MAX_VALUE     2
#define CFG_SLSTR_MAX_VALUE         15
#define CFG_SLHYBDEP_MAX_VALUE      24
#define CFG_SL_ID_MAX_VALUE          3
#define CFG_VFORMAT_MAX_VALUE        2
#define CFG_DEBLUR_MAX_VALUE         2
#define CFG_FILTER_MAX_VALUE         4

#define CFG_FILTER_NOT_INSTALLED     5

// some default values
#define CFG_GAMMA_DEFAULTVAL      5
#define CFG_GAMMA_DEFAULT_SETMASK (CFG_GAMMA_DEFAULTVAL<<CFG_GAMMA_OFFSET)


// now the N64 default
#define N64_MISC_CLR_MASK       (CFG_USEIGR_CLRMASK      & CFG_QUDEBLUR_CLRMASK & \
                                 CFG_QU15BITMODE_CLRMASK )
#define N64_VIDEO_CLR_MASK      (CFG_FILTER_RSTMASK      & CFG_VFORMAT_CLRMASK  & \
                                 CFG_GAMMA_CLRMASK       & CFG_DEBLUR_CLRMASK   & \
                                 CFG_15BITMODE_CLRMASK   )
#define N64_IMAGE240P_CLR_MASK  (CFG_240P_LINEX_RSTMASK)
#define N64_IMAGE480I_CLR_MASK  (CFG_480I_BOB_DEINTER_CLRMASK)

#define N64_DEFAULT_VIDEO_CFG     (CFG_FILTER_AUTO_SETMASK | CFG_GAMMA_DEFAULT_SETMASK | \
                                   CFG_DEBLUR_OFF_SETMASK  )

// the jumper
#define JUMPER_GETALL_MASK  0x7F


#define JUMPER_FILTERADDON_OFFSET       6
#define JUMPER_LINEX2_OFFSET            5
#define JUMPER_480I_BOB_DEINTER_OFFSET  4
#define JUMPER_SLSTR_OFFSET             2
#define JUMPER_VFORMAT_OFFSET           0
  #define JUMPER_YPBPR_OFFSET             1
  #define JUMPER_RGSB_OFFSET              0

#define JUMPER_FILTERADDON_GETMASK       (1<<JUMPER_FILTERADDON_OFFSET)
#define JUMPER_LINEX2_GETMASK            (1<<JUMPER_LINEX2_OFFSET)
#define JUMPER_480I_BOB_DEINTER_GETMASK  (1<<JUMPER_480I_BOB_DEINTER_OFFSET)
#define JUMPER_SLSTR_GETMASK             (3<<JUMPER_SLSTR_OFFSET)
#define JUMPER_SOG_GETMASK               (3<<JUMPER_YPBPR_OFFSET)
  #define JUMPER_YPBPR_GETMASK           (1<<JUMPER_YPBPR_OFFSET)
  #define JUMPER_RGSB_GETMASK            (1<<JUMPER_RGSB_OFFSET)


#define VPLL_START_FRAMES     5
#define VPLL_LOCK_LOST_FRAMES 1

#define RWM_H_OFFSET 5
#define RWM_V_OFFSET (VD_HEIGHT - 3)
#define RWM_LENGTH   10
#define RWM_SHOW_CNT 256


extern alt_u8 use_filteraddon;

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
int cfg_show_testpattern(configuration_t* sysconfig);
int cfg_save_to_flash(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_from_flash(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_n64defaults(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_jumperset(configuration_t* sysconfig, alt_u8 need_confirm);
void cfg_apply_to_logic(configuration_t* sysconfig);
void cfg_read_from_logic(configuration_t* sysconfig);
inline alt_u8 cfg_get_jumper()
  {  return (IORD_ALTERA_AVALON_PIO_DATA(JUMPER_CFG_SET_IN_BASE) & JUMPER_GETALL_MASK);  };
void cfg_clear_words(configuration_t* sysconfig);
void cfg_update_reference(configuration_t* sysconfig);
inline void check_filteraddon()
  {  use_filteraddon = ((IORD_ALTERA_AVALON_PIO_DATA(JUMPER_CFG_SET_IN_BASE) & JUMPER_FILTERADDON_GETMASK) >> JUMPER_FILTERADDON_OFFSET) ? 0 : 1;  };
void enable_vpll_test(void);
void disable_vpll_test(void);
int run_vpll_test(configuration_t* sysconfig);

#endif /* CONFIG_H_ */
