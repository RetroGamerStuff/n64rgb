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


#ifndef CFG_HEADER_CFG_IO_P_H_
#define CFG_HEADER_CFG_IO_P_H_

extern const char *OffOn[], *LineX2_480i[], *AdvSL[], *LinkSL[], *EvenOdd[],
                  *VideoFormat[], *DeBlurCfg[], *SLDesc[], *GammaValue[],
                  *QuickChange[], *FilterAddOn[];

// misc
cfg_word_t cfg_data_misc =
  { .cfg_word_mask    = CFG_MISC_GETALL_MASK,
    .cfg_word_val     = 0x00,
    .cfg_ref_word_val = 0x00
  };

config_t show_sl_in_osd = {
    .cfg_word        = &cfg_data_misc,
    .cfg_word_offset = CFG_SLOSD_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_SLOSD_SETMASK,
        .clrflag_mask = CFG_SLOSD_CLRMASK
    },
    .value_string = &OffOn
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
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_QUICKCHANGE_MAX_VALUE,
        .getvalue_mask = CFG_QUICKCHANGE_GETMASK,
    },
    .value_string = &QuickChange
};


// video
cfg_word_t cfg_data_video =
  { .cfg_word_mask    = CFG_VIDEO_GETALL_MASK,
    .cfg_word_val     = 0x00,
    .cfg_ref_word_val = 0x00
  };

config_t show_testpat = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_SHOW_TESTPAT_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_SHOW_TESTPAT_SETMASK,
        .clrflag_mask = CFG_SHOW_TESTPAT_CLRMASK
    }
};

config_t filteraddon_cutoff = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_FILTERADDON_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_FILTER_MAX_VALUE,
        .getvalue_mask = CFG_FILTERADDON_GETMASK,
    },
    .value_string = &FilterAddOn
};

config_t vformat = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_VFORMAT_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_VFORMAT_MAX_VALUE,
        .getvalue_mask = CFG_VFORMAT_GETMASK
    },
    .value_string = &VideoFormat
};

config_t gamma_lut = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_GAMMA_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_GAMMA_MAX_VALUE,
        .getvalue_mask = CFG_GAMMA_GETMASK
    },
    .value_string = &GammaValue
};

config_t deblur = {
    .cfg_word        = &cfg_data_video,
    .cfg_word_offset = CFG_DEBLUR_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_DEBLUR_MAX_VALUE,
        .getvalue_mask = CFG_DEBLUR_GETMASK,
    },
    .value_string = &DeBlurCfg
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


// image 240p
cfg_word_t cfg_data_image240p =
  { .cfg_word_mask    = CFG_IMAGE240P_GETALL_MASK,
    .cfg_word_val     = 0x00,
    .cfg_ref_word_val = 0x00
  };

config_t linex2 = {
    .cfg_word        = &cfg_data_image240p,
    .cfg_word_offset = CFG_240P_LINEX2_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_240P_LINEX2_SETMASK,
        .clrflag_mask = CFG_240P_LINEX2_CLRMASK
    },
    .value_string = &OffOn
};

config_t slhyb_str = {
    .cfg_word        = &cfg_data_image240p,
    .cfg_word_offset = CFG_240P_SLHYBDEP_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_SLHYBDEP_MAX_VALUE,
        .getvalue_mask = CFG_240P_SLHYBDEP_GETMASK
    },
    .value_string = &SLDesc
};

config_t sl_str = {
    .cfg_word        = &cfg_data_image240p,
    .cfg_word_offset = CFG_240P_SLSTR_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_SLSTR_MAX_VALUE,
        .getvalue_mask = CFG_240P_SLSTR_GETMASK
    },
    .value_string = &SLDesc
};

config_t sl_method = {
    .cfg_word        = &cfg_data_image240p,
    .cfg_word_offset = CFG_240P_SL_METHOD_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_240P_SL_METHOD_SETMASK,
        .clrflag_mask = CFG_240P_SL_METHOD_CLRMASK
    },
    .value_string = &AdvSL
};

config_t sl_id = {
    .cfg_word        = &cfg_data_image240p,
    .cfg_word_offset = CFG_240P_SL_ID_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_240P_SL_ID_SETMASK,
        .clrflag_mask = CFG_240P_SL_ID_CLRMASK
    },
    .value_string = &EvenOdd
};

config_t sl_en = {
    .cfg_word = &cfg_data_image240p,
    .cfg_word_offset = CFG_240P_SL_EN_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_240P_SL_EN_SETMASK,
        .clrflag_mask = CFG_240P_SL_EN_CLRMASK
    },
    .value_string = &OffOn
};


// image 480i
cfg_word_t cfg_data_image480i =
  { .cfg_word_mask    = CFG_IMAGE480I_GETALL_MASK,
    .cfg_word_val     = 0x00,
    .cfg_ref_word_val = 0x00
  };

config_t linex2_480i = {
    .cfg_word        = &cfg_data_image480i,
    .cfg_word_offset = CFG_480I_LINEX2_OFFSET,
    .cfg_type        = VALUE,
	.value_details   = {
        .max_value     = CFG_480I_LINEX2_MAX_VALUE,
        .getvalue_mask = CFG_480I_LINEX2_GETMASK
    },
    .value_string = &LineX2_480i
};

config_t slhyb_str_480i = {
    .cfg_word        = &cfg_data_image480i,
    .cfg_word_offset = CFG_480I_SLHYBDEP_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_SLHYBDEP_MAX_VALUE,
        .getvalue_mask = CFG_480I_SLHYBDEP_GETMASK
    },
    .value_string = &SLDesc
};

config_t sl_str_480i = {
    .cfg_word        = &cfg_data_image480i,
    .cfg_word_offset = CFG_480I_SLSTR_OFFSET,
    .cfg_type        = VALUE,
    .value_details   = {
        .max_value     = CFG_SLSTR_MAX_VALUE,
        .getvalue_mask = CFG_480I_SLSTR_GETMASK
    },
    .value_string = &SLDesc
};

config_t sl_link_480i = {
    .cfg_word        = &cfg_data_image480i,
    .cfg_word_offset = CFG_480I_SL_LINK240P_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_480I_SL_LINK240P_SETMASK,
        .clrflag_mask = CFG_480I_SL_LINK240P_CLRMASK
    },
    .value_string = &LinkSL
};

config_t sl_id_480i = {
    .cfg_word        = &cfg_data_image480i,
    .cfg_word_offset = CFG_480I_SL_ID_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks   = {
        .setflag_mask = CFG_480I_SL_ID_SETMASK,
        .clrflag_mask = CFG_480I_SL_ID_CLRMASK
    },
    .value_string = &EvenOdd
};

config_t sl_en_480i = {
    .cfg_word = &cfg_data_image480i,
    .cfg_word_offset = CFG_480I_SL_EN_OFFSET,
    .cfg_type        = FLAG,
    .flag_masks      = {
        .setflag_mask = CFG_480I_SL_EN_SETMASK,
        .clrflag_mask = CFG_480I_SL_EN_CLRMASK
    },
    .value_string = &OffOn
};


#endif /* CFG_HEADER_CFG_IO_P_H_ */
