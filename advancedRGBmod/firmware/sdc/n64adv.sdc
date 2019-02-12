
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

create_generated_clock -name CLK_4M -source [get_ports {SYS_CLK}] -divide_by 25 -multiply_by 2 [get_nets {sys_pll_u|altpll_component|auto_generated|wire_pll1_clk[0]}]
create_generated_clock -name CLK_16k -source [get_ports {SYS_CLK}] -divide_by 3125 -multiply_by 1 [get_nets {sys_pll_u|altpll_component|auto_generated|wire_pll1_clk[1]}]
create_generated_clock -name CLK_25M -source [get_ports {SYS_CLK}] -divide_by 2 -multiply_by 1 [get_nets {sys_pll_u|altpll_component|auto_generated|wire_pll1_clk[2]}]

create_generated_clock -name VCLK_50M -source [get_ports {VCLK}] -divide_by 1 -multiply_by 1 -duty_cycle 75 [get_nets {video_pll_u|altpll_component|auto_generated|wire_pll1_clk[0]}]
create_generated_clock -name VCLK_75M -source [get_ports {VCLK}] -divide_by 2 -multiply_by 3 -duty_cycle 75 [get_nets {video_pll_u|altpll_component|auto_generated|wire_pll1_clk[1]}]


create_generated_clock -master_clock CLK_25M -source [get_nets {sys_pll_u|altpll_component|auto_generated|wire_pll1_clk[2]}] -divide_by 1 -multiply_by 1 -name ALTERA_DCLK [get_ports {*ALTERA_DCLK}]


create_generated_clock -master_clock VCLK -source [get_ports {VCLK}] -multiply_by 1 -duty_cycle 75 -name VCLK_1x_out_pre [get_nets {n64adv_ppu_u|linemult_u|vclk_tx_mux_u|LPM_MUX_component|auto_generated|result_node[0]}]
create_generated_clock -master_clock VCLK_50M -source [get_nets {video_pll_u|altpll_component|auto_generated|wire_pll1_clk[0]}] -multiply_by 1 -duty_cycle 75 -name VCLK_2x_out_pre [get_nets {n64adv_ppu_u|linemult_u|vclk_tx_mux_u|LPM_MUX_component|auto_generated|result_node[0]}] -add
create_generated_clock -master_clock VCLK_75M -source [get_nets {video_pll_u|altpll_component|auto_generated|wire_pll1_clk[1]}] -multiply_by 1 -duty_cycle 75 -name VCLK_3x_out_pre [get_nets {n64adv_ppu_u|linemult_u|vclk_tx_mux_u|LPM_MUX_component|auto_generated|result_node[0]}] -add

create_generated_clock -master_clock VCLK -source [get_ports {VCLK}] -multiply_by 1 -duty_cycle 75 -name VCLK_1x_testpattern_o [get_ports CLK_ADV712x]
create_generated_clock -master_clock VCLK_1x_out_pre -source [get_nets {n64adv_ppu_u|vclk_tx_post_testpattern_mux_u|LPM_MUX_component|auto_generated|result_node[0]}] -multiply_by 1 -duty_cycle 75 -name VCLK_1x_out [get_ports CLK_ADV712x] -add
create_generated_clock -master_clock VCLK_2x_out_pre -source [get_nets {n64adv_ppu_u|vclk_tx_post_testpattern_mux_u|LPM_MUX_component|auto_generated|result_node[0]}] -multiply_by 1 -duty_cycle 75 -name VCLK_2x_out [get_ports CLK_ADV712x] -add
create_generated_clock -master_clock VCLK_3x_out_pre -source [get_nets {n64adv_ppu_u|vclk_tx_post_testpattern_mux_u|LPM_MUX_component|auto_generated|result_node[0]}] -multiply_by 1 -duty_cycle 75 -name VCLK_3x_out [get_ports CLK_ADV712x] -add


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

set_input_delay -clock { VCLK } -min 0.0 [get_ports {nVDSYNC}]
set_input_delay -clock { VCLK } -max 6.5 [get_ports {nVDSYNC}]
set_input_delay -clock { VCLK } -min 0.0 [get_ports {VD_i[*]}]
set_input_delay -clock { VCLK } -max 6.5  [get_ports {VD_i[*]}]

set_input_delay -clock { CLK_25M } 0 [get_ports *ALTERA_DATA0]

set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tms]


#**************************************************************
# Set Output Delay
#**************************************************************

set_output_delay -clock { VCLK_1x_testpattern_o } -max 0.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_1x_testpattern_o } -min -1.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_1x_out } -max 0.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_1x_out } -min -1.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_2x_out } -max 0.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_2x_out } -min -1.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_3x_out } -max 0.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock { VCLK_3x_out } -min -1.5 [get_ports {VD_o* nCSYNC_ADV712x}] -add

#set_output_delay -clock { VCLK_1x_testpattern_o } 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add
#set_output_delay -clock { VCLK_1x_out } 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add
#set_output_delay -clock { VCLK_2x_out } 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add
#set_output_delay -clock { VCLK_3x_out } 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add

set_output_delay -clock { ALTERA_DCLK } 0 [get_ports {*ALTERA_SCE *ALTERA_SDO}]

set_output_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdo]


#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -asynchronous -group \
                            {VCLK VCLK_1x_testpattern_o VCLK_1x_out_pre VCLK_1x_out} \
                            {VCLK_50M VCLK_2x_out_pre VCLK_2x_out} \
                            {VCLK_75M VCLK_3x_out_pre VCLK_3x_out} \
                            SYS_CLK \
                            {CLK_4M CLK_16k CLK_25M}


#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_ports {nRST CTRL_i UseVGA_HVSync nFilterBypass nEN_RGsB nEN_YPbPr SL_str* n240p n480i_bob}]
set_false_path -from [get_registers {n64adv_controller_u|use_igr n64adv_controller_u|OSDInfo[*] n64adv_controller_u|OutConfigSet[*] \
                                     n64adv_ppu_u|get_vinfo_u|FrameID n64adv_ppu_u|get_vinfo_u|n64_480i n64adv_ppu_u|get_vinfo_u|line_cnt[*] n64adv_ppu_u|get_vinfo_u|vmode \
                                     n64adv_ppu_u|deblur_management_u|nblur_n64* n64adv_ppu_u|deblur_management_u|ndo_deblur \
                                     n64adv_ppu_u|linemult_u|FrameID n64adv_ppu_u|linemult_u|SL_rval[*]}]
set_false_path -to [get_ports {nRST}]
set_false_path -to [get_ports {nHSYNC* nVSYNC* nCSYNC}]


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

