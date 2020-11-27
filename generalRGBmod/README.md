# General N64 RGB Digital-to-Analog Mod

This folder contains all you need for a complete DIY RGB mod.

Please don't ask me for selling a modding. I either sell some prototypes on some forums marketplaces (which is very unlikely) or I don't have any of the boards.
This is a complete DIY modding project. So everybody is on his own here.
If you are looking for a ready to install kit, just look on your own for a seller. Preferably you should invest your money into a [kit made by viletim](http://etim.net.au/shop/shop.php?crn=209). I also provide similar firmware files for his board in another GitHub repository named [n64rgb_fw4viletim](https://github.com/borti4938/n64rgb_fw4viletim) (note that I do not provide support for the firmware port there).

**WARNING:** This is an advanced DIY project if you do everything on your own. You need decent soldering skills. The CPLD has 0.5mm fine pitch with 100pins. Next to it the video amp has a 0.65mm pin pitch on the board there are some SMD1206 resistor and ferrit bead arrays.

## Features

- Supporting for different CPLDs on a common PCB design:
  * MaxII EPM240T100C5
  * MaxII EPM570T100C5
  * MaxV 5M240ZT100C5
  * MaxV 5M570ZT100C5
- Video DAC:
  * V2: Video DAC ADV7125
- Detection of 240p/288p vs. 480i/576i together with detection of NTSC vs. PAL mode
- optional VI-De-Blur in 240p/288p (horizontal resolution decreased from 640 to 320 pixels)
- optional 15bit color mode
- IGR features:
  * reset the console with the controller (can be disabled via jumper)
  * full control on de-blur and 15bit mode with the controller (can be disabled via jumper)
- optional dedicated mechanical switches for toggling VI-Deblur and 15bit color mode


The following shortly describes the main features of the firmware and how to use / control them.

#### Notes


### In-Game Routines (IGR)

Three functionalities are implemented: toggle de-blur feature and toggle the 15bit mode as well as resetting the console.

The button combination are as follows:

- reset the console: Z + Start + R + A + B
- (de)activate de-blur:
  - deactivate: Z + Start + R + C-le
  - activate: Z + Start + R + C-ri
- (de)activate 15bit mode:
  - deactivate: Z + Start + R + C-up
  - activate: Z + Start + R + C-dw

_Modifiying the IGR Button Combinations_:  
It's difficult to make everybody happy with it. Third party controllers, which differ from the original ones by design, make it even more difficult. So it is possible to generate your own firmware with **your own** preferred **button combinations** implemented. Please refere to the document **IGR.README.md** located in the top folder of this repository for further information.

_Final remark on IGR_:  
However, as the communication between N64 and the controller goes over a single wire, sniffing the input is not an easy task (and probably my solution is not the best one). This together with the lack of an exhaustive testing (many many games out there as well my limited time), I'm looking forward to any incomming issue report to furhter improve this feature :)


### VI-DeBlur

VI-Deblur of the picture information is only be done in 240p/288p. This is be done by simply blanking every second pixel. Normally, the blanked pixels are used to introduce blur by the N64 in 240p/288p mode. However, some games like Mario Tennis, 007 Goldeneye, and some others use these pixel for additional information rather than for blurring effects. In other words this means that these games uses full horizontal resolution even in 240p/288p output mode. Hence, the picture looks more blurry in this case if de-blur feature is activated.


### 15bit Color Mode

The 15bit color mode reduces the color depth from 21bit (7bit for each color) down to 15bits (5bit for each color). Some very few games just use the five MSBs of the color information and the two LSBs for some kind of gamma dither. The 15bit color mode simply sets the two LSBs to '0'.

### Jumpers

There are five jumpers on the modding board - two double solder jumper and three standard jumper.  
For double jumper, jumper Jxx.1 is indicated by an arrow, Jxx.2 is on the opposite side.

- J11.1 -> activates VI-deblur if closed. **[1]**
- J11.2 -> activates 15bit color mode **[1]**
- J12.x -> disables IGR functions if closed
  - x.1: VI-DeBlur and 15bit mode toggle by controller
  - x.2: reset by controller
- J21 -> enables Sync on Green if closed
- J31 -> 75ohm CSync Level
  - open: ~1.87Vpp at a 75ohm termination, i.e. you need an additional resistor in your RGB cables sync line to further attenuate voltage level
  - closed: ~300mVpp at a 75ohm termination, which is standard, i.e. you must not have any components attached to your RGB cables sync line
- J2: toggles video low pass filter of the Filter AddOn
  - open: enabled
  - closed: bypassed 

**Notes**:  

**[1]**  
Jumper becomes active on power cycle or if toggled while running operation.  

## Technical Information

A complete installation and setup guide to this modding kit is provided in the main folder of the repository. Here are some additional short notes.

### Checklist: How to build the project

- Use PCB files to order your own PCB or simply use the shared project on OSHPark
- Source the components you need, e.g. from Mouser
- Wait for everything to arrive
- Assemble your PCB:
  * If you use a MaxII CPLD, you have to assemble FB3 and must not use U4, C41 and C42.
  * If you use a MaxV CPLD, you need U4 (a 1.8V voltage regulator), C41 and C42. Don't touch FB3 in this case!
- Flash the firmware to the CPLD:
  * You need a Altera USB Blaster
  * The board needs to be powered; so you may consider to install the PCB into your N64 first and then use the N64 for powering the board
  * If you want to build an adapter, you may take a look onto [my DIY adapter](https://oshpark.com/shared_projects/mEwjoesz) at [my profile on OSHPark](https://oshpark.com/profiles/borti4938)
- Install the modding board:
  * Installation description is part of the guide located in the top folder.
  * However an installation guide of a similar product made by viletim is provided [here](http://etim.net.au/n64rgb/). Keep in mind that there are minor differences.
  * You have to be aware of the pinout of your video-encoder build into your N64. Pads on the DIY modding board are labeled.

### Source the PCB
Choose the PCB service which suits you. Here are some:

- OSHPark: look onto [my profile](https://oshpark.com/profiles/borti4938)
- PCBWay.com: [Link](http://www.pcbway.com/), [Affiliate Link](http://www.pcbway.com/setinvite.aspx?inviteid=10658)

### BOM / Part List for the PCB
This part list a recommendation how the PCB is designed. If you are able to find pin-compatible devices / parts, you can use them.

The manufacturer's part numbers (MPNs) are just to help you to source appropriate components.

### Firmware
The firmware is located in the folder firmware/. To build the firmware on your own you need Quartus Prime Lite (any version which supports MaxII and MaxV devices).

If you only want to flash the firmware, the configuration files are pre-compiled and located in firmware/output_files. You need the POF appropriate for the CPLD you have choosen (look for the *\_extension*).

For flashing you need:

- Altera USB Blaster
- Standalone Quartus Prime Programmer and Tools


