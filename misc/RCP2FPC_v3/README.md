# A small Flex-PCB
## N64 RCP-NUS -> to 24pin 0.5mm pitch FPC cable connector


## Purpose

This PCB enables you to connect your N64Advanced Version 2 (HDMI) modding kit with a FPC cable.

## Pinout of version 3

From top to bottom (RCP on left, connector right)

| **Pin** | **Signal** |
|:-------:|:-----------|
| 24 | 3.3V |
| 23 | 3.3V |
| 22 | 3.3V |
| 21 | GND |
| 20 | GND |
| 19 | D0 |
| 18 | D1 |
| 17 | D2 |
| 16 | D3 |
| 15 | D4 |
| 14 | D5 |
| 13 | D6 |
| 12 | GND |
| 11 | #DSYNC |
| 10 | GND |
| 9 | VCLK |
| 8 | GND |
| 7 | LRCLK (Audio) |
| 6 | SDATA (Audio) |
| 5 | GND |
| 4 | SCLK (Audio) |
| 3 | GND |
| 2 | Ctrl. (controller port pin 2, pin 16 PIF-NUS) |
| 1 | Rst (pin 27 PIF-NUS) |


## Preparation

You need

- four resistor arrays: 4x47ohm parallel, SMD1206, e.g. CAT16-47R0F4LF or CAY16-47R0F4LF
- one FPC cable connector: 24 pin, 0.5mm pitch

Assemble the board. The resistor arrays don't have an orientation, the connector should be clear.
Make sure that you don't have a short at the connector.

## Installation

Solder the FPC board to the output pins 6 - 28 (pin 5 has a dot, pin 30 also) of the RCP-NUS.

Connect pad for Ctrl. and Rst. as stated in the table above. More information about appropriate solder points can be found in the Guide.

