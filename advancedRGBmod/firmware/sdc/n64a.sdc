## Generated SDC file "n64advanced.sdc"

## Copyright (C) 2017  Intel Corporation. All rights reserved.
## Your use of Intel Corporation's design tools, logic functions 
## and other software and tools, and its AMPP partner logic 
## functions, and any output files from any of the foregoing 
## (including device programming or simulation files), and any 
## associated documentation or information are expressly subject 
## to the terms and conditions of the Intel Program License 
## Subscription Agreement, the Intel Quartus Prime License Agreement,
## the Intel MegaCore Function License Agreement, or other 
## applicable license agreement, including, without limitation, 
## that your use is for the sole purpose of programming logic 
## devices manufactured by Intel and sold by Intel or its 
## authorized distributors.  Please refer to the applicable 
## agreement for further details.


## VENDOR  "Altera"
## PROGRAM "Quartus Prime"
## VERSION "Version 17.0.1 Build 598 06/07/2017 SJ Lite Edition"

## DATE    "Tue Jul 04 21:09:02 2017"

##
## DEVICE  "10M02SCE144C8G"
##


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {VCLK} -period 20.000 -waveform { 0.000 15.000 } [get_ports { VCLK }]
create_clock -name {SYS_CLK} -period 20.000 -waveform { 0.000 10.000 } [get_ports { SYS_CLK }]


#**************************************************************
# Create Generated Clock
#**************************************************************

create_generated_clock -name CLK_4M -source [get_ports {SYS_CLK}] -divide_by 25 -multiply_by 2 [get_nets {controller_u|sys_pll|altpll_component|auto_generated|wire_pll1_clk[0]}]
create_generated_clock -name CLK_16k -source [get_ports {SYS_CLK}] -divide_by 3125 -multiply_by 1 [get_nets {controller_u|sys_pll|altpll_component|auto_generated|wire_pll1_clk[1]}]
create_generated_clock -name CLK_25M -source [get_ports {SYS_CLK}] -divide_by 2 -multiply_by 1 [get_nets {controller_u|sys_pll|altpll_component|auto_generated|wire_pll1_clk[2]}]

create_generated_clock -name CLK_ADV712x -source [get_ports {VCLK}] -divide_by 1 -multiply_by 1 [get_ports {CLK_ADV712x}]

create_generated_clock -name ALTERA_DCLK -source [get_nets {controller_u|sys_pll|altpll_component|auto_generated|wire_pll1_clk[2]}] -divide_by 1 -multiply_by 1 [get_ports {*ALTERA_DCLK}]


#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************

derive_clock_uncertainty


#**************************************************************
# Set Input Delay
#**************************************************************

set_input_delay -clock { VCLK } -min 0.0 [get_ports {nDSYNC}]
set_input_delay -clock { VCLK } -max 6.5 [get_ports {nDSYNC}]
set_input_delay -clock { VCLK } -min 0.0 [get_ports {D_i[*]}]
set_input_delay -clock { VCLK } -max 6.5  [get_ports {D_i[*]}]

set_input_delay -clock { CLK_25M } 0 [get_ports *ALTERA_DATA0]

set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tms]


#**************************************************************
# Set Output Delay
#**************************************************************

set_output_delay -clock { CLK_ADV712x } -max 0.5 [get_ports {V1_o* V2_o* V3_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { CLK_ADV712x } -min -1.5 [get_ports {V1_o* V2_o* V3_o* nCSYNC_ADV712x}] -add

set_output_delay -clock { VCLK } 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add

set_output_delay -clock { ALTERA_DCLK } 0 [get_ports {*ALTERA_SCE *ALTERA_SDO}]

set_output_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdo]


#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -asynchronous -group \
                            {VCLK CLK_ADV712x} \
                            {SYS_CLK CLK_4M CLK_16k CLK_25M}


#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_ports {nRST CTRL_i UseVGA_HVSync nFilterBypass nEN_RGsB nEN_YPbPr SL_str* n240p n480i_bob}]
set_false_path -from [get_registers {controller_u|show_* controller_u|OutConfigSet[*]}]
set_false_path -to [get_ports {nRST SYS_CLKen}]


#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

