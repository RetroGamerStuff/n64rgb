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

#define VD_240P_HEADER_H_OFFSET   9
#define VD_240P_OVERLAY_V_OFFSET  4
#define VD_480I_OVERLAY_V_OFFSET  5
#define VD_240P_OVERLAY_H_OFFSET  7

#define COPYRIGHT_SIGN          0x0A
#define COPYRIGHT_H_OFFSET      (VD_WIDTH - 14)
#define COPYRIGHT_V_OFFSET      (VD_HEIGHT - 1)
#define COPYRIGHT_SIGN_H_OFFSET (COPYRIGHT_H_OFFSET - 2)

#define CR_SIGN_LICENSE_H_OFFSET  18
#define CR_SIGN_LICENSE_V_OFFSET   2

#define VERSION_H_OFFSET (OVERLAY_H_OFFSET + 17)
#define VERSION_V_OFFSET (OVERLAY_V_OFFSET +  4)

#define CHECKBOX_TICK 0x0D

#define VICFG_OVERLAY_H_OFFSET    OVERLAY_H_OFFSET
#define VICFG_OVERLAY_V_OFFSET    OVERLAY_V_OFFSET_WH
#define VICFG_VALS_H_OFFSET       (28 + VICFG_OVERLAY_H_OFFSET)
#define VICFG_VALS_V_OFFSET       VICFG_OVERLAY_V_OFFSET

#define VICFG1_NTSC_PAL_AWARENESS_V_OFFSET  ( 0 + VICFG_VALS_V_OFFSET)
#define VICFG1_NTSC_PAL_SELECT_V_OFFSET     ( 1 + VICFG_VALS_V_OFFSET)
#define VICFG1_240P_SET_V_OFFSET            ( 2 + VICFG_VALS_V_OFFSET)
#define VICFG1_480I_SET_V_OFFSET            ( 3 + VICFG_VALS_V_OFFSET)
#define VICFG1_TIMING_V_OFFSET              ( 4 + VICFG_VALS_V_OFFSET)
#define VICFG1_GAMMA_V_OFFSET               ( 5 + VICFG_VALS_V_OFFSET)
#define VICFG1_PAGE2_V_OFFSET               ( 7 + VICFG_VALS_V_OFFSET)

#define VICFG2_COLOR_SPACE_V_OFFSET     ( 0 + VICFG_VALS_V_OFFSET)
#define VICFG2_EXCH_RB_OUT_V_OFFSET     ( 1 + VICFG_VALS_V_OFFSET)
#define VICFG2_DEBLURMODE_V_OFFSET      ( 2 + VICFG_VALS_V_OFFSET)
#define VICFG2_DEBLURMODE_DEF_V_OFFSET  ( 3 + VICFG_VALS_V_OFFSET)
#define VICFG2_16BIT_V_OFFSET           ( 4 + VICFG_VALS_V_OFFSET)
#define VICFG2_16BIT_DEF_V_OFFSET       ( 5 + VICFG_VALS_V_OFFSET)
#define VICFG2_PAGE1_V_OFFSET           ( 7 + VICFG_VALS_V_OFFSET)

#define VICFG_VSUB_OVERLAY_H_OFFSET   OVERLAY_H_OFFSET
#define VICFG_VSUB_OVERLAY_V_OFFSET   OVERLAY_V_OFFSET_WH
#define VICFG_VSUB_VALS_H_OFFSET      (28 + VICFG_VSUB_OVERLAY_H_OFFSET)
#define VICFG_VSUB_VALS_V_OFFSET      VICFG_VSUB_OVERLAY_V_OFFSET
#define VICFG_VSUB_LINEX_V_OFFSET     ( 0 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_VPLL_V_OFFSET      ( 1 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_FIELDFIX_V_OFFSET  ( 1 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_SL_EN_V_OFFSET     ( 2 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_SL_METHOD_V_OFFSET ( 3 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_SL_ID_V_OFFSET     ( 4 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_SL_STR_V_OFFSET    ( 5 + VICFG_VSUB_OVERLAY_V_OFFSET)
#define VICFG_VSUB_SLHYB_STR_V_OFFSET ( 6 + VICFG_VSUB_OVERLAY_V_OFFSET)

#define VICFG_VPLLSUB_OVERLAY_H_OFFSET  TEXTOVERLAY_H_OFFSET
#define VICFG_VPLLSUB_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define VICFG_VPLLSUB_VALS_H_OFFSET     (26 + VICFG_VPLLSUB_OVERLAY_H_OFFSET)
#define VICFG_VPLLSUB_VALS_V_OFFSET     VICFG_VPLLSUB_OVERLAY_V_OFFSET
#define VICFG_VPLLSUB_TEST_V_OFFSET     ( 4 + VICFG_VPLLSUB_OVERLAY_V_OFFSET)
#define VICFG_VPLLSUB_EN_V_OFFSET       ( 5 + VICFG_VPLLSUB_OVERLAY_V_OFFSET)

#define VICFG_VTIMSUB_OVERLAY_H_OFFSET  TEXTOVERLAY_H_OFFSET
#define VICFG_VTIMSUB_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define VICFG_VTIMSUB_VALS_H_0_OFFSET     (21 + VICFG_VTIMSUB_OVERLAY_H_OFFSET)
#define VICFG_VTIMSUB_VALS_H_1_OFFSET     (31 + VICFG_VTIMSUB_OVERLAY_H_OFFSET)
#define VICFG_VTIMSUB_MODE_V_OFFSET     (0 + VICFG_VTIMSUB_OVERLAY_V_OFFSET)
#define VICFG_VTIMSUB_VSHIFT_V_OFFSET   (2 + VICFG_VTIMSUB_OVERLAY_V_OFFSET)
#define VICFG_VTIMSUB_HSHIFT_V_OFFSET   (3 + VICFG_VTIMSUB_OVERLAY_V_OFFSET)
#define VICFG_VTIMSUB_PALDEJ_V_OFFSET   (4 + VICFG_VTIMSUB_OVERLAY_V_OFFSET)
#define VICFG_VTIMSUB_RESET_V_OFFSET    (6 + VICFG_VTIMSUB_OVERLAY_V_OFFSET)

#define MISC_OVERLAY_H_OFFSET       OVERLAY_H_OFFSET
#define MISC_OVERLAY_V_OFFSET       OVERLAY_V_OFFSET_WH
#define MISC_VALS_H_OFFSET          (23 + MISC_OVERLAY_H_OFFSET)
#define MISC_VALS_V_OFFSET          VICFG_OVERLAY_V_OFFSET
#define MISC_IGR_RESET_V_OFFSET     ( 1 + MISC_VALS_V_OFFSET)
#define MISC_IGR_DEBLUR_V_OFFSET    ( 2 + MISC_VALS_V_OFFSET)
#define MISC_IGR_16BITMODE_V_OFFSET ( 3 + MISC_VALS_V_OFFSET)
#define MISC_FILTERADDON_V_OFFSET   ( 5 + MISC_VALS_V_OFFSET)
#define MISC_SHOWTESTPAT_V_OFFSET   ( 6 + MISC_VALS_V_OFFSET)

#define RWDATA_OVERLAY_H_OFFSET  OVERLAY_H_OFFSET
#define RWDATA_OVERLAY_V_OFFSET  OVERLAY_V_OFFSET_WH
#define RWDATA_SAVE_FL_V_OFFSET  (1 + RWDATA_OVERLAY_V_OFFSET)
#define RWDATA_LOAD_FL_V_OFFSET  (3 + RWDATA_OVERLAY_V_OFFSET)
#define RWDATA_LOAD_JS_V_OFFSET  (4 + RWDATA_OVERLAY_V_OFFSET)
#define RWDATA_LOAD_N64_V_OFFSET (5 + RWDATA_OVERLAY_V_OFFSET)


#define INFO_OVERLAY_H_OFFSET OVERLAY_H_OFFSET
#define INFO_OVERLAY_V_OFFSET OVERLAY_V_OFFSET_WH
#define INFO_VALS_H_OFFSET    (19 + INFO_OVERLAY_H_OFFSET)
#define INFO_VALS_V_OFFSET    INFO_OVERLAY_V_OFFSET

#define INFO_PPU_STATE_V_OFFSET (0 + INFO_VALS_V_OFFSET)
#define INFO_VIN_V_OFFSET       (2 + INFO_VALS_V_OFFSET)
#define INFO_VOUT_V_OFFSET      (3 + INFO_VALS_V_OFFSET)
#define INFO_COL_V_OFFSET       (4 + INFO_VALS_V_OFFSET)
#define INFO_FORMAT_V_OFFSET    (5 + INFO_VALS_V_OFFSET)
#define INFO_DEBLUR_V_OFFSET    (6 + INFO_VALS_V_OFFSET)
#define INFO_FAO_V_OFFSET       (7 + INFO_VALS_V_OFFSET)


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
    "2021 borti4938"; /* 14 chars */

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
    "* PPU Feedback:\n"
    "* Video:\n"
    "  - Input:\n"
    "  - Output:\n"
    "  - Color Depth:\n"
    "  - Format:\n"
    "* LowRes.-DeBlur:\n"
    "* Filter AddOn:";

static const char *vicfg1_header =
    "VI config. 1";
static const char *vicfg1_overlay =
    "* NTSC/PAL awareness:\n"
    "* Linemultiplier:\n"
    "  - 240p settings:\n"
    "  - 480i settings:\n"
    "* V/H position/timing:\n"
    "* Gamma Value:\n\n"
    "* VI config page 2:";

static const char *vicfg2_header =
    "VI config. 2";
static const char *vicfg2_overlay =
    "* Color Space:\n"
    "* Exchange R&B out:\n"
    "* LowRes.-DeBlur:\n"
    "  - powercycle default:\n"
    "* 16bit mode:\n"
    "  - powercycle default:\n\n"
    "* VI config page 1:";

static const char *vicfg_240p_opt_header =
    "Config. (240p)";
static const char *vicfg_240p_opt_overlay =
    "* Enable Linemultiplier:\n"
    "  - Video PLL:\n"
    "* Use Scanlines:\n"
    "  - Method:\n"
    "  - Scanline ID:\n"
    "  - Scanline Strength:\n"
    "  - Hybrid Depth:";

static const char *vicfg_480i_opt_header =
    "Config. (480i)";
static const char *vicfg_480i_opt_overlay =
    "* De-Interlacing (Bob):\n"
    "  - Field-Shift Fix:\n"
    "* Use Scanlines:\n"
    "  - Method:\n"
    "  - Scanline ID:\n"
    "  - Scanline Strength:\n"
    "  - Hybrid Depth:";


static const char *vicfg_vpll_opt_header =
    "Config. (VPLL)";
static const char *vicfg_vpll_opt_overlay =
    "In order to get LineX3 available, you have to have\n"
    "video PLL running and locked. If VPLL lost lock\n"
    "during runtime, you may try to re-enable VPLL here.\n\n"
    "  * Test Video PLL:\n"
    "  * Enable Video PLL:";


static const char *vicfg_timing_opt_header =
    "Config. (Position)";
static const char *vicfg_timing_opt_overlay =
    "* Settings for:";
static const char *vicfg_timing_opt_overlay0v =
    "* Vertical shift:";
static const char *vicfg_timing_opt_overlay0h =
    "* Horizontal shift:";
static const char *vicfg_timing_opt_overlay1 =
    "* DeJitter for PAL LX2*:";
static const char *vicfg_timing_opt_overlay2 =
    "* Reset values:";
  /* 1234567890123456789012345678901234567890123456789012 */



static const char *misc_header =
    "Misc. config.";
static const char *misc_overlay =
    "* In-Game Routines:\n"
    "  - Reset:\n"
    "  - LowRes.-DeBlur:\n"
    "  - 16bit mode:\n"
    "* Filter AddOn:\n"
    "  - Filter Cut-Off:\n"
    "* Show Test-Pattern";

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
    "        Copyright   2015 - 2021 Peter Bartmann\n"
    " This project is published under GNU GPL v3.0 or\n"
    " later. You should have received a copy of the GNU\n"
    " General Public License along with this project.\n"
    " If not, see\n"
    "         <http://www.gnu.org/licenses/>.\n\n"
    " What ever you do, also respect licenses of third\n"
    " party vendors providing the design tools...";
  /* 1234567890123456789012345678901234567890123456789012 */

static const char *welcome_header =
    "Welcome";
static const char *welcome_overlay =
    " Changes coming with latest firmware update:\n\n"
    " * add PPU feedback value to info screen\n"
    " * PAL pattern detection for DeJitter improved\n"
    " * tiny bug fixes:\n"
    "   - VSYNC detection fixed for 4MHz clock domain\n"
    "   - valid data window slightly shifted\n\n"
    " Save config once to not show this screen again.\n"
    " Press B to enter main screen! Have fun! Peter :)";
  /* 1234567890123456789012345678901234567890123456789012 */


static const char *home_header =
    "Main Menu";
static const char *home_overlay =
    "[Video info]\n"
    "[VI configuration]\n"
    "[Miscellaneous config.]\n"
    "[Load/Save]\n\n"
    "About...\n"
    "Acknowledgment...\n"
    "License...";

const char *EnterSubMenu       = "[Enter submenu]";
const char *RunTestPattern     = "[run (alpha)]";
const char *StartVPLLTest      = "[Start VPLL Test]";
const char *LoadTimingDefaults = "[Load defaults]";
const char *LineX3VPLLHint     = "*LineX3: needs VPLL enabled";
const char *LineX3Hint         = "*LineX3: only available in NTSC video mode";
const char *DeJitterHint       = "*experimental feature";
const char *Global             = "Global";
const char *text_480i_576i_br  = "(480i/576i)";
const char *not_available      = "-----";

const char *OffOn[]         = {"Off","On"};
const char *NTSCPAL_SEL[]   = {"NTSC  ","PAL   "};
const char *LineX_240p[]    = {"LineX Off","LineX2","LineX3*"};
const char *VTimingSel[]    = {"Current","NTSC LX1","PAL LX1","NTSC LX2 (2x 240p)","NTSC LX2 (2x 480i)","NTSC LX3 (3x 240p)","PAL LX2 (2x 288p)","PAL LX2 (2x 576i)"};
const char *VideoPLL[]      = {"Off","Locked"};
const char *EvenOdd[]       = {"Even","Odd "};
const char *AdvSL[]         = {"Simple","Advanced"};
const char *LinkSL[]        = {"480i ind.","Link 240p"};
const char *VideoMode[]     = {"240p60","480i60","288p50","576i50","480p60","576p50","720p60 (pseudo)"};
const char *VideoColor[]    = {"21bit","16bit"};
const char *VideoFormat[]   = {"RGBS","RGBS/RGsB","YPbPr"};
const char *FilterAddOn[]   = {"(Auto)"," 9.5MHz","18.0MHz","36.0MHz","(bypassed)","(found)","(not installed)"};

const char *text_240p_288p[]  = {"240p","288p"};
const char *text_480i_576i[]  = {"480i","576i"};

#endif /* MENU_TEXT_TEXTDEFS_P_H_ */
