


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

create_clock -name SCLK_0 -period 37.037 -waveform { 0.000 18.519 } [get_ports {SCLK_0}]
create_clock -name SCLK_1 -period 37.037 -waveform { 0.000 18.519 } [get_ports {SCLK_1}]
create_clock -name VCLK_0 -period 20.000 -waveform { 0.000 15.000 } [get_ports {VCLK_0}]
create_clock -name VCLK_1 -period 20.000 -waveform { 0.000 15.000 } [get_ports {VCLK_1}]


#**************************************************************
# Create Generated Clock
#**************************************************************

create_generated_clock -name CLK_4M -source [get_ports {SCLK_0}] -divide_by 27 -multiply_by 4 [get_nets {pll4ctrl_u|altpll_component|auto_generated|wire_pll1_clk[0]}]
create_generated_clock -name CLK_16k -source [get_ports {SCLK_0}] -divide_by 3375 -multiply_by 2 [get_nets {pll4ctrl_u|altpll_component|auto_generated|wire_pll1_clk[1]}]
create_generated_clock -name CLK_25M -source [get_ports {SCLK_0}] -divide_by 27 -multiply_by 25 [get_nets {pll4ctrl_u|altpll_component|auto_generated|wire_pll1_clk[2]}]
create_generated_clock -name VCLK_Tx -source [get_ports {VCLK_1}] -divide_by 1 -multiply_by 1 [get_nets {pll4video_u|altpll_component|auto_generated|wire_pll1_clk[1]}]
create_generated_clock -name AMCLK -source [get_ports {SCLK_1}] -divide_by 78 -multiply_by 71 [get_nets {pll4audio_u|altpll_component|auto_generated|wire_pll1_clk[0]}]

create_generated_clock -master_clock CLK_25M -source [get_nets {pll4ctrl_u|altpll_component|auto_generated|wire_pll1_clk[2]}] -divide_by 1 -multiply_by 1 -name ALTERA_DCLK [get_ports {*ALTERA_DCLK}]
create_generated_clock -master_clock VCLK_Tx -source [get_nets {pll4video_u|altpll_component|auto_generated|wire_pll1_clk[1]}] -divide_by 1 -multiply_by 1 -name VCLK_o [get_ports {VCLK_o}]
create_generated_clock -master_clock AMCLK -source [get_nets {pll4audio_u|altpll_component|auto_generated|wire_pll1_clk[0]}] -divide_by 1 -multiply_by 1 -name AMCLK_o [get_ports {AMCLK_o}]


#**************************************************************
# Set Clock Uncertainty
#**************************************************************

derive_clock_uncertainty


#**************************************************************
# Set Input Delay
#**************************************************************

set_input_delay -clock { VCLK_1 } -min 0.0 [get_ports {nVDSYNC}]
set_input_delay -clock { VCLK_1 } -max 6.5 [get_ports {nVDSYNC}]
set_input_delay -clock { VCLK_1 } -min 0.0 [get_ports {VD_i[*]}]
set_input_delay -clock { VCLK_1 } -max 6.5  [get_ports {VD_i[*]}]

set_input_delay -clock { CLK_25M } 0 [get_ports *ALTERA_DATA0]

set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tms]


#**************************************************************
# Set Output Delay
#**************************************************************

set_output_delay -clock { VCLK_o } 0 [get_ports {VCLK_o}] -add
set_output_delay -clock { VCLK_o } -max  1.8 [get_ports {VDE_o VSYNC_o HSYNC_o VD_o*}] -add
set_output_delay -clock { VCLK_o } -min -1.3 [get_ports {VDE_o VSYNC_o HSYNC_o VD_o*}] -add

set_output_delay -clock { AMCLK_o } -max  2 [get_ports {ASCLK_o ASDATA_o ALRCLK_o}] -add
set_output_delay -clock { AMCLK_o } -min -2 [get_ports {ASCLK_o ASDATA_o ALRCLK_o}] -add

set_output_delay -clock { ALTERA_DCLK } 0 [get_ports {*ALTERA_SCE *ALTERA_SDO}]

set_output_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdo]


#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -asynchronous -group \
                            {SCLK_0 CLK_4M CLK_16k CLK_25M} \
                            {VCLK_0 VCLK_1} \
                            {VCLK_Tx VCLK_o} \
                            {SCLK_1} \
                            {AMCLK AMCLK_o}
set_clock_groups -group VCLK_Tx VCLK_o -logically_exclusive
set_clock_groups -group AMCLK AMCLK_o -logically_exclusive
set_clock_groups -group {CLK_4M CLK_25M} CLK_16k -logically_exclusive


#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_ports {nRST CTRL_i ASCLK_i ASDATA_i ALRCLK_i INT_ADV7513 I2C_SCL I2C_SDA}]
set_false_path -from [get_registers {n64adv2_controller_u|use_igr n64adv2_controller_u|OutConfigSet[*] n64adv2_controller_u|OSDInfo[*]\
                                     n64adv2_ppu_u|get_vinfo_u|FrameID n64adv2_ppu_u|get_vinfo_u|n64_480i n64adv2_ppu_u|get_vinfo_u|line_cnt[*] n64adv2_ppu_u|get_vinfo_u|vmode \
                                     n64adv2_ppu_u|deblur_management_u|nblur_n64* n64adv2_ppu_u|deblur_management_u|ndo_deblur \
                                     n64adv2_ppu_u|scaler_u|FrameID n64adv2_ppu_u|scaler_u|SL_rval[*]}]
set_false_path -to [get_ports {nRST I2C_SCL I2C_SDA LED_0 LED_1 ExtData[*]}]
