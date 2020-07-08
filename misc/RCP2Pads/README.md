# A small Flex-PCB
## N64 RCP-NUS breakout

## Purpose

This PCB enables you to apply appropriate buffering to the RCP output pins for N64RGB installs. This is useful for any CPLD and FPGA RGB Kit like viletims N64RGB (public and commercial), N64RGBv2.1, N64Advanced and many more variations spreaded over the internet.


## Where to Order?

This board can be ordered from every service you want to choose, e.g. PCBWay.  
An easy small prototype solution though might be the flex cable service by [OSHPark](https://oshpark.com/).
Make sure to tick the flex service before going to the checkout when chosing for OSHPark!


## Pinout

From top to bottom (RCP on left, connector right)

| **Pin** | **Signal** |
|:-------:|:-----------|
| 13 | GND |
| 12 | D0 |
| 11 | D1 |
| 10 | D2 |
| 9 | D3 |
| 8 | D4 |
| 7 | D5 |
| 6 | D6 |
| 5 | #DSYNC |
| 4 | VCLK |
| 3 | GND |
| 2 | Ctrl. (controller port pin 2, pin 16 PIF-NUS) |
| 1 | Rst (pin 27 PIF-NUS) |


## Preparation

You need components as listed in BOM.
Assemble the board.
The ferrite bead / resistor arrays don't have an orientation.


## Installation

Solder the FPC board to the output pins 8 - 28 (pin 5 has a dot, pin 30 also) of the RCP-NUS.

Connect pad for Ctrl. and Rst. as stated in the table above. More information about appropriate solder points can be found in the Guide.

Use the pads to connect your modding kit. Note the pinout of the modding kit. If possible, keep/run VCLK separated from data wires.