/*********************************************************************************
 *
 * This file is part of the N64 RGB/YPbPr DAC project.
 *
 * Copyright (C) 2015-2019 by Peter Bartmann <borti4938@gmx.de>
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
 * flash.c
 *
 *  Created on: 31.01.2018
 *      Author: Peter Bartmann
 *
 *********************************************************************************
 *
 * This file is inspired by the Open Source Scan Converter project (file flash.c)
 * which is
 *
 * Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
 *
 * The OSSC project is published under the GPL license version 3 at GitHub
 * <https://github.com/marqs85/ossc>
 *
 ********************************************************************************/

#include <unistd.h>
#include "system.h"
#include "sys/alt_sys_init.h"
#include "altera_epcq_controller.h"
#include "altera_epcq_controller_regs.h"
#include "flash.h"

ALTERA_EPCQ_CONTROLLER_AVL_MEM_AVL_CSR_INSTANCE ( EPCQ_CONTROLLER_0, EPCQ_CONTROLLER_0_AVL_MEM, EPCQ_CONTROLLER_0_AVL_CSR, epcq_controller_0);
alt_epcq_controller_dev *epcq_controller_dev;

alt_u8 use_flash = 0;

int check_flash()
{
  epcq_controller_dev = &epcq_controller_0;

  if ((epcq_controller_dev == NULL) || !(epcq_controller_dev->is_epcs && (epcq_controller_dev->page_size == PAGESIZE)))
    return -FLASH_DETECT_ERROR;

  use_flash = 1;

  return 0;
}

int read_flash(alt_u32 offset, alt_u32 length, alt_u8 *dstbuf)
{
  int retval, i;

  retval = alt_epcq_controller_read(&epcq_controller_dev->dev, offset, dstbuf, length);
  if (retval != 0) return -FLASH_READ_ERROR;

  for (i=0; i<length; i++)
    dstbuf[i] = ALT_CI_NIOS_CUSTOM_INSTR_BITSWAP_0(dstbuf[i]) >> 24;

  return 0;
}

int write_flash_page(alt_u8 *pagedata, alt_u32 length, alt_u32 pagenum)
{
  int retval, i;

  if ((pagenum % PAGES_PER_SECTOR) == 0) {
    retval = alt_epcq_controller_erase_block(&epcq_controller_dev->dev, pagenum*PAGESIZE);
    if (retval != 0) return -FLASH_ERASE_ERROR;
  }

  // Bit-reverse bytes for flash
  for (i=0; i<length; i++)
    pagedata[i] = ALT_CI_NIOS_CUSTOM_INSTR_BITSWAP_0(pagedata[i]) >> 24;

  retval = alt_epcq_controller_write_block(&epcq_controller_dev->dev, (pagenum/PAGES_PER_SECTOR)*PAGES_PER_SECTOR*PAGESIZE, pagenum*PAGESIZE, pagedata, length);

  if (retval != 0) return -FLASH_WRITE_ERROR;

  return 0;
}
