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
 * cfg_header/cfg_io_p.h
 *
 *  Created on: 17.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"

#include "config.h"
#include "vd_driver.h"


#ifndef CFG_HEADER_CFG_IO_P_H_
#define CFG_HEADER_CFG_IO_P_H_

extern char szText[VD_WIDTH];

extern const char *OffOn[], *LineX_240p[], *AdvSL[], *LinkSL[], *EvenOdd[],
                  *VideoFormat[], *DeBlurMode[], *DeBlurPixSens[], *DeBlurRst[],
                  *QuickChange[], *FilterAddOn[];

static void val2txt_func(alt_u8 v) { sprintf(szText,"%u", v); };
static void val_plus1_2txt_func(alt_u8 v) { sprintf(szText,"%u", v + 1); };
static void val_plus2_2txt_func(alt_u8 v) { sprintf(szText,"%u", v + 2); };
static void val_plus6_2txt_func(alt_u8 v) { sprintf(szText,"%u", v + 6); };
static void scanline_str2txt_func(alt_u8 v) { v++; sprintf(szText,"%3u.%02u%%", (v*625)/100, 25*(v&3)); };
static void scanline_hybrstr2txt_func(alt_u8 v) { sprintf(szText,"%3u.%02u%%", (v*625)/100, 25*(v&3)); };
static void gamma2txt_func(alt_u8 v) { sprintf(szText,"%u.%02u", v > 4, 5* v + 75 - (100 * (v > 4))); };


// misc
cfg_b32word_t cfg_data_misc =
  { .cfg_word_mask    = CFG_MISC_GETALL_MASK,
    .cfg_word_val     = 0x00000000,
    .cfg_ref_word_val = 0x00000000
  };

config_t use_vpll = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_USE_VPLL_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_USE_VPLL_SETMASK,
        .clrflag_mask = CFG_USE_VPLL_CLRMASK
    },
    .value_string = &OffOn
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
    .cfg_word_offset = CFG_USEIGR_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_USEIGR_SETMASK,
        .clrflag_mask = CFG_USEIGR_CLRMASK
    },
    .value_string = &OffOn
};

config_t igr_quickchange = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_QUICKCHANGE_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_QUICKCHANGE_MAX_VALUE,
        .getvalue_mask = CFG_QUICKCHANGE_GETMASK,
    },
    .value_string = &QuickChange
};


// video
cfg_b32word_t cfg_data_video =
  { .cfg_word_mask    = CFG_VIDEO_GETALL_MASK,
    .cfg_word_val     = 0x00000000,
    .cfg_ref_word_val = 0x00000000
  };

config_t exchange_rb_out = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_EXC_RB_OUT_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_EXC_RB_OUT_SETMASK,
        .clrflag_mask = CFG_EXC_RB_OUT_CLRMASK
    },
    .value_string = &OffOn
};

config_t filteraddon_cutoff = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_FILTERADDON_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_FILTER_MAX_VALUE,
        .getvalue_mask = CFG_FILTERADDON_GETMASK,
    },
    .value_string = &FilterAddOn
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

config_t mode15bit = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_15BITMODE_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_15BITMODE_SETMASK,
        .clrflag_mask = CFG_15BITMODE_CLRMASK
    },
    .value_string = &OffOn
};

config_t pal_awareness = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_PALAWARENESS_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_PALAWARENESS_SETMASK,
        .clrflag_mask = CFG_PALAWARENESS_CLRMASK
    },
    .value_string = &OffOn
};

config_t deblur_p2p_sense = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_P2PS_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_DEBLUR_P2PS_SETMASK,
        .clrflag_mask = CFG_DEBLUR_P2PS_CLRMASK
    },
    .value_string = &DeBlurPixSens
};

config_t deblur_frame_cnt_high = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_FCNT_HIGH_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_DEBLUR_FCNT_HIGH_MAX_VALUE,
        .getvalue_mask = CFG_DEBLUR_FCNT_HIGH_GETMASK,
    },
    .val2char_func = &val_plus2_2txt_func
};

config_t deblur_frame_cnt_low = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_FCNT_LOW_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_DEBLUR_FCNT_LOW_MAX_VALUE,
        .getvalue_mask = CFG_DEBLUR_FCNT_LOW_GETMASK,
    },
    .val2char_func = &val_plus1_2txt_func
};

config_t deblur_stability = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_STAB_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_DEBLUR_STAB_MAX_VALUE,
        .getvalue_mask = CFG_DEBLUR_STAB_GETMASK,
    },
    .val2char_func = &val_plus6_2txt_func
};

config_t deblur_rst_behavior = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_RST_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_DEBLUR_RST_MAX_VALUE,
        .getvalue_mask = CFG_DEBLUR_RST_GETMASK,
    },
    .value_string = &DeBlurRst
};

config_t deblur_mode = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_MODE_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_DEBLUR_MODE_MAX_VALUE,
        .getvalue_mask = CFG_DEBLUR_MODE_GETMASK,
    },
    .value_string = &DeBlurMode
};


// image 240p and 480i

cfg_b32word_t cfg_data_image =
  { .cfg_word_mask    = CFG_IMAGE_GETALL_MASK,
    .cfg_word_val     = 0x00000000,
    .cfg_ref_word_val = 0x00000000
  };

alt_u32 cfg_data_image_ntsc_word_val_tray = 0x00000000;
alt_u32 cfg_data_image_ntsc_word_ref_tray = 0x00000000;

alt_u32 cfg_data_image_pal_word_val_tray = 0x00000000;
alt_u32 cfg_data_image_pal_word_ref_tray = 0x00000000;

config_t linex_240p = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_240P_LINEX_OFFSET,
    .cfg_type        = TXTVALUE,
    .value_details   = {
        .max_value     = CFG_240P_LINEX_MAX_VALUE,
        .getvalue_mask = CFG_240P_LINEX_GETMASK
    },
    .value_string = &LineX_240p
};

config_t slhyb_str = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_240P_SLHYBDEP_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLHYBDEP_MAX_VALUE,
        .getvalue_mask = CFG_240P_SLHYBDEP_GETMASK
    },
    .val2char_func = &scanline_hybrstr2txt_func
};

config_t sl_str = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_240P_SLSTR_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLSTR_MAX_VALUE,
        .getvalue_mask = CFG_240P_SLSTR_GETMASK
    },
    .val2char_func = &scanline_str2txt_func
};

config_t sl_method = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_240P_SL_METHOD_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_240P_SL_METHOD_SETMASK,
        .clrflag_mask = CFG_240P_SL_METHOD_CLRMASK
    },
    .value_string = &AdvSL
};

config_t sl_id = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_240P_SL_ID_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_240P_SL_ID_SETMASK,
        .clrflag_mask = CFG_240P_SL_ID_CLRMASK
    },
    .value_string = &EvenOdd
};

config_t sl_en = {
    .cfg_word = &cfg_data_image,
    .cfg_word_offset = CFG_240P_SL_EN_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_240P_SL_EN_SETMASK,
        .clrflag_mask = CFG_240P_SL_EN_CLRMASK
    },
    .value_string = &OffOn
};

config_t bob_deinter_480i = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_480I_BOB_DEINTER_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_BOB_DEINTER_SETMASK,
        .clrflag_mask = CFG_480I_BOB_DEINTER_CLRMASK
    },
    .value_string = &OffOn
};

config_t field_shift_fix_480i = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_480I_FIELDFIX_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_FIELDFIX_SETMASK,
        .clrflag_mask = CFG_480I_FIELDFIX_CLRMASK
    },
    .value_string = &OffOn
};

config_t slhyb_str_480i = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_480I_SLHYBDEP_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLHYBDEP_MAX_VALUE,
        .getvalue_mask = CFG_480I_SLHYBDEP_GETMASK
    },
    .val2char_func = &scanline_hybrstr2txt_func
};

config_t sl_str_480i = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_480I_SLSTR_OFFSET,
    .cfg_type        = NUMVALUE,
    .value_details   = {
        .max_value     = CFG_SLSTR_MAX_VALUE,
        .getvalue_mask = CFG_480I_SLSTR_GETMASK
    },
    .val2char_func = &scanline_str2txt_func
};

config_t sl_link_480i = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_480I_SL_LINK240P_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_480I_SL_LINK240P_SETMASK,
        .clrflag_mask = CFG_480I_SL_LINK240P_CLRMASK
    },
    .value_string = &LinkSL
};

config_t sl_id_480i = {
    .cfg_word        = &cfg_data_image,
    .cfg_word_offset = CFG_480I_SL_ID_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_480I_SL_ID_SETMASK,
        .clrflag_mask = CFG_480I_SL_ID_CLRMASK
    },
    .value_string = &EvenOdd
};

config_t sl_en_480i = {
    .cfg_word = &cfg_data_image,
    .cfg_word_offset = CFG_480I_SL_EN_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_SL_EN_SETMASK,
        .clrflag_mask = CFG_480I_SL_EN_CLRMASK
    },
    .value_string = &OffOn
};


#endif /* CFG_HEADER_CFG_IO_P_H_ */
