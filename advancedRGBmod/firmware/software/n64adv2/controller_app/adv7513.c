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
 * adv7513.c
 *
 *  Created on: 11.09.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/


#include "alt_types.h"
#include "i2c_opencores.h"
#include "system.h"

#include "adv7513.h"
#include "adv7513_regs_p.h"

int check_adv7513()
{
  if (adv7513_readreg(ADV7513_REG_CHIP_REVISION) != ADV7513_CHIP_ID)
    return -ADV_INIT_FAILED;
  return 0;
}

void init_adv7513() {
  while (!ADV_POWER_RDY(adv7513_readreg(ADV7513_REG_STATUS))) {}

  adv7513_writereg(ADV7513_REG_POWER, 0x10);
  //adv7513_writereg(ADV7513_REG_POWER2, 0xc0);

  adv7513_writereg(ADV7513_REG_INT0(2), 0x03);          // Must be set to 0x03 for proper operation
  adv7513_writereg(ADV7513_REG_INT0(4), 0xE0);          // Must be set to 0b1110000
  adv7513_writereg(ADV7513_REG_INT0(6), 0x30);          // Must be set to 0x30 for proper operation
  adv7513_writereg(ADV7513_REG_INPUT_CLK_DIV, 0x61);    // [7:4] Must be set to Default Value (0110)
                                                        // [3:2] Input Video CLK Divide: 01 = Input Clock Divided by 2
                                                        // [1:0] Must be set to 1 for proper operation
  adv7513_writereg(ADV7513_REG_INT1(0), 0xA4);          // Must be set to 0xA4 for proper operation
  adv7513_writereg(ADV7513_REG_INT1(1), 0xA4);          // Must be set to 0xA4 for proper operation
  adv7513_reg_bitclear(ADV7513_REG_INT1(2), 6);         // enable Video CLK Divide output if bit 6 is set
  adv7513_writereg(ADV7513_REG_INT2, 0xD0);             // Must be set to 0xD0 for proper operation
  adv7513_writereg(ADV7513_REG_INT3, 0x00);             // Must be set to 0x00 for proper operation

  adv7513_writereg(ADV7513_REG_I2C_FREQ_ID_CFG, 0x20);  // [7:4] Sampling frequency for I2S audio: 0010 = 48.0 kHz
                                                        // [3:0] Input Video Format: 0000 = 24 bit RGB 4:4:4 or YCbCr 4:4:4 (separate syncs)
  adv7513_writereg(ADV7513_REG_VIDEO_INPUT_CFG1, 0x32); // [5:4] Color Depth for Input Video Data: 11 = 8 bit
                                                        // [3:2] Input Style: 00 = Normal RGB or YCbCr 4:4:4 (24 bits) with Separate Syncs
                                                        // [1] Video data input edge selection: 1 = rising edge
                                                        // [0] Input Color Space Selection: 0 = RGB
  adv7513_reg_bitset(ADV7513_REG_TIMING_GEN_SEQ,1);     // first generate DE (and then adjust HV sync if needed)
  adv7513_reg_bitset(ADV7513_REG_VIDEO_INPUT_CFG2,0);   // enable internal DE generator
  adv7513_writereg(ADV7513_REG_PIXEL_REPETITION,0xE0);  // [7] must be set to 1
                                                        // [6:5] 10 or 11 = use VIC manual

  adv7513_writereg(ADV7513_REG_HDCP_HDMI_CFG, 0x06);    // [6:5] and [3:2] Must be set to Default Value (00 and 01 respectively)
                                                        // [1] HDMI Mode: 1 = HDMI Mode

  adv7513_writereg(ADV7513_REG_AN(10), 0x60);           // [7:5] Programmable delay for input video clock: 011 = no delay
                                                        // [3] Must be set to Default Value (0)

  adv7513_writereg(ADV7513_REG_N0, 0x00);               // N value for 48kHz
  adv7513_writereg(ADV7513_REG_N1, 0x18);               // 6144 decimal equals binary
  adv7513_writereg(ADV7513_REG_N2, 0x00);               // 0000 0000 0001 1000 0000 0000
  adv7513_writereg(ADV7513_REG_AUDIO_SOURCE, 0x03);     // [7] CTS Source Select: 0 = CTS Automatic
                                                        // [6:4] Audio Select: 000 = I2S
                                                        // [3:2] Mode Selection for Audio Select
                                                        // [1:0] MCLK Ratio: 11 = 512xfs
//  adv7513_reg_bitset(ADV7513_REG_AUDIO_CONFIG,5);       // [5] MCLK Enable: 1 = MCLK is available, 0 = MCLK not available
                                                        // [4:1] Must be set to Default Value (0111)
  adv7513_writereg(ADV7513_REG_I2S_CONFIG, 0x86);       // [7] Select source of audio sampling frequency: 1 = use sampling frequency from I2C register
                                                        // [2] I2S0 enable for the 4 I2S pins: 1 = Enabled
                                                        // [1:0] I2S Format: 10 = left justified mode, 00 = standard
  adv7513_writereg(ADV7513_REG_AUDIO_CFG3, 0x0B);       // [3:0] I2S Word length per channel: 1011 = 24bit

  adv7513_writereg(ADV7513_REG_INFOFRAME_UPDATE, 0xE0); // [7] Auto Checksum Enable: 1 = Use automatically generated checksum
                                                        // [6] AVI Packet Update: 1 = AVI Packet I2C update active
                                                        // [5] Audio InfoFrame Packet Update: 1 = Audio InfoFrame Packet I2C update active
  adv7513_writereg(ADV7513_REG_AVI_INFOFRAME(0), 0x01); // [6:5] Output format: 00 = RGB
                                                        // [1:0] Scan Information: 01 = TV, 10 = PC
  adv7513_writereg(ADV7513_REG_AVI_INFOFRAME(1), 0x19); // [5:4] Picture Aspect Ratio: 01 = 4:3
                                                        // [3:0] Active Format Aspect Ratio: 1001 = 4:3 (center)
  adv7513_writereg(ADV7513_REG_AVI_INFOFRAME(2), 0x08); // [3:2] RGB Quantization range: 10 = full range
  adv7513_writereg(ADV7513_REG_INFOFRAME_UPDATE, 0x80); // [7] Auto Checksum Enable: 1 = Use automatically generated checksum
                                                        // [6] AVI Packet Update: 0 = AVI Packet I2C update inactive
                                                        // [5] Audio InfoFrame Packet Update: 0 = Audio InfoFrame Packet I2C update inactive
}

void adv7513_vic_manual_setup(alt_u8 vmode, alt_u8 linex2)
{
  if (linex2 == 0) {
    if (vmode == 0) adv7513_writereg(ADV7513_REG_VIC_MANUAL, 0x08);
    else            adv7513_writereg(ADV7513_REG_VIC_MANUAL, 0x17);
    adv7513_reg_bitset(ADV7513_REG_INPUT_CLK_DIV, 2);
    adv7513_reg_bitset(ADV7513_REG_INT1(2), 6);
  } else {
    if (vmode == 0) adv7513_writereg(ADV7513_REG_VIC_MANUAL, 0x0E);
    else            adv7513_writereg(ADV7513_REG_VIC_MANUAL, 0x1D);
    adv7513_reg_bitclear(ADV7513_REG_INPUT_CLK_DIV, 2);
    adv7513_reg_bitclear(ADV7513_REG_INT1(2), 6);
  }
}

void adv7513_de_gen_setup (alt_u8 vmode, alt_u8 linex2)
{
  alt_u16 hs_delay, vs_delay, width, height; // ToDO: these numbers should be parameters!
  if (vmode == 0) {
    hs_delay = 224;
    if (linex2 == 0) {
      vs_delay = 17;
      height = 240;
    } else {
      vs_delay = 68;
      height = 480;
    }
  } else {
    hs_delay = 274;
    if (linex2 == 0) {
      vs_delay = 21;
      height = 288;
    } else {
      vs_delay = 42;
      height = 576;
    }
  }
  width = 1278;
  /*
   * External Sync Input Modes
      To properly frame the active video, the ADV7513 can use an external DE (via external pin) or can generate its own DE signal.
      To activate the internal DE generation, set register 0x17[0] to 1. Registers 0x35 -- 0x3A and 0xFB are used to define the DE.
      Registers 0xFB[7], 0x35 and 0x36[7:6] define the number of pixels from the Hsync leading edge to the DE leading edge minus
      one. Registers 0xFB[6:5] and 0x36[5:0] is the number of Hsyncs between leading edge of VS and DE. Register 0x37[7:5]
      defines the difference of Hsync counts during Vsync blanking for the two fields in interlaced video. Registers 0xFB[4],
      0x37[4:0] and 0x38[7:1] indicate the width of the DE. Registers 0x39 and 0x3A[7:4] are the number of lines of active video.
   */

//  alt_u8 de_setup_msbs = (((hs_delay & (0x1 << 10)) >> 10) << 7) |
//                         (((vs_delay & (0x3 <<  6)) >>  6) << 5) |
//                         (((width    & (0x1 << 12)) >> 12) << 4) |
//                         (((height   & (0x1 << 12)) >> 12) << 3) |
//                         (adv7513_readreg(ADV7513_REG_DE_GENERATOR_MSBS) & 0x7);
//  adv7513_writereg(ADV7513_REG_DE_GENERATOR_MSBS,de_setup_msbs);

  adv7513_writereg(ADV7513_REG_DE_GENERATOR(0),(hs_delay & (0xff << 2)) >> 2);
  adv7513_writereg(ADV7513_REG_DE_GENERATOR(1),((hs_delay & 0x3) << 6) | (vs_delay & 0x3f));
  adv7513_writereg(ADV7513_REG_DE_GENERATOR(2),((width & (0x1f << 7)) >> 7));
  adv7513_writereg(ADV7513_REG_DE_GENERATOR(3),((width & 0x7f) << 1));
  adv7513_writereg(ADV7513_REG_DE_GENERATOR(4),((height & (0xff << 4)) >> 4));
  adv7513_writereg(ADV7513_REG_DE_GENERATOR(5),((height & 0xf) << 4));

//  if linex2 = 0

}

alt_u8 adv7513_readreg(alt_u8 regaddr)
{
    //Phase 1
    I2C_start(I2C_MASTER_BASE, ADV7513_I2C_BASE, 0);
    I2C_write(I2C_MASTER_BASE, regaddr, 0);

    //Phase 2
    I2C_start(I2C_MASTER_BASE, ADV7513_I2C_BASE, 1);
    return (alt_u8) I2C_read(I2C_MASTER_BASE,1);
}

void adv7513_writereg(alt_u8 regaddr, alt_u8 data)
{
    I2C_start(I2C_MASTER_BASE, ADV7513_I2C_BASE, 0);
    I2C_write(I2C_MASTER_BASE, regaddr, 0);
    I2C_write(I2C_MASTER_BASE, data, 1);
}


inline void adv7513_reg_bitset(alt_u8 regaddr, alt_u8 bit) {
  if (bit > 7) return;
  adv7513_writereg(regaddr,(adv7513_readreg(regaddr) | (1 << bit)));
}

inline void adv7513_reg_bitclear(alt_u8 regaddr, alt_u8 bit) {
  if (bit > 7) return;
  adv7513_writereg(regaddr,(adv7513_readreg(regaddr) & ~(1 << bit)));
}
