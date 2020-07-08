# N64 RGB 

A collection of RGB mods for the N64. This repository includes:

- a simple RGB amp
  * based on TI's video amplifier THS7374 / THS7373 (only for some NTSC-units)
  * folder: simpleRGBAmp
- a general RGB digital-to-analog converter based on viletims schematics
  * supported devices: MaxII (EPM240T100C5, EPM570T100C5) and MaxV (5M240ZT100C4, 5M570ZT100C4) CPLDs
  * PCB files (common for all supported devices) and firmware (separate programming files) are provided
  * folder: generalRGBmod
- an advanced RGB digital-to-analog converter with optional YPbPr conversion
  * supported FPGA: Cyclone IV EP4CE10E22 and Cyclone 10 LP 10CL010YE144
  * PCB files (common for all supported devices) and firmware (separate programming files) are provided
  * additional Filter board for the outputted video signal if you need that
  * folder: advancedRGBmod
- additional RCP-NUS Adapter (flexible PCB):
  * buffering of video signals for N64RGB/N64Adv installs with wires
  * PCB files provided
  * folder: misc/RCP2Pads
- additional RCP-NUS to N64RGBv2.1/N64Adv install adapter (long flexible PCB):
  * buffering of video signals for N64RGBv2.1/N64Adv
  * can be directly connected to modding board
  * PCB files provided
  * folder: misc/RCP2N64RGB
- Filter AddOn for N64RGBv2 and N64 Advanced:
  * used to filter the video signal comming out of the ADV7125 or to adjust signal levels such that one can use SNES PAL RGB cables
  * PCB files provided
  * folder: misc/FilterAddOn
- Filter AddOn as flexible PCB for N64RGBv2.1 and N64 Advanced:
  * used to filter the video signal comming out of the ADV7125 or to adjust signal levels such that one can use SNES PAL RGB cables
  * PCB files provided
  * folder: misc/N64RGB2MoutFilter
- additional power supply PCB for viletime N64RGB DAC and N64RGBv1
  * PCB files provided
  * folder: misc/PowerReg


## Acknowledgement

Of course, many thanks to viletim for his initial CPLD-based DAC project, for providing a valuable commercial modding board as well as for publishing the schematics and source codes for that project.
I also want to send many thanks to Ikari_01. He has written the code to detect 480i and PAL/NTSC output with the CPLD. He provides a source code for the XC9572XL, which can be accessed here: [URL to Ikari_01's GitHub repository](https://github.com/mrehkopf/n64rgb)
Last but not least, thank you to Bob for his website [RetroRGB](http://retrorgb.com) and especially in this context here for collecting and sharing all the [RGB information regarding the N64](http://retrorgb.com/n64.html).