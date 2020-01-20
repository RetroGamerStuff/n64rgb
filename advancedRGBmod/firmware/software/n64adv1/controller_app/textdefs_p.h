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
 * menu_text/textdefs_p.h
 *
 *  Created on: 14.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/

#include <string.h>
#include <unistd.h>


#ifndef MENU_TEXT_TEXTDEFS_P_H_
#define MENU_TEXT_TEXTDEFS_P_H_

#define HEADER_UNDERLINE      0x08
#define HOME_LOWSEC_UNDERLINE 0x01
#define HEADER_H_OFFSET       24
#define HEADER_V_OFFSET        0
#define OVERLAY_H_OFFSET       2
#define OVERLAY_V_OFFSET       0
#define OVERLAY_V_OFFSET_WH    2
#define TEXTOVERLAY_H_OFFSET   0
#define HOMEOVERLAY_H_OFFSET   3

#define COPYRIGHT_SIGN          0x0A
#define COPYRIGHT_H_OFFSET      (VD_WIDTH - 14)
#define COPYRIGHT_V_OFFSET      (VD_HEIGHT - 1)
#define COPYRIGHT_SIGN_H_OFFSET (COPYRIGHT_H_OFFSET - 2)

#define CR_SIGN_LICENSE_H_OFFSET  18
#define CR_SIGN_LICENSE_V_OFFSET   2

#define VERSION_H_OFFSET (OVERLAY_H_OFFSET + 17)
#define VERSION_V_OFFSET (OVERLAY_V_OFFSET +  4)


#define CFG_OVERLAY_H_OFFSET    OVERLAY_H_OFFSET
#define CFG_OVERLAY_V_OFFSET    OVERLAY_V_OFFSET_WH
#define CFG_VALS_H_OFFSET       (23 + CFG_OVERLAY_H_OFFSET)
#define CFG_VALS_V_OFFSET       CFG_OVERLAY_V_OFFSET
#define CFG_240P_SET_V_OFFSET   ( 1 + CFG_VALS_V_OFFSET)
#define CFG_480I_SET_V_OFFSET   ( 2 + CFG_VALS_V_OFFSET)
#define CFG_FORMAT_V_OFFSET     ( 3 + CFG_VALS_V_OFFSET)
#define CFG_DEBLURMODE_V_OFFSET ( 4 + CFG_VALS_V_OFFSET)
#define CFG_DEBLURADV_V_OFFSET  ( 5 + CFG_VALS_V_OFFSET)
#define CFG_15BIT_V_OFFSET      ( 6 + CFG_VALS_V_OFFSET)
#define CFG_GAMMA_V_OFFSET      ( 7 + CFG_VALS_V_OFFSET)

#define CFG_VSUB_OVERLAY_H_OFFSET   OVERLAY_H_OFFSET
#define CFG_VSUB_OVERLAY_V_OFFSET   OVERLAY_V_OFFSET_WH
#define CFG_VSUB_VALS_H_OFFSET      (28 + CFG_VSUB_OVERLAY_H_OFFSET)
#define CFG_VSUB_VALS_V_OFFSET      CFG_VSUB_OVERLAY_V_OFFSET
#define CFG_VSUB_LINEX_V_OFFSET     ( 0 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_VPLL_V_OFFSET      ( 1 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_FIELDFIX_V_OFFSET  ( 1 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_SL_EN_V_OFFSET     ( 2 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_SL_METHOD_V_OFFSET ( 3 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_SL_ID_V_OFFSET     ( 4 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_SL_STR_V_OFFSET    ( 5 + CFG_VSUB_OVERLAY_V_OFFSET)
#define CFG_VSUB_SLHYB_STR_V_OFFSET ( 6 + CFG_VSUB_OVERLAY_V_OFFSET)

#define CFG_DBSUB_OVERLAY_H_OFFSET  OVERLAY_H_OFFSET
#define CFG_DBSUB_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define CFG_DBSUB_VALS_H_OFFSET     (29 + CFG_VSUB_OVERLAY_H_OFFSET)
#define CFG_DBSUB_VALS_V_OFFSET     CFG_DBSUB_OVERLAY_V_OFFSET
#define CFG_DBSUB_P2P_V_OFFSET      ( 1 + CFG_DBSUB_OVERLAY_V_OFFSET)
#define CFG_DBSUB_DBCNT_V_OFFSET    ( 2 + CFG_DBSUB_OVERLAY_V_OFFSET)
#define CFG_DBSUB_DBCNTDZ_V_OFFSET  ( 3 + CFG_DBSUB_OVERLAY_V_OFFSET)
#define CFG_DBSUB_DBSTAB_V_OFFSET   ( 5 + CFG_DBSUB_OVERLAY_V_OFFSET)
#define CFG_DBSUB_DBRST_V_OFFSET    ( 6 + CFG_DBSUB_OVERLAY_V_OFFSET)

#define CFG_VPLLSUB_OVERLAY_H_OFFSET  TEXTOVERLAY_H_OFFSET
#define CFG_VPLLSUB_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define CFG_VPLLSUB_VALS_H_OFFSET     (26 + CFG_VPLLSUB_OVERLAY_H_OFFSET)
#define CFG_VPLLSUB_VALS_V_OFFSET     CFG_VPLLSUB_OVERLAY_V_OFFSET
#define CFG_VPLLSUB_TEST_V_OFFSET     ( 4 + CFG_VPLLSUB_OVERLAY_V_OFFSET)
#define CFG_VPLLSUB_EN_V_OFFSET       ( 5 + CFG_VPLLSUB_OVERLAY_V_OFFSET)

#define MISC_OVERLAY_H_OFFSET     OVERLAY_H_OFFSET
#define MISC_OVERLAY_V_OFFSET     OVERLAY_V_OFFSET_WH
#define MISC_VALS_H_OFFSET        (23 + MISC_OVERLAY_H_OFFSET)
#define MISC_VALS_V_OFFSET        CFG_OVERLAY_V_OFFSET
#define MISC_IGR_RESET_V_OFFSET   ( 1 + MISC_VALS_V_OFFSET)
#define MISC_IGR_QUICK_V_OFFSET   ( 2 + MISC_VALS_V_OFFSET)
#define MISC_FILTERADDON_V_OFFSET ( 4 + MISC_VALS_V_OFFSET)
#define MISC_SHOWTESTPAT_V_OFFSET ( 6 + MISC_VALS_V_OFFSET)

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
#define INFO_VPLL_V_OFFSET    (2 + INFO_VALS_V_OFFSET)
#define INFO_VOUT_V_OFFSET    (3 + INFO_VALS_V_OFFSET)
#define INFO_COL_V_OFFSET     (4 + INFO_VALS_V_OFFSET)
#define INFO_FORMAT_V_OFFSET  (5 + INFO_VALS_V_OFFSET)
#define INFO_DEBLUR_V_OFFSET  (6 + INFO_VALS_V_OFFSET)
#define INFO_FAO_V_OFFSET     (7 + INFO_VALS_V_OFFSET)


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
    "2020 borti4938"; /* 14 chars */

const char *btn_overlay_0 =
    "A ... Enter\n"
    "B ... Close";

const char *btn_overlay_1 =
    "A ... Enter\n"
    "B ... Back";

const char *btn_overlay_2 =
    "A ... Confirm\n"
    "B ... Cancel";

static const char *vinfo_header =
    "Video-Info";
static const char *vinfo_overlay =
    "* Video\n"
    "  - Input:\n"
    "  - PLL:\n"
    "  - Output:\n"
    "  - Color Depth:\n"
    "  - Format:\n"
    "* 240p-DeBlur:\n"
    "* Filter AddOn:";

static const char *cfg_header =
    "Configuration";
static const char *cfg_overlay =
    "* Linemultiplier:\n"
    "  - 240p settings:\n"
    "  - 480i settings:\n"
    "* Output Format:\n"
    "* 240p-DeBlur:\n"
    "  - Adv. settings:\n"
    "* 15bit Mode:\n"
    "* Gamma Value:";

static const char *cfg_240p_opt_header =
    "Config. (240p)";
static const char *cfg_240p_opt_overlay =
    "* Enable Linemultiplier:\n"
    "  - Video PLL:\n"
    "* Use Scanlines:\n"
    "  - Method:\n"
    "  - Scanline ID:\n"
    "  - Scanline Strength:\n"
    "  - Hybrid Depth:";

static const char *cfg_480i_opt_header =
    "Config. (480i)";
static const char *cfg_480i_opt_overlay =
    "* De-Interlacing (Bob):\n"
    "  - Field-Shift Fix:\n"
    "* Use Scanlines:\n"
    "  - Method:\n"
    "  - Scanline ID:\n"
    "  - Scanline Strength:\n"
    "  - Hybrid Depth:";


static const char *cfg_deblur_opt_header =
    "Adv.DeBlur Config.";
static const char *cfg_deblur_opt_overlay =
    "* Picture Sensitivity:\n"
    "  - Pixel-to-Pixel:\n"
    "  - Pixel cnt. th. high:\n"
    "  - Pixel cnt. th. low:\n"
    "* Content Stability:\n"
    "  - Frame cnt. th. bit:\n"
    "  - Reset estimate:";


static const char *cfg_vpll_opt_header =
    "Config. (VPLL)";
static const char *cfg_vpll_opt_overlay =
    "In order to get LineX3 available, you have to have\n"
    "video PLL running and locked. If VPLL lost lock\n"
    "during runtime, you may try to re-enable VPLL here.\n\n"
    "  * Test Video PLL:\n"
    "  * Enable Video PLL:";
  /* 1234567890123456789012345678901234567890123456789012 */



static const char *misc_header =
    "Miscellaneous";
static const char *misc_overlay =
    "* In-Game Routines:\n"
    "  - Reset:\n"
    "  - Quick-Access:\n"
    "* Filter AddOn:\n"
    "  - Filter Cut-Off:\n\n"
    "* Show Test-Pattern (alpha state)\n"
    "  [A to enter, B to exit]";

static const char *rwdata_header =
    "Load/Save";
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
    " - viletim  : First public DIY N64 RGB DAC project\n"
    " - Ikari_01 : Initial implementation of PAL/NTSC\n"
    "              as well as 480i/576i detection\n"
    " - sftwninja: Pushing me to the N64Adv. project\n"
    " - Xenogears: Sponsoring of prototypes\n"
    " - ArcadeTV:  Logo design\n"
    "Visit the GitHub project:\n"
    "      <https://github.com/borti4938/n64rgb>\n"
    "Any contribution in any kind is highly welcomed!";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *about_overlay =
    "The N64 RGB project is open source, i.e. PCB files,\n"
    "HDL and SW sources are provided to you FOR FREE!\n\n"
    "Your version\n"
    " - firmware (HDL):\n"
    " - firmware (SW) :\n\n"
    "Questions / (limited) Support:\n"
    " - GitHub: <https://github.com/borti4938/n64rgb>\n"
    " - Email:  <borti4938@gmail.com>";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *license_overlay =
    " The N64Advanced is part of the\n"
    " N64 RGB/YPbPr DAC project\n"
    "        Copyright   2015 - 2020 Peter Bartmann\n"
    " This project is published under GNU GPL v3.0 or\n"
    " later. You should have received a copy of the GNU\n"
    " General Public License along with this project.\n"
    " If not, see\n"
    "         <http://www.gnu.org/licenses/>.\n\n"
    " What ever you do, also respect licenses of third\n"
    " party vendors providing the design tools...";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *home_header =
    "Main Menu";
static const char *home_overlay =
    "[Video-Info]\n"
    "[Configuration]\n"
    "[Miscellaneous]\n"
    "[Load/Save]\n\n"
    "About...\n"
    "Acknowledgment...\n"
    "License...";

const char *EnterSubMenu    = "[Enter submenu]";
const char *StartTest       = "[Start VPLL Test]";
const char *LineX3VPLLHint  = "*LineX3: needs VPLL enabled";
const char *LineX3Hint      = "*LineX3: only available in NTSC video mode";

const char *OffOn[]         = {"Off","On"};
const char *LineX_240p[]    = {"LineX Off","LineX2","LineX3*"};
const char *VideoPLL[]      = {"Off","Locked"};
const char *EvenOdd[]       = {"Even","Odd "};
const char *AdvSL[]         = {"Simple","Advanced"};
const char *LinkSL[]        = {"480i ind.","Link 240p"};
const char *VideoMode[]     = {"240p60","480i60","288p50","576i50","480p60","576p50","720p60 (pseudo)"};
const char *VideoColor[]    = {"21bit","15bit"};
const char *VideoFormat[]   = {"RGBS","RGBS/RGsB","YPbPr"};
const char *DeBlur[]        = {"(estimated)","(forced)","(480i/576i)"};
const char *DeBlurMode[]    = {"Auto","Off","Always"};
const char *DeBlurPixSens[] = {"Normal","High"};
const char *DeBlurRst[]     = {"Never","On Vmode change","On reset","On both"};
const char *FilterAddOn[]   = {"(Auto)"," 9.5MHz","18.0MHz","36.0MHz","(bypassed)","(not installed)"};

const char *QuickChange[]   = {"Off","DeBlur","15bit mode","All"};

#endif /* MENU_TEXT_TEXTDEFS_P_H_ */
