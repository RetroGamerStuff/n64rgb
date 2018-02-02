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
 * flash.h
 *
 *  Created on: 31.01.2018
 *      Author: Peter Bartmann
 *
 *********************************************************************************
 *
 * This file is inspired by the Open Source Scan Converter project (file flash.h)
 * which is
 *
 * Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
 *
 * The OSSC project is published under the GPL license version 3 at GitHub
 * <https://github.com/marqs85/ossc>
 *
 ********************************************************************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "alt_types.h"
#include "altera_epcq_controller2.h"

#define FLASH_DETECT_ERROR      200
#define FLASH_READ_ERROR        201
#define FLASH_ERASE_ERROR       202
#define FLASH_WRITE_ERROR       203

void set_flash(alt_epcq_controller2_dev* *epcq_controller2_dev);
int check_flash(alt_epcq_controller2_dev *epcq_controller2_dev);
int read_flash(alt_epcq_controller2_dev *epcq_controller2_dev, alt_u32 offset, alt_u32 length, alt_u8 *dstbuf);
int write_flash_page(alt_epcq_controller2_dev *epcq_controller2_dev, alt_u8 *pagedata, alt_u32 length, alt_u32 pagenum);

#endif /* FLASH_H_ */
