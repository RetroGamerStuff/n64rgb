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
 * cfg_io_p.h
 *
 *  Created on: 17.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"

#include "config.h"
#include "menu.h"


#ifndef CFG_HEADER_CFG_IO_P_H_
#define CFG_HEADER_CFG_IO_P_H_

extern char szText[VD_WIDTH];

extern const char *OffOn[], *LineX_240p[], *AdvSL[], *LinkSL[], *EvenOdd[],
                  *VideoFormat[], *QuickChange[], *FilterAddOn[];

// misc
cfg_b32word_t cfg_data_misc =
  { .cfg_word_mask    = CFG_MISC_GETALL_MASK,
    .cfg_word_val     = 0x00000000,
    .cfg_ref_word_val = 0x00000000
  };

config_t use_vpll = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_USE_VPLL_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_USE_VPLL_SETMASK,
        .clrflag_mask = CFG_USE_VPLL_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t test_vpll = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_TEST_VPLL_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_TEST_VPLL_SETMASK,
        .clrflag_mask = CFG_TEST_VPLL_CLRMASK
    }
};

config_t show_testpat = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_SHOW_TESTPAT_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_SHOW_TESTPAT_SETMASK,
        .clrflag_mask = CFG_SHOW_TESTPAT_CLRMASK
    }
};

config_t show_logo = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_SHOWLOGO_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_SHOWLOGO_SETMASK,
        .clrflag_mask = CFG_SHOWLOGO_CLRMASK
    }
};

config_t show_osd = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_SHOWOSD_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_SHOWOSD_SETMASK,
        .clrflag_mask = CFG_SHOWOSD_CLRMASK
    }
};

config_t mute_osd_tmp = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_MUTEOSDTMP_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_MUTEOSDTMP_SETMASK,
        .clrflag_mask = CFG_MUTEOSDTMP_CLRMASK
    }
};

config_t igr_reset = {
    .cfg_word = &cfg_data_misc,
    .cfg_word_offset = CFG_IGRRST_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_IGRRST_SETMASK,
        .clrflag_mask = CFG_IGRRST_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t igr_deblur = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_IGRDEBLUR_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_IGRDEBLUR_SETMASK,
        .clrflag_mask = CFG_IGRDEBLUR_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t igr_15bitmode = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_IGR16BITMODE_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_IGR16BITMODE_SETMASK,
        .clrflag_mask = CFG_IGR16BITMODE_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t pal_awareness = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_PALAWARENESS_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_PALAWARENESS_SETMASK,
        .clrflag_mask = CFG_PALAWARENESS_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t exchange_rb_out = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_EXC_RB_OUT_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_EXC_RB_OUT_SETMASK,
        .clrflag_mask = CFG_EXC_RB_OUT_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t filteraddon_cutoff = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_FILTERADDON_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_FILTER_MAX_VALUE,
        .getvalue_mask = CFG_FILTERADDON_GETMASK,
    },
    .value_string = &FilterAddOn
};

// video

config_tray_t timing_words[5] = {
  { .config_val = CFG_TIMING_DEFAULTS, .config_ref_val = CFG_TIMING_DEFAULTS},
  { .config_val = CFG_TIMING_DEFAULTS, .config_ref_val = CFG_TIMING_DEFAULTS},
  { .config_val = CFG_TIMING_DEFAULTS, .config_ref_val = CFG_TIMING_DEFAULTS},
  { .config_val = CFG_TIMING_DEFAULTS, .config_ref_val = CFG_TIMING_DEFAULTS},
  { .config_val = CFG_TIMING_DEFAULTS, .config_ref_val = CFG_TIMING_DEFAULTS}
};


cfg_b32word_t cfg_data_video =
  { .cfg_word_mask    = CFG_VIDEO_GETALL_MASK,
    .cfg_word_val     = 0x00000000,
    .cfg_ref_word_val = 0x00000000
  };

config_t pal_dejitter = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_PALDEJ_OFFSET,
    .cfg_type        = FLAGTXT,
    .flag_masks      = {
        .setflag_mask = CFG_PAL_DEJITTER_SETMASK,
        .clrflag_mask = CFG_PAL_DEJITTER_CLRMASK
    },
    .val2char_func = &flag2set_func
};

config_t hor_shift = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_HORSHIFT_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_HORSHIFT_MAX_VALUE,
        .getvalue_mask = CFG_HORSHIFT_GETMASK
    },
    .val2char_func = &val2txt_7b_binaryoffset_half_func
};

config_t vert_shift = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_VERTSHIFT_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_VERTSHIFT_MAX_VALUE,
        .getvalue_mask = CFG_VERTSHIFT_GETMASK
    },
    .val2char_func = &val2txt_6b_binaryoffset_func
};

config_t vformat = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_VFORMAT_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_VFORMAT_MAX_VALUE,
        .getvalue_mask = CFG_VFORMAT_GETMASK
    },
    .value_string = &VideoFormat
};

config_t gamma_lut = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_GAMMA_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_GAMMA_MAX_VALUE,
        .getvalue_mask = CFG_GAMMA_GETMASK
    },
    .val2char_func = &gamma2txt_func
};

config_t deblur_mode = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_MODE_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_DEBLUR_MODE_SETMASK,
        .clrflag_mask = CFG_DEBLUR_MODE_CLRMASK
    },
    .value_string = &OffOn
};

config_t mode16bit = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_16BITMODE_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_16BITMODE_SETMASK,
        .clrflag_mask = CFG_16BITMODE_CLRMASK
    },
    .value_string = &OffOn
};


// image 240p and 480i

cfg_b32word_t cfg_data_linex =
  { .cfg_word_mask    = CFG_LINEX_GETALL_MASK,
    .cfg_word_val     = 0x00000000,
    .cfg_ref_word_val = 0x00000000
  };

config_tray_t linex_words[2] = {
  { .config_val = 0x00000000, .config_ref_val = 0x00000000},
  { .config_val = 0x00000000, .config_ref_val = 0x00000000}
};



config_t linex_240p = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_240P_LINEX_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_240P_LINEX_MAX_VALUE,
        .getvalue_mask = CFG_240P_LINEX_GETMASK
    },
    .value_string = &LineX_240p
};

config_t slhyb_str = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_240P_SLHYBDEP_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLHYBDEP_MAX_VALUE,
        .getvalue_mask = CFG_240P_SLHYBDEP_GETMASK
    },
    .val2char_func = &scanline_hybrstr2txt_func
};

config_t sl_str = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_240P_SLSTR_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLSTR_MAX_VALUE,
        .getvalue_mask = CFG_240P_SLSTR_GETMASK
    },
    .val2char_func = &scanline_str2txt_func
};

config_t sl_method = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_240P_SL_METHOD_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_240P_SL_METHOD_SETMASK,
        .clrflag_mask = CFG_240P_SL_METHOD_CLRMASK
    },
    .value_string = &AdvSL
};

config_t sl_id = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_240P_SL_ID_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_240P_SL_ID_SETMASK,
        .clrflag_mask = CFG_240P_SL_ID_CLRMASK
    },
    .value_string = &EvenOdd
};

config_t sl_en = {
    .cfg_word = &cfg_data_linex,
    .cfg_word_offset = CFG_240P_SL_EN_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_240P_SL_EN_SETMASK,
        .clrflag_mask = CFG_240P_SL_EN_CLRMASK
    },
    .value_string = &OffOn
};

config_t bob_deinter_480i = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_BOB_DEINTER_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_BOB_DEINTER_SETMASK,
        .clrflag_mask = CFG_480I_BOB_DEINTER_CLRMASK
    },
    .value_string = &OffOn
};

config_t field_shift_fix_480i = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_FIELDFIX_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_FIELDFIX_SETMASK,
        .clrflag_mask = CFG_480I_FIELDFIX_CLRMASK
    },
    .value_string = &OffOn
};

config_t slhyb_str_480i = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_SLHYBDEP_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLHYBDEP_MAX_VALUE,
        .getvalue_mask = CFG_480I_SLHYBDEP_GETMASK
    },
    .val2char_func = &scanline_hybrstr2txt_func
};

config_t sl_str_480i = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_SLSTR_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLSTR_MAX_VALUE,
        .getvalue_mask = CFG_480I_SLSTR_GETMASK
    },
    .val2char_func = &scanline_str2txt_func
};

config_t sl_link_480i = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_SL_LINK240P_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_480I_SL_LINK240P_SETMASK,
        .clrflag_mask = CFG_480I_SL_LINK240P_CLRMASK
    },
    .value_string = &LinkSL
};

config_t sl_id_480i = {
    .cfg_word        = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_SL_ID_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_480I_SL_ID_SETMASK,
        .clrflag_mask = CFG_480I_SL_ID_CLRMASK
    },
    .value_string = &EvenOdd
};

config_t sl_en_480i = {
    .cfg_word = &cfg_data_linex,
    .cfg_word_offset = CFG_480I_SL_EN_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_SL_EN_SETMASK,
        .clrflag_mask = CFG_480I_SL_EN_CLRMASK
    },
    .value_string = &OffOn
};


#endif /* CFG_HEADER_CFG_IO_P_H_ */
