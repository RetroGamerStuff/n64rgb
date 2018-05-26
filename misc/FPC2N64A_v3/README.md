# A PCB adapter
## FPC cable connector to N64Advanced


## Purpose

This PCB enables you to connect your N64Advanced modding kit with a FPC cable.

## Pinout of version 3

From top to bottom (N64A main on right)

| **Pin** | **Signal** |
|:-------:|:-----------|
| 24 | Rst |
| 23 | Ctrl. |
| 22 | GND |
| 21 | n.c. |
| 20 | GND |
| 19 | n.c. |
| 18 | n.c. |
| 17 | GND |
| 16 | VCLK |
| 15 | GND |
| 14 | #DSYNC |
| 13 | GND |
| 12 | D6 |
| 11 | D5 |
| 10 | D4 |
| 9 | D3 |
| 8 | D2 |
| 7 | D1 |
| 6 | D0 |
| 5 | GND |
| 4 | GND |
| 3 | 3.3V |
| 2 | 3.3V |
| 1 | 3.3V |

## Preparation

You need

- one FPC cable connector: 24 pin, 0.5mm pitch
- one FPC cable 24pin with 0.5mm pitch of length 15cm with connectors on same side 

Assemble the board; orientation of the connector should be clear.
Make sure that you don't have a short at the connector.

## Installation

Solder the adapter board to the data inputs of the N64A modding board.
You only have small vias available. So make sure 
 
- to allign the board prior to set the first solder joint
- to make good solder joints (the solder has to flow through the connection vias down to the pads)

Connect the pads for the Controller and Reset to the pads at the pads on the modding board. You can also make a short connection to the neighbored resistor array (pin 1 and 3 of RN12).

Finally, connect the modding board to the installed RCP2FPC_v3 board.



