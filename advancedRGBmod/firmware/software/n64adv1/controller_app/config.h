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
 * config.h
 *
 *  Created on: 11.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include <string.h>
#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"


#ifndef CONFIG_H_
#define CONFIG_H_

typedef enum {
  MISC = 0,
  VIDEO,
  LINEX,
} cfg_word_type_t;

#define NUM_CFG_B32WORDS    3

typedef enum {
  NTSC = 0,
  PAL
} cfg_linex_sel_type_t;
#define LINEX_TYPES 2


typedef enum {
  PPU_CURRENT = 0,
  NTSC_LX2_PR,
  NTSC_LX2_INT,
  NTSC_LX3_PR,
  PAL_LX2_PR,
  PAL_LX2_INT
} cfg_timing_model_sel_type_t;
#define NUM_TIMING_MODES  5

typedef enum {
  OFF = 0,
  ON
} cfg_offon_sel_type_t;

typedef struct {
  const alt_u32         cfg_word_mask;
  alt_u32               cfg_word_val;
  alt_u32               cfg_ref_word_val;
} cfg_b32word_t;

typedef struct {
	cfg_b32word_t* cfg_word_def[NUM_CFG_B32WORDS];
} configuration_t;

typedef enum {
  FLAG,
  FLAGTXT,
  TXTVALUE,
  NUMVALUE
} config_type_t;

typedef struct {
  const alt_u32 setflag_mask;
  const alt_u32 clrflag_mask;
} config_flag_t;

typedef struct {
  const alt_u32 getvalue_mask;
  const alt_u8 max_value;
} config_value_t;

typedef void (*val2char_func_call)(alt_u8);

typedef struct {
  cfg_b32word_t       *cfg_word;
  union {
    alt_u8              cfg_word_offset;
    alt_u8              cfg_value;
  };
  const config_type_t cfg_type;
  union {
    const config_flag_t  flag_masks;
    const config_value_t value_details;
  };
  union {
    const char*        *value_string;
    val2char_func_call val2char_func;
  };
} config_t;

typedef struct {
  alt_u32 config_val;
  alt_u32 config_ref_val;
} config_tray_t;

#define VPLL_TEST_FAILED       10
#define CFG_VERSION_INVALID   100
#define CFG_FLASH_NOT_USED    101
#define CFG_FLASH_SAVE_ABORT  102
#define CFG_FLASH_LOAD_ABORT  CFG_FLASH_SAVE_ABORT
#define CFG_N64DEF_LOAD_ABORT CFG_FLASH_SAVE_ABORT
#define CFG_DEF_LOAD_ABORT    CFG_FLASH_SAVE_ABORT
#define CFG_JUMPER_LOAD_ABORT CFG_FLASH_SAVE_ABORT

// the overall masks
#define CFG_MISC_GETALL_MASK      0x00003FFF
#define CFG_VIDEO_GETALL_MASK     0x017F3FFF
#define CFG_LINEX_GETALL_MASK     0x7FF77FF7


// misc  (set 2)
#define CFG_MISC_OUT_BASE   CFG_SET2_OUT_BASE

#define CFG_USE_VPLL_OFFSET     13
#define CFG_TEST_VPLL_OFFSET    12
#define CFG_SHOW_TESTPAT_OFFSET 11
#define CFG_SHOWLOGO_OFFSET     10
#define CFG_SHOWOSD_OFFSET       9
#define CFG_MUTEOSDTMP_OFFSET    8
#define CFG_IGRRST_OFFSET        7
#define CFG_IGRDEBLUR_OFFSET     6
#define CFG_IGR16BITMODE_OFFSET  5
#define CFG_PALAWARENESS_OFFSET  4
#define CFG_EXC_RB_OUT_OFFSET    3
#define CFG_FILTERADDON_OFFSET   0

#define CFG_USE_VPLL_GETMASK      (1<<CFG_USE_VPLL_OFFSET)
  #define CFG_USE_VPLL_SETMASK      (1<<CFG_USE_VPLL_OFFSET)
  #define CFG_USE_VPLL_CLRMASK      (CFG_MISC_GETALL_MASK & ~CFG_USE_VPLL_SETMASK)
#define CFG_TEST_VPLL_GETMASK     (1<<CFG_TEST_VPLL_OFFSET)
  #define CFG_TEST_VPLL_SETMASK     (1<<CFG_TEST_VPLL_OFFSET)
  #define CFG_TEST_VPLL_CLRMASK     (CFG_MISC_GETALL_MASK & ~CFG_TEST_VPLL_SETMASK)
#define CFG_SHOW_TESTPAT_GETMASK  (1<<CFG_SHOW_TESTPAT_OFFSET)
  #define CFG_SHOW_TESTPAT_SETMASK  (1<<CFG_SHOW_TESTPAT_OFFSET)
  #define CFG_SHOW_TESTPAT_CLRMASK  (CFG_VIDEO_GETALL_MASK & ~CFG_SHOW_TESTPAT_SETMASK)
#define CFG_SHOWLOGO_GETMASK      (1<<CFG_SHOWLOGO_OFFSET)
  #define CFG_SHOWLOGO_SETMASK      (1<<CFG_SHOWLOGO_OFFSET)
  #define CFG_SHOWLOGO_CLRMASK      (CFG_MISC_GETALL_MASK & ~CFG_SHOWLOGO_SETMASK)
#define CFG_SHOWOSD_GETMASK       (1<<CFG_SHOWOSD_OFFSET)
  #define CFG_SHOWOSD_SETMASK       (1<<CFG_SHOWOSD_OFFSET)
  #define CFG_SHOWOSD_CLRMASK       (CFG_MISC_GETALL_MASK & ~CFG_SHOWOSD_SETMASK)
#define CFG_MUTEOSDTMP_GETMASK    (1<<CFG_MUTEOSDTMP_OFFSET)
  #define CFG_MUTEOSDTMP_SETMASK    (1<<CFG_MUTEOSDTMP_OFFSET)
  #define CFG_MUTEOSDTMP_CLRMASK    (CFG_MISC_GETALL_MASK & ~CFG_MUTEOSDTMP_SETMASK)
#define CFG_IGRRST_GETMASK        (1<<CFG_IGRRST_OFFSET)
  #define CFG_IGRRST_SETMASK        (1<<CFG_IGRRST_OFFSET)
  #define CFG_IGRRST_CLRMASK        (CFG_MISC_GETALL_MASK & ~CFG_IGRRST_SETMASK)
#define CFG_IGRDEBLUR_GETMASK     (1<<CFG_IGRDEBLUR_OFFSET)
  #define CFG_IGRDEBLUR_SETMASK     (1<<CFG_IGRDEBLUR_OFFSET)
  #define CFG_IGRDEBLUR_CLRMASK     (CFG_MISC_GETALL_MASK & ~CFG_IGRDEBLUR_SETMASK)
#define CFG_IGR16BITMODE_GETMASK  (1<<CFG_IGR16BITMODE_OFFSET)
  #define CFG_IGR16BITMODE_SETMASK  (1<<CFG_IGR16BITMODE_OFFSET)
  #define CFG_IGR16BITMODE_CLRMASK  (CFG_MISC_GETALL_MASK & ~CFG_IGR16BITMODE_SETMASK)
#define CFG_PALAWARENESS_GETMASK  (1<<CFG_PALAWARENESS_OFFSET)
  #define CFG_PALAWARENESS_SETMASK  (1<<CFG_PALAWARENESS_OFFSET)
  #define CFG_PALAWARENESS_CLRMASK  (CFG_MISC_GETALL_MASK & ~CFG_PALAWARENESS_SETMASK)
#define CFG_EXC_V_RB_OUT_GETMASK         (1<<CFG_EXC_RB_OUT_OFFSET)
  #define CFG_EXC_RB_OUT_SETMASK         (1<<CFG_EXC_RB_OUT_OFFSET)
  #define CFG_EXC_RB_OUT_CLRMASK         (CFG_VIDEO_GETALL_MASK & ~CFG_EXC_RB_OUT_SETMASK)
#define CFG_FILTERADDON_GETMASK       (7<<CFG_FILTERADDON_OFFSET)
  #define CFG_FILTER_RSTMASK            (CFG_MISC_GETALL_MASK & ~CFG_FILTERADDON_GETMASK)
  #define CFG_FILTER_OFF_SETMASK        (CFG_MISC_GETALL_MASK & (4<<CFG_FILTERADDON_OFFSET))
  #define CFG_FILTER_AUTO_SETMASK       (CFG_MISC_GETALL_MASK & (0<<CFG_FILTERADDON_OFFSET))
  #define CFG_FILTERADDON_CLRMASK       (CFG_MISC_GETALL_MASK & ~CFG_FILTERADDON_GETMASK)


// video (set 1)
#define CFG_VIDEO_OUT_BASE  CFG_SET1_OUT_BASE

#define CFG_PALDEJ_OFFSET       24
#define CFG_HORSHIFT_OFFSET     16
#define CFG_VERTSHIFT_OFFSET     8
#define CFG_VFORMAT_OFFSET       6
  #define CFG_YPBPR_OFFSET         7
  #define CFG_RGSB_OFFSET          6
#define CFG_GAMMA_OFFSET         2
#define CFG_DEBLUR_MODE_OFFSET   1
#define CFG_16BITMODE_OFFSET     0


#define CFG_PAL_DEJITTER_GETMASK      (1<<CFG_PALDEJ_OFFSET)
  #define CFG_PAL_DEJITTER_SETMASK      (1<<CFG_PALDEJ_OFFSET)
  #define CFG_PAL_DEJITTER_CLRMASK      (CFG_VIDEO_GETALL_MASK & ~CFG_PAL_DEJITTER_GETMASK)
#define CFG_HORSHIFT_GETMASK          (0x7F<<CFG_HORSHIFT_OFFSET)
  #define CFG_HORSHIFT_RSTMASK          (CFG_VIDEO_GETALL_MASK & ~CFG_HORSHIFT_GETMASK)
  #define CFG_HORSHIFT_CLRMASK          (CFG_VIDEO_GETALL_MASK & ~CFG_HORSHIFT_GETMASK)
#define CFG_VERTSHIFT_GETMASK         (0x3F<<CFG_VERTSHIFT_OFFSET)
  #define CFG_VERTSHIFT_RSTMASK         (CFG_VIDEO_GETALL_MASK & ~CFG_VERTSHIFT_GETMASK)
  #define CFG_VERTSHIFT_CLRMASK         (CFG_VIDEO_GETALL_MASK & ~CFG_VERTSHIFT_GETMASK)
#define CFG_VFORMAT_GETMASK           (3<<CFG_VFORMAT_OFFSET)
  #define CFG_VFORMAT_RSTMASK           (CFG_VIDEO_GETALL_MASK & ~CFG_VFORMAT_GETMASK)
  #define CFG_VFORMAT_CLRMASK           (CFG_VIDEO_GETALL_MASK & ~CFG_VFORMAT_GETMASK)
    #define CFG_YPBPR_GETMASK             (1<<CFG_YPBPR_OFFSET)
    #define CFG_YPBPR_SETMASK             (1<<CFG_YPBPR_OFFSET)
    #define CFG_YPBPR_CLRMASK             (CFG_VIDEO_GETALL_MASK & ~CFG_YPBPR_SETMASK)
    #define CFG_RGSB_GETMASK              (1<<CFG_RGSB_OFFSET)
    #define CFG_RGSB_SETMASK              (1<<CFG_RGSB_OFFSET)
    #define CFG_RGSB_CLRMASK              (CFG_GETALL_MASK & ~CFG_RGSB_SETMASK)
#define CFG_GAMMA_GETMASK             (0xF<<CFG_GAMMA_OFFSET)
  #define CFG_GAMMASEL_RSTMASK          (CFG_VIDEO_GETALL_MASK & ~CFG_GAMMA_GETMASK)
  #define CFG_GAMMA_CLRMASK             (CFG_VIDEO_GETALL_MASK & ~CFG_GAMMA_GETMASK)
#define CFG_DEBLUR_MODE_GETMASK       (1<<CFG_DEBLUR_MODE_OFFSET)
  #define CFG_DEBLUR_MODE_SETMASK       (1<<CFG_DEBLUR_MODE_OFFSET)
  #define CFG_DEBLUR_MODE_CLRMASK       (CFG_VIDEO_GETALL_MASK & ~CFG_DEBLUR_MODE_GETMASK)
#define CFG_16BITMODE_GETMASK         (1<<CFG_16BITMODE_OFFSET)
  #define CFG_16BITMODE_SETMASK         (1<<CFG_16BITMODE_OFFSET)
  #define CFG_16BITMODE_CLRMASK         (CFG_VIDEO_GETALL_MASK & ~CFG_16BITMODE_SETMASK)

#define CFG_VIDEO_GETTIMINGS_MASK     (CFG_PAL_DEJITTER_GETMASK | CFG_HORSHIFT_GETMASK | CFG_VERTSHIFT_GETMASK)
#define CFG_VIDEO_GETNONTIMINGS_MASK  (CFG_VFORMAT_GETMASK | CFG_GAMMA_GETMASK | CFG_DEBLUR_MODE_GETMASK | CFG_16BITMODE_GETMASK)

// linex for 240p and 480i (set 0)
#define CFG_LINEX_OUT_BASE  CFG_SET0_OUT_BASE

#define CFG_240P_LINEX_OFFSET       29
#define CFG_240P_SLHYBDEP_OFFSET    24
  #define CFG_240P_SLHYBDEPMSB_OFFSET 28
  #define CFG_240P_SLHYBDEPLSB_OFFSET 24
#define CFG_240P_SLSTR_OFFSET       20
  #define CFG_240P_SLSTRMSB_OFFSET    23
  #define CFG_240P_SLSTRLSB_OFFSET    20
#define CFG_240P_SL_METHOD_OFFSET   18
#define CFG_240P_SL_ID_OFFSET       17
#define CFG_240P_SL_EN_OFFSET       16
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

#define CFG_240P_LINEX_GETMASK        (3<<CFG_240P_LINEX_OFFSET)
  #define CFG_240P_LINEX_RSTMASK        (CFG_LINEX_GETALL_MASK & ~CFG_240P_LINEX_GETMASK)
#define CFG_240P_SLHYBDEP_GETMASK     (0x1F<<CFG_240P_SLHYBDEP_OFFSET)
  #define CFG_240P_SLHYBDEP_RSTMASK     (CFG_LINEX_GETALL_MASK & ~CFG_240P_SLHYBDEP_GETMASK)
  #define CFG_240P_SLHYBDEP_CLRMASK     (CFG_LINEX_GETALL_MASK & ~CFG_240P_SLHYBDEP_GETMASK)
#define CFG_240P_SLSTR_GETMASK        (0xF<<CFG_240P_SLSTR_OFFSET)
  #define CFG_240P_SLSTR_RSTMASK        (CFG_LINEX_GETALL_MASK & ~CFG_240P_SLSTR_GETMASK)
#define CFG_240P_SLSTR_CLRMASK        (CFG_LINEX_GETALL_MASK & ~CFG_240P_SLSTR_GETMASK)
#define CFG_240P_SL_METHOD_GETMASK    (1<<CFG_240P_SL_METHOD_OFFSET)
  #define CFG_240P_SL_METHOD_SETMASK    (1<<CFG_240P_SL_METHOD_OFFSET)
  #define CFG_240P_SL_METHOD_CLRMASK    (CFG_LINEX_GETALL_MASK & ~CFG_240P_SL_METHOD_SETMASK)
#define CFG_240P_SL_ID_GETMASK        (1<<CFG_240P_SL_ID_OFFSET)
  #define CFG_240P_SL_ID_SETMASK        (1<<CFG_240P_SL_ID_OFFSET)
  #define CFG_240P_SL_ID_CLRMASK        (CFG_LINEX_GETALL_MASK & ~CFG_240P_SL_ID_SETMASK)
#define CFG_240P_SL_EN_GETMASK        (1<<CFG_240P_SL_EN_OFFSET)
  #define CFG_240P_SL_EN_SETMASK        (1<<CFG_240P_SL_EN_OFFSET)
  #define CFG_240P_SL_EN_CLRMASK        (CFG_LINEX_GETALL_MASK & ~CFG_240P_SL_EN_SETMASK)
#define CFG_480I_FIELDFIX_GETMASK     (1<<CFG_480I_FIELDFIX_OFFSET)
  #define CFG_480I_FIELDFIX_SETMASK     (1<<CFG_480I_FIELDFIX_OFFSET)
  #define CFG_480I_FIELDFIX_CLRMASK     (CFG_LINEX_GETALL_MASK & ~CFG_480I_FIELDFIX_GETMASK)
#define CFG_480I_PAL_DEJITTER_GETMASK (1<<CFG_480I_PAL_DEJ_OFFSET)
  #define CFG_480I_PAL_DEJITTER_SETMASK (1<<CFG_480I_PAL_DEJ_OFFSET)
  #define CFG_480I_PAL_DEJITTER_CLRMASK (CFG_LINEX_GETALL_MASK & ~CFG_480I_PAL_DEJITTER_GETMASK)
#define CFG_480I_BOB_DEINTER_GETMASK  (1<<CFG_480I_BOB_DEINTER_OFFSET)
  #define CFG_480I_BOB_DEINTER_SETMASK  (1<<CFG_480I_BOB_DEINTER_OFFSET)
  #define CFG_480I_BOB_DEINTER_CLRMASK  (CFG_LINEX_GETALL_MASK & ~CFG_480I_BOB_DEINTER_GETMASK)
#define CFG_480I_SLHYBDEP_GETMASK     (0x1F<<CFG_480I_SLHYBDEP_OFFSET)
  #define CFG_480I_SLHYBDEP_RSTMASK     (CFG_LINEX_GETALL_MASK & ~CFG_480I_SLHYBDEP_GETMASK)
#define CFG_480I_SLHYBDEP_CLRMASK       (CFG_LINEX_GETALL_MASK & ~CFG_480I_SLHYBDEP_GETMASK)
#define CFG_480I_SLSTR_GETMASK        (0xF<<CFG_480I_SLSTR_OFFSET)
  #define CFG_480I_SLSTR_RSTMASK        (CFG_LINEX_GETALL_MASK & ~CFG_480I_SLSTR_GETMASK)
  #define CFG_480I_SLSTR_CLRMASK        (CFG_LINEX_GETALL_MASK & ~CFG_480I_SLSTR_GETMASK)
#define CFG_480I_SL_LINK240P_GETMASK  (1<<CFG_480I_SL_LINK240P_OFFSET)
  #define CFG_480I_SL_LINK240P_SETMASK  (1<<CFG_480I_SL_LINK240P_OFFSET)
  #define CFG_480I_SL_LINK240P_CLRMASK  (CFG_LINEX_GETALL_MASK & ~CFG_480I_SL_LINK240P_GETMASK)
#define CFG_480I_SL_ID_GETMASK        (1<<CFG_480I_SL_ID_OFFSET)
  #define CFG_480I_SL_ID_SETMASK        (1<<CFG_480I_SL_ID_OFFSET)
  #define CFG_480I_SL_ID_CLRMASK        (CFG_LINEX_GETALL_MASK & ~CFG_480I_SL_ID_SETMASK)
#define CFG_480I_SL_EN_GETMASK        (1<<CFG_480I_SL_EN_OFFSET)
  #define CFG_480I_SL_EN_SETMASK        (1<<CFG_480I_SL_EN_OFFSET)
  #define CFG_480I_SL_EN_CLRMASK        (CFG_LINEX_GETALL_MASK & ~CFG_480I_SL_EN_SETMASK)


// max values
#define CFG_HORSHIFT_MAX_VALUE    111
#define CFG_VERTSHIFT_MAX_VALUE    63
#define CFG_FILTER_MAX_VALUE        4
#define CFG_GAMMA_MAX_VALUE         8
#define CFG_240P_LINEX_MAX_VALUE    2
#define CFG_SLSTR_MAX_VALUE        15
#define CFG_SLHYBDEP_MAX_VALUE     24
#define CFG_SL_ID_MAX_VALUE         3
#define CFG_VFORMAT_MAX_VALUE       2

#define CFG_FILTER_NOT_INSTALLED    5

// some default values other than 0 (go into default value of config)
// these are N64 defaults
#define CFG_VFORMAT_DEFAULTVAL                (CFG_RGSB_SETMASK >> CFG_VFORMAT_OFFSET)
  #define CFG_VFORMAT_DEFAULT_SETMASK           (CFG_VFORMAT_DEFAULTVAL << CFG_VFORMAT_OFFSET)
#define CFG_GAMMA_DEFAULTVAL                  5
  #define CFG_GAMMA_DEFAULT_SETMASK             (CFG_GAMMA_DEFAULTVAL << CFG_GAMMA_OFFSET)
#define CFG_240P_SL_METHOD_DEFAULTVAL         1
  #define CFG_240P_SL_METHOD_DEFAULT_SETMASK    (CFG_240P_SL_METHOD_DEFAULTVAL << CFG_240P_SL_METHOD_OFFSET)
#define CFG_480I_SL_LINK240P_DEFAULTVAL       1
  #define CFG_480I_SL_LINK240P_DEFAULT_SETMASK  (CFG_480I_SL_LINK240P_DEFAULTVAL << CFG_480I_SL_LINK240P_OFFSET)

#define CFG_MISC_DEFAULT          CFG_IGRRST_SETMASK
  #define CFG_MISC_GET_NODEFAULTS   (CFG_SHOWLOGO_GETMASK | CFG_SHOWOSD_GETMASK | CFG_EXC_RB_OUT_SETMASK)
#define CFG_VIDEO_DEFAULT         (CFG_VFORMAT_DEFAULT_SETMASK | CFG_GAMMA_DEFAULT_SETMASK)
  #define CFG_VIDEO_GET_NODEFAULTS  (CFG_VIDEO_GETTIMINGS_MASK | CFG_VFORMAT_GETMASK)
#define CFG_LINEX_DEFAULT         (CFG_240P_SL_METHOD_DEFAULT_SETMASK | CFG_480I_FIELDFIX_SETMASK | CFG_480I_SL_LINK240P_DEFAULT_SETMASK)
  #define CFG_LINEX_GET_NODEFAULTS  0x0000

#define CFG_TIMING_DEFAULTS 0x00402000


// the jumper
#define JUMPER_GETALL_MASK  0xFF

#define JUMPER_FILTERADDON_OFFSET       7
#define JUMPER_BYPASS_FILTER_OFFSET     6
#define JUMPER_LINEX2_OFFSET            5
#define JUMPER_480I_BOB_DEINTER_OFFSET  4
#define JUMPER_SLSTR_OFFSET             2
#define JUMPER_VFORMAT_OFFSET           0
  #define JUMPER_YPBPR_OFFSET             1
  #define JUMPER_RGSB_OFFSET              0

#define JUMPER_FILTERADDON_GETMASK       (1<<JUMPER_FILTERADDON_OFFSET)
#define JUMPER_BYPASS_FILTER_GETMASK     (1<<JUMPER_BYPASS_FILTER_OFFSET)
#define JUMPER_LINEX2_GETMASK            (1<<JUMPER_LINEX2_OFFSET)
#define JUMPER_480I_BOB_DEINTER_GETMASK  (1<<JUMPER_480I_BOB_DEINTER_OFFSET)
#define JUMPER_SLSTR_GETMASK             (3<<JUMPER_SLSTR_OFFSET)
#define JUMPER_SOG_GETMASK               (3<<JUMPER_VFORMAT_OFFSET)
  #define JUMPER_YPBPR_GETMASK           (1<<JUMPER_YPBPR_OFFSET)
  #define JUMPER_RGSB_GETMASK            (1<<JUMPER_RGSB_OFFSET)


#define RWM_H_OFFSET 5
#define RWM_V_OFFSET (VD_HEIGHT - 3)
#define RWM_LENGTH   10
#define RWM_SHOW_CNT 256


extern alt_u8 use_filteraddon;

static inline alt_u8 is_local_cfg(config_t* cfg_data)
  { return cfg_data->cfg_word == NULL;  }

void cfg_toggle_flag(config_t* cfg_data);
void cfg_set_flag(config_t* cfg_data);
void cfg_clear_flag(config_t* cfg_data);
void cfg_inc_value(config_t* cfg_data);
void cfg_dec_value(config_t* cfg_data);
alt_u8 cfg_get_value(config_t* cfg_data,alt_u8 get_reference);
void cfg_set_value(config_t* cfg_data, alt_u8 value);
int cfg_show_testpattern(configuration_t* sysconfig);
int cfg_save_to_flash(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_from_flash(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_reset_timing(configuration_t* sysconfig);
int cfg_load_defaults(configuration_t* sysconfig, alt_u8 need_confirm);
int cfg_load_jumperset(configuration_t* sysconfig, alt_u8 need_confirm);
void cfg_store_timing_word(configuration_t* sysconfig, alt_u8 timing_selection);
void cfg_load_timing_word(configuration_t* sysconfig, alt_u8 timing_selection);
void cfg_store_linex_word(configuration_t* sysconfig, alt_u8 palmode);
void cfg_load_linex_word(configuration_t* sysconfig, alt_u8 palmode);
void cfg_apply_to_logic(configuration_t* sysconfig);
void cfg_read_from_logic(configuration_t* sysconfig);
alt_u8 cfg_get_jumper();
void cfg_clear_words(configuration_t* sysconfig);
void cfg_update_reference(configuration_t* sysconfig);
void check_filteraddon();

#endif /* CONFIG_H_ */
