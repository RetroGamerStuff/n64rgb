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
 * adv7513.h
 *
 *  Created on: 11.09.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include "system.h"
#include "alt_types.h"


#ifndef ADV7513_H_
#define ADV7513_H_

#define ADV7513_CHIP_ID     0x13
#define ADV7513_I2C_BASE    (0x72>>1)
#define ADV7513_REG_STATUS  0x42

#define ADV_INIT_FAILED 150 // ToDo: move codes into separate header file?

#define ADV_POWER_RDY(x)      ((x & 0x70) == 0x70)
#define ADV_MONITOR_SENSE(x)  (x & 0x20)


int check_adv7513(void);
void init_adv7513(void);
void adv7513_vic_manual_setup(alt_u8 vmode, alt_u8 linex2);
void adv7513_de_gen_setup(alt_u8 vmode, alt_u8 linex2);
alt_u8 adv7513_readreg(alt_u8 regaddr);
void adv7513_writereg(alt_u8 regaddr, alt_u8 data);
inline void adv7513_reg_bitset(alt_u8 regaddr, alt_u8 bit);
inline void adv7513_reg_bitclear(alt_u8 regaddr, alt_u8 bit);


#endif /* ADV7513_H_ */
