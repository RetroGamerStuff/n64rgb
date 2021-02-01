
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
 * cfg_int_p.h
 *
 *  Created on: 17.09.2020
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"

#include "config.h"


#ifndef CFG_INT_P_H_
#define CFG_INT_P_H_

extern const char  *OffOn[], *NTSCPAL_SEL[], *VTimingSel[];

config_t deblur_mode_powercycle = {
    // .cfg_b32word_t* must be NULL to show that this is a local value
    .cfg_type     = FLAG,
    .cfg_value    = OFF,
    .value_details = {
      .max_value = ON,
    },
    .value_string = &OffOn
};

config_t mode16bit_powercycle = {
    // .cfg_b32word_t* must be NULL to show that this is a local value
    .cfg_type     = FLAG,
    .cfg_value    = OFF,
    .value_details = {
      .max_value = ON,
    },
    .value_string = &OffOn
};

config_t ntsc_pal_selection = {
    // .cfg_b32word_t* must be NULL to show that this is a local value
    .cfg_type     = TXTVALUE, // treat as txtvalue for modifying function
    .cfg_value    = NTSC,
    .value_details = {
      .max_value = PAL,
    },
    .value_string = &NTSCPAL_SEL
};

config_t timing_selection = {
    // .cfg_b32word_t* must be NULL to show that this is a local value
    .cfg_type     = TXTVALUE, // treat as txtvalue for modifying function
    .cfg_value    = 0,
    .value_details = {
      .max_value = PAL_LX2_INT,
    },
    .value_string = &VTimingSel
};

#endif /* CFG_INT_P_H_ */
