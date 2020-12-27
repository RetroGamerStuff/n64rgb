# Advanced N64 RGB Digital-to-Analog Mod

This folder contains all you need for a complete DIY RGB mod.

Please don't ask me for selling a modding. I either sell some prototypes on some forums marketplaces (which is very unlikely) or I don't have any of the boards.
This is a complete DIY modding project. So everybody is on his own here.

**WARNING:** This is an advanced DIY project if you do everything on your own. You need decent soldering skills. The FPGA has 0.5mm fine pitch with 144pins and one exposed pad below the IC, which has to be soldered to the PCB. Next to it the video DAC has also 0.5mm pin pitch on the board there are some SMD1206 resistor and ferrit bead arrays.

## Features

- Supporting for different FPGAs [1] on a common PCB design:
  * Cyclone IV EP4CE10E22
  * Cyclone 10 LP 10CL010YE144
- Video DAC ADV7125 (or ADV7123)
- Detection of 240p/288p vs. 480i/576i together with detection of NTSC vs. PAL mode
- VI-DeBlur in 240p/288p (horizontal resolution decreased from 640 to 320 pixels)
- 15bit color mode
- IGR features:
  * reset the console with the controller
  * full control on de-blur and 15bit mode with the controller
- Advanced features:
  * output of RGsB or YPBPr on demand
  * linedoubling of 240p/288p video to 480p/576p
  * linetrippling of 240p to (pseudo) 720p (NTSC content only, generated resolution is not a standard resolution so compatibility highly depends on your TV)
  * Bob de-interlace of 480i/576i to 480p/576p (with optional field shift fix (reduces vertical flicker))
  * configurable hybrid scanlines
  * Dejitter for PAL in LineX2 mode [2]
  * possible VGA output [3]
  * on-screen menu for configuration


The following shortly describes the main features of the firmware and how to use / control them.

#### Notes
##### [1]
For now, support for 6k LEs FPGAs (EP4CE6E22 and 10CL006YE144) has been discontinued. They are to small for an OSD menu implementation due to the small amount of block RAM. I will try to get it fit in future.

##### [2]

Experimental feature which might not be compatible with all PAL games. Please report incompatibilities on GitHub!

##### [3]

Not available if the filter adddon is used as HSYNC and VSYNC are shared outputs with F1 and F2 (filter selection)

### In-Game Routines (IGR)

Three functunalities are implemented: toggle de-blur feature / override heuristic for de-blur and toggle the 15bit mode (see above) as well as resetting the console.

The button combination are as follows:

- open on-screen menu: D-pad ri + L + R + C-ri
- reset the console: Z + Start + R + A + B (must be enabled in menu)
- (de)activate vi-deblur: (must be enabled in menu, see description above)
  - deactivate: Z + Start + R + C-le
  - activate: Z + Start + R + C-ri
- (de)activate 15bit mode: (must be enabled in menu, see description above)
  - deactivate: Z + Start + R + C-up
  - activate: Z + Start + R + C-dw

_Final remark on IGR_:  
However, as the communication between N64 and the controller goes over a single wire, sniffing the input is not an easy task (and probably my solution is not the best one). This together with the lack of an exhaustive testing (many many games out there as well my limited time), I'm looking forward to any incomming issue report to furhter improve this feature :)


### VI-DeBlur

VI-DeBlur of the picture information is only performed in 240p/288p. This is be done by simply blanking every second pixel. Normally, the blanked pixels are used to introduce blur by the N64 in 240p/288p mode. However, some games like Mario Tennis, 007 Goldeneye, and some others use these pixel for additional information rather than for blurring effects. In other words this means that these games uses full horizontal resolution even in 240p/288p output mode. Hence, the picture looks more blurry in this case if vi-deblur feature is activated.

The default state (whether vi-deblur is enabled for 240p/288p content or not) can be set in the menu.


### 15bit Color Mode

The 15bit color mode reduces the color depth from 21bit (7bit for each color) downto 15bits (5bit for each color). Some very few games just use the five MSBs of the color information and the two LSBs for some kind of gamma dither. The 15bit color mode simply sets the two LSBs to '0'.

- By default the 15bit mode is *off*! The default is set on each power cycle but not on a reset.
- to deactivate 15bit mode press Z + Start + R + C-up. (quick access function must be enabled in menu)
- to (re)activate 15bit mode press Z + Start + R + C-dw. (quick access function must be enabled in menu)


### Low Pass Filtering of the Video Output

The video DAC does not use any filtering. In general, this is also not needed. If you need DAC post-filtering though (receiver has a ADC build without any pre-filtering), you can install the filter addon (also provided in this repository).

The filter addon is based on a THS7368, where the video channels with selectable filter are used. As the filter is selected over the same pins as HSYNC and VSYNC, the addon is not compatible to VGA.


## Technical Information

A complete installation and setup guide to this modding kit is provided in the main folder of the repository. Here are some additional short notes.

### Checklist: How to build the project

- Use PCB files to order your own PCB or simply use the shared project(s) on OSHPark
- Source the components you need, e.g. from Mouser or Digikey
- Wait for everything to arrive
- Assemble your PCB
- Set all jumpers
- Flash the firmware to the serial flash device either in advanced (e.g. using the MiniPro) or after installation (e.g. using an Altera USB Blaster)
  * In advanced:
    * use the n64adv\__fpga-device_\_spi.bin binary
    * If your programmer does not support the serial flash  device on the N64Adv board (currently IS25LP016D), you may setup the flash program for another compatible device (e.g. A25L016 @SOP8). You may have to uncheck _Check ID_ options (or similar)
  * After installing:
    * Use the JIC programming file named n64adv\__fpga-device_.jic
    * Using the IntelFPGA programmer, JTAG chain is initialized by loading JIC file
    * N64 needs to be powered for flashing
    * Power cycle the N64 after flashing

### Jumpers

**IMPORTANT NOTE**  
Most jumpers are used as fallback if the software is unable to load a valid configuration from flash device U5. They were introduced on the modding board before a menu was in the firmware.  
The only exception is J1.2 - this jumper is still used to show the software whether the _Filter AddOn_ is installed or not.

There are some jumpers spreaded over the PCB, namely _J1_, _J2_, _J3_, _J4_, _J5_ and _J6_. _J1 - J4_ have to parts, let's say _.1_ and _.2_, where _.1_ is marked with the _dot_ on the PCB.

#### J1 (Filter AddOn)
##### J1.1 
- opened: output HSYNC and VSYNC for possible VGA output
- closed: use filter addon

##### J1.2
- opened: use filter of the addon board
- closed: bypass filter (actually filter is set to 95MHz cut-off which is way above the video signal content)

#### J2 (RGB, RGsB, YPbPr)
##### J2.1
- opened: RGB output
- closed: RGsB output (sync on green)

##### J2.2
- opened: RGB / RGsB output
- closed: YPbPr output (beats J2.1)


#### J3 (Scanline)
##### J3.1/J3.2
- opened / opened: 0%
- opened / closed: 25%
- closed / opened: 50%
- closed / closed: 100%

#### J33 (CSYNC level @ _/CS (75ohm)_ pad)

- opened: appr. 1.87V @ 75ohm termination i.e. needs a resistor inside the sync wire further attenuating the signal. Designed to work for cables with 470 ohm resistor inside resulting in appr. 450mV @ 75ohm termination
- closed: appr. 300mV @ 75ohm termination suitable for pass through wired cables at sync, works with standard TV / scaler setup

#### J4 (Linemode)
##### J4.1
- opened: linedoubling of 240p/288p to 480p/576p
- closed: no-linedoubling (beats J4.2)

##### J4.2
- opened: no bob de-interlace of 480i/576i
- closed: bob de-interlace of 480i/576i to 480p/576p (J4.1 must be opened)

#### J5
_J5_ is the JTAG connector.

#### J6 (_Outdated_; Power supply of analog outputs)
The analog part can be power with **either** 3.3V **or** 5V. If you want to power this part of the PCB with 3.3V, close _J6_ and leave pad _5V_ unconnected. If you want to power this part with 5V, leave _J6_ opened and connect pad _5V_ to +5V power rail of the N64. **NEVER connect 5V and close J6.**

#### Fallback Mode

A fallback mode can be activated by holding reset button down while powering on the N64. In the fallback mode **linedoubling and component conversion is turned off**. Sync on G/Y output is still on if one J2.1 or J2.2 is set as it does not affect RGBS.



### Source the PCB

Choose the PCB service which suits you. Here are some:

- OSHPark: [Link to the Main PCB](https://oshpark.com/shared_projects/Xo6yoQwD) (If the PCB was updated and I forgot to update this link, look onto [my profile](https://oshpark.com/profiles/borti4938))
- PCBWay.com: [Link](http://www.pcbway.com/), [Affiliate Link](http://www.pcbway.com/setinvite.aspx?inviteid=10658)

### Part List for the PCB

This part list a recommendation how the PCB is designed. If you are able to find pin-compatible devices / parts, you can use them.

#### Components

Components are listed in advancedRGBmod/Main-PCB/BOM_n64advanced.xlsx


### Firmware
The firmware is located in the folder firmware/. To build the firmware on your own you need Quartus Prime Lite (any version which supports Cyclone IV and 10 LP devices).

If you only want to flash the firmware, the configuration files are pre-compiled and located in firmware/output_files. You need the JIC appropriate for the FPGA you opted for (look for the *\_extension*).

For flashing you need:

- Altera USB Blaster
- Standalone Quartus Prime Programmer and Tools

#### Firmware Revision Numbering

- 1.0: initially commited version
- 2.0 (HDL) / 1.0 (SW): initial version with OSD driven menu
- HDL got a fallback to v. 1.xx from 2.0 after a while

#### Road Map / New Ideas

- return of vi-deblur heuristic
- test screen
- some more menu stuff


Any other ideas: email me :)