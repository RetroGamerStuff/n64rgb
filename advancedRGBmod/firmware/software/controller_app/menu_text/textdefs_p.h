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
 * menu_text/textdefs_p.h
 *
 *  Created on: 14.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#ifndef MENU_TEXT_TEXTDEFS_P_H_
#define MENU_TEXT_TEXTDEFS_P_H_

#include <string.h>
#include <unistd.h>


#define HEADER_UNDERLINE      0x08
#define HOME_LOWSEC_UNDERLINE 0x01
#define HEADER_H_OFFSET       0
#define OVERLAY_H_OFFSET      2
#define OVERLAY_V_OFFSET      0
#define OVERLAY_V_OFFSET_WH   2
#define TEXTOVERLAY_H_OFFSET  0
#define HOMEOVERLAY_H_OFFSET  3

#define COPYRIGHT_SIGN          0x0A
#define COPYRIGHT_H_OFFSET      (VD_WIDTH - 15)
#define COPYRIGHT_V_OFFSET      (VD_HEIGHT - 1)
#define COPYRIGHT_SIGN_H_OFFSET (COPYRIGHT_H_OFFSET - 2)

#define CR_SIGN_LICENSE_H_OFFSET  17
#define CR_SIGN_LICENSE_V_OFFSET   2

#define VERSION_H_OFFSET 20
#define VERSION_V_OFFSET  5


#define BTN_OVERLAY_0_H_OFFSET  (VD_WIDTH - 12)
#define BTN_OVERLAY_0_V_OFFSET  (VD_HEIGHT - 4)
#define BTN_OVERLAY_1_H_OFFSET  (VD_WIDTH - 12)
#define BTN_OVERLAY_1_V_OFFSET  (VD_HEIGHT - 3)

#define CFG_OVERLAY_H_OFFSET  OVERLAY_H_OFFSET
#define CFG_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define CFG_VALS_H_OFFSET     (27 + CFG_OVERLAY_H_OFFSET)
#define CFG_VALS_V_OFFSET     CFG_OVERLAY_V_OFFSET
#define CFG_LINEX2_V_OFFSET   ( 1 + CFG_VALS_V_OFFSET)
#define CFG_480IBOB_V_OFFSET  ( 2 + CFG_VALS_V_OFFSET)
#define CFG_SLOPTS_V_OFFSET   ( 3 + CFG_VALS_V_OFFSET)
#define CFG_FORMAT_V_OFFSET   ( 4 + CFG_VALS_V_OFFSET)
#define CFG_DEBLUR_V_OFFSET   ( 5 + CFG_VALS_V_OFFSET)
#define CFG_15BIT_V_OFFSET    ( 6 + CFG_VALS_V_OFFSET)
#define CFG_GAMMA_V_OFFSET    ( 7 + CFG_VALS_V_OFFSET)

#define CFG_SL_OVERLAY_H_OFFSET   OVERLAY_H_OFFSET
#define CFG_SL_OVERLAY_V_OFFSET   OVERLAY_V_OFFSET_WH
#define CFG_SL_VALS_H_OFFSET      (25 + CFG_SL_OVERLAY_H_OFFSET)
#define CFG_SL_VALS_V_OFFSET      CFG_SL_OVERLAY_V_OFFSET
#define CFG_SL_EN_V_OFFSET        ( 0 + CFG_SL_VALS_V_OFFSET)
#define CFG_SL_ID_V_OFFSET        ( 1 + CFG_SL_VALS_V_OFFSET)
#define CFG_SL_STR_V_OFFSET       ( 2 + CFG_SL_VALS_V_OFFSET)

#define MISC_OVERLAY_H_OFFSET     OVERLAY_H_OFFSET
#define MISC_OVERLAY_V_OFFSET     OVERLAY_V_OFFSET_WH
#define MISC_VALS_H_OFFSET        (23 + MISC_OVERLAY_H_OFFSET)
#define MISC_VALS_V_OFFSET        CFG_OVERLAY_V_OFFSET
#define MISC_IGR_RESET_V_OFFSET   ( 1 + MISC_VALS_V_OFFSET)
#define MISC_IGR_QUICK_V_OFFSET   ( 2 + MISC_VALS_V_OFFSET)
#define MISC_FILTERADDON_V_OFFSET ( 5 + MISC_VALS_V_OFFSET)

#define RWDATA_OVERLAY_H_OFFSET  OVERLAY_H_OFFSET
#define RWDATA_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define RWDATA_SAVE_FL_V_OFFSET  (1 + RWDATA_OVERLAY_V_OFFSET)
#define RWDATA_LOAD_FL_V_OFFSET  (3 + RWDATA_OVERLAY_V_OFFSET)
#define RWDATA_LOAD_JS_V_OFFSET  (4 + RWDATA_OVERLAY_V_OFFSET)
#define RWDATA_LOAD_N64_V_OFFSET (5 + RWDATA_OVERLAY_V_OFFSET)


#define INFO_OVERLAY_H_OFFSET OVERLAY_H_OFFSET
#define INFO_OVERLAY_V_OFFSET OVERLAY_V_OFFSET_WH
#define INFO_VALS_H_OFFSET    (18 + INFO_OVERLAY_H_OFFSET)
#define INFO_VALS_V_OFFSET    INFO_OVERLAY_V_OFFSET

#define INFO_VIN_V_OFFSET     (1 + INFO_VALS_V_OFFSET)
#define INFO_VOUT_V_OFFSET    (2 + INFO_VALS_V_OFFSET)
#define INFO_COL_V_OFFSET     (3 + INFO_VALS_V_OFFSET)
#define INFO_FORMAT_V_OFFSET  (4 + INFO_VALS_V_OFFSET)
#define INFO_DEBLUR_V_OFFSET  (5 + INFO_VALS_V_OFFSET)
#define INFO_FAO_V_OFFSET     (6 + INFO_VALS_V_OFFSET)


#define MAIN_OVERLAY_H_OFFSET 2
#define MAIN_OVERLAY_V_OFFSET OVERLAY_V_OFFSET_WH

#define MAIN2VINFO_V_OFFSET   (0 + MAIN_OVERLAY_V_OFFSET)
#define MAIN2CFG_V_OFFSET     (1 + MAIN_OVERLAY_V_OFFSET)
#define MAIN2MISC_V_OFFSET    (2 + MAIN_OVERLAY_V_OFFSET)
#define MAIN2SAVE_V_OFFSET    (3 + MAIN_OVERLAY_V_OFFSET)
#define MAIN2ABOUT_V_OFFSET   (5 + MAIN_OVERLAY_V_OFFSET)
#define MAIN2THANKS_V_OFFSET  (6 + MAIN_OVERLAY_V_OFFSET)
#define MAIN2LICENSE_V_OFFSET (7 + MAIN_OVERLAY_V_OFFSET)


static const char *copyright_note =
    "2018 borti4938"; /* 14 chars */

static const char *btn_overlay_0 =
    "A ... Enter\n"
    "B ... Close";

static const char *btn_overlay_1 =
    "A ... Enter\n"
    "B ... Back";

static const char *vinfo_header =
    "N64 Advanced - Video-Info";
static const char *vinfo_overlay =
    "* Video\n"
    "  - Input:\n"
    "  - Output:\n"
    "  - Color Depth:\n"
    "  - Format:\n"
    "* 240p-DeBlur:\n"
    "* Filter AddOn:";

static const char *cfg_header =
    "N64 Advanced - Configuration";
static const char *cfg_overlay =
    "* Linedoubling:\n"
    "  - LineX2:\n"
    "  - 480i de-int. (bob):\n"
    "  - Scanlines:\n"
    "* Output Format:\n"
    "* 240p-DeBlur:\n"
    "* 15bit Mode:\n"
    "* Gamma Value:";

static const char *cfg_sl_header =
    "N64 Advanced - Scanline configuration";
static const char *cfg_sl_overlay =
    "* Use Scanlines:\n"
    "* Scanline ID:\n"
    "* Scanline Strength:";

static const char *misc_header =
    "N64 Advanced - Miscellaneous";
static const char *misc_overlay =
    "* In-Game Routines:\n"
    "  - Reset:\n"
    "  - Quick-Access:\n\n"
    "* Filter AddOn:\n"
    "  - Filter Cut-Off:";

static const char *rwdata_header =
    "N64 Advanced - Load/Save";
static const char *rwdata_overlay =
    "Save:\n"
    " - Configuration\n"
    "Load:\n"
    " - Last Configuration\n"
    " - Defaults from Jumper Set\n"
    " - N64 Standard";

static const char *thanks_overlay =
    "The N64 RGB project would not be what it is without\n"
    "the contributions of many other people. Here, I want\n"
    "to point out especially:\n"
    " - viletim  : First public DIY N64 DAC project\n"
    " - Ikari_01 : Initial implementation of PAL/NTSC\n"
    "              as well as 480i/576i detection\n"
    " - sftwninja: Pushing me to the N64A project\n"
    " - Xenogears: Sponsoring of prototypes\n\n"
    "Visit GitHub:\n"
    "      <https://github.com/borti4938/n64rgb>\n"
    "Any contribution in any kind is highly welcomed!";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *about_overlay =
    "The N64 RGB project is open source, i.e.PCB files,\n"
    "HDL and SW sources are provided to you FOR FREE!\n\n"
    "Your version\n"
    " - firmware (HDL):\n"
    " - firmware (SW) :\n\n"
    "Questions / (limited) Support:\n"
    " - GitHub: <https://github.com/borti4938/n64rgb>\n"
    " - Email:  <borti4938@gmx.de>";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *license_overlay =
    "The N64Advanced is part of the\n"
    "N64 RGB/YPbPr DAC project\n"
    "       Copyright   2015 - 2018 Peter Bartmann\n"
    "This project is published under GNU GPL v3.0 or\n"
    "later. You should have received a copy of the GNU\n"
    "General Public License along with this project.\n"
    "If not, see\n"
    "        <http://www.gnu.org/licenses/>.\n\n"
    "What ever you do, also respect licenses of third\n"
    "party vendors providing the design tools...";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *home_header =
    "N64 Advanced - Main Menu";
static const char *home_overlay =
    "[Video-Info]\n"
    "[Configuration]\n"
    "[Miscellaneous]\n"
    "[Load/Save]\n\n"
    "About...\n"
    "Acknowledgment...\n"
    "License...";

const char *EnterSubMenu = "[Enter submenu]";

const char *OffOn[]        = {"Off","On"};
const char *EvenOdd[]      = {"Even","Odd"};
const char *VideoMode[]    = {"240p60","288p50","480i60","576i50","480p60","576p50"};
const char *VideoColor[]   = {"21bit","15bit"};
const char *VideoFormat[]  = {"RGBS","RGBS/RGsB","YPbPr"};
const char *DeBlur[]       = {"(estimated)","(forced)","(480i/576i)"};
const char *DeBlurCfg[]    = {"Auto","Off","Always"};
const char *FilterAddOn[]  = {"Auto","9.5MHz","18MHz","(bypassed)","(not installed)"};
const char *SLStrength[]   = {"  6.25%"," 12.50%"," 18.75%"," 25.00%",
                              " 31.25%"," 37.50%"," 43,75%"," 50.00%",
                              " 56.25%"," 62.50%"," 68.75%"," 75.00%",
                              " 81.25%"," 87.50%"," 93,75%","100.00%"};
const char *GammaValue[]   = {"0.70","0.75","0.80","0.85","0.90","1.00","1.10","1.15","1.20"};
const char *QuickChange[]  = {"Off","DeBlur","15bit mode","All"};

#endif /* MENU_TEXT_TEXTDEFS_P_H_ */
