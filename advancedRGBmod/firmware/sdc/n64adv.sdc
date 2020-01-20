##################################################################################
##
## This file is part of the N64 RGB/YPbPr DAC project.
##
## Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmx.de>
##
## N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
##################################################################################
##
## Company:  Circuit-Board.de
## Engineer: borti4938
##
##################################################################################

#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

set vclk_input [get_ports {VCLK}]
set n64_vclk_per 20.000
set n64_vclk_waveform [list 0.000 [expr $n64_vclk_per*3/4]]
set sys_clk_per 20.000
set sys_clk_waveform [list 0.000 [expr $sys_clk_per/2]] 

# N64 base clock
create_clock -name {VCLK_N64_VIRT} -period $n64_vclk_per -waveform $n64_vclk_waveform

# Video Clock (multiple defines for easier definition of clock groups)
create_clock -name {VCLK_1x_base} -period $n64_vclk_per -waveform $n64_vclk_waveform $vclk_input
create_clock -name {VCLK_testpattern_base} -period $n64_vclk_per -waveform $n64_vclk_waveform $vclk_input -add

# system clock
create_clock -name {SYS_CLK} -period $sys_clk_per -waveform $sys_clk_waveform [get_ports {SYS_CLK}]


#**************************************************************
# Create Generated Clock
#**************************************************************

# Video Clocks

create_clock -name {VCLK_2x_base} -period $n64_vclk_per -waveform $n64_vclk_waveform $vclk_input -add

set vclk_pll_in [get_pins {clk_n_rst_hk_u|video_pll_u|altpll_component|auto_generated|pll1|inclk[0]}]
set vclk_pll_3x_out [get_pins {clk_n_rst_hk_u|video_pll_u|altpll_component|auto_generated|pll1|clk[0]}]
create_generated_clock -name {VCLK_3x_base} -source $vclk_pll_in -master_clock {VCLK_1x_base} -divide_by 2 -multiply_by 3 -duty_cycle 75 $vclk_pll_3x_out


# Other Internal Video Clocks
# First MUX
set vclk_mux_0_out [get_pins {n64adv_ppu_u|linemult_u|vclk_tx_3mux_u|LPM_MUX_component|auto_generated|result_node[0]|combout}]
create_generated_clock -name {VCLK_1x_out_pre0} -source $vclk_input -master_clock {VCLK_1x_base} $vclk_mux_0_out
create_generated_clock -name {VCLK_2x_out_pre0} -source $vclk_input -master_clock {VCLK_2x_base} $vclk_mux_0_out -add
create_generated_clock -name {VCLK_3x_out_pre0} -source $vclk_pll_3x_out -master_clock {VCLK_3x_base} $vclk_mux_0_out -add

# Secound MUX
set vclk_mux_1_out [get_pins {n64adv_ppu_u|vclk_tx_post_testpattern_2mux_u|LPM_MUX_component|auto_generated|result_node[0]|combout}]
create_generated_clock -name {VCLK_1x_out_pre1} -source $vclk_mux_0_out -master_clock {VCLK_1x_out_pre0} $vclk_mux_1_out
create_generated_clock -name {VCLK_2x_out_pre1} -source $vclk_mux_0_out -master_clock {VCLK_2x_out_pre0} $vclk_mux_1_out -add
create_generated_clock -name {VCLK_3x_out_pre1} -source $vclk_mux_0_out -master_clock {VCLK_3x_out_pre0} $vclk_mux_1_out -add
create_generated_clock -name {VCLK_testpattern_out_pre} -source $vclk_input -master_clock {VCLK_testpattern_base} $vclk_mux_1_out -add

# Output Video Clocks
set vclk_out [get_ports {CLK_ADV712x}]
create_generated_clock -name {VCLK_1x_out} -source $vclk_mux_1_out -master_clock {VCLK_1x_out_pre1} $vclk_out
create_generated_clock -name {VCLK_2x_out} -source $vclk_mux_1_out -master_clock {VCLK_2x_out_pre1} $vclk_out -add
create_generated_clock -name {VCLK_3x_out} -source $vclk_mux_1_out -master_clock {VCLK_3x_out_pre1} $vclk_out -add
create_generated_clock -name {VCLK_testpattern_out} -source $vclk_mux_1_out -master_clock {VCLK_testpattern_out_pre} $vclk_out -add

# System PLL Clocks
set sys_pll_in [get_pins {clk_n_rst_hk_u|sys_pll_u|altpll_component|auto_generated|pll1|inclk[0]}]
set sys_pll_4M_out [get_pins {clk_n_rst_hk_u|sys_pll_u|altpll_component|auto_generated|pll1|clk[0]}]
set sys_pll_16k_out [get_pins {clk_n_rst_hk_u|sys_pll_u|altpll_component|auto_generated|pll1|clk[1]}]
set sys_pll_25M_out [get_pins {clk_n_rst_hk_u|sys_pll_u|altpll_component|auto_generated|pll1|clk[2]}]
create_generated_clock -name {CLK_4M} -source $sys_pll_in -divide_by 25 -multiply_by 2 $sys_pll_4M_out
create_generated_clock -name {CLK_16k} -source $sys_pll_in -divide_by 3125 -multiply_by 1 $sys_pll_16k_out
create_generated_clock -name {CLK_25M} -source $sys_pll_in -divide_by 2 -multiply_by 1 $sys_pll_25M_out

# DCLK for Flash
create_generated_clock -name {ALTERA_DCLK} -source $sys_pll_25M_out [get_ports {*ALTERA_DCLK}]


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

# delays were carried out at the modding board and includes some potential skew
set n64_data_delay_min 0.0
set n64_data_delay_max 6.5

set_input_delay -clock {VCLK_N64_VIRT} -min $n64_data_delay_min [get_ports {nVDSYNC}]
set_input_delay -clock {VCLK_N64_VIRT} -max $n64_data_delay_max [get_ports {nVDSYNC}]
set_input_delay -clock {VCLK_N64_VIRT} -min $n64_data_delay_min [get_ports {VD_i[*]}]
set_input_delay -clock {VCLK_N64_VIRT} -max $n64_data_delay_max  [get_ports {VD_i[*]}]

set_input_delay -clock { CLK_25M } 0 [get_ports *ALTERA_DATA0]

set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tms]


#**************************************************************
# Set Output Delay
#**************************************************************
set adv_tsu 0.5
set adv_th  1.5
set out_dly_max $adv_tsu
set out_dly_min [expr -$adv_th]

set_output_delay -clock {VCLK_1x_out} -max $out_dly_max [get_ports {VD_o* nCSYNC_ADV712x}]
set_output_delay -clock {VCLK_1x_out} -min $out_dly_min [get_ports {VD_o* nCSYNC_ADV712x}]
set_output_delay -clock {VCLK_2x_out} -max $out_dly_max [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock {VCLK_2x_out} -min $out_dly_min [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock {VCLK_3x_out} -max $out_dly_max [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock {VCLK_3x_out} -min $out_dly_min [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock {VCLK_testpattern_out} -max $out_dly_max [get_ports {VD_o* nCSYNC_ADV712x}] -add
set_output_delay -clock {VCLK_testpattern_out} -min $out_dly_min [get_ports {VD_o* nCSYNC_ADV712x}] -add

#set_output_delay -clock {VCLK_1x_testpattern_o} 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add
#set_output_delay -clock {VCLK_1x_out} 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add
#set_output_delay -clock {VCLK_2x_out} 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add
#set_output_delay -clock {VCLK_3x_out} 0 [get_ports {nHSYNC* nVSYNC* nCSYNC}] -add

set_output_delay -clock {ALTERA_DCLK} 0 [get_ports {*ALTERA_SCE *ALTERA_SDO}]

set_output_delay -clock {altera_reserved_tck} 20 [get_ports {altera_reserved_tdo}]


#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -logically_exclusive \
                    -group {VCLK_1x_base VCLK_1x_out_pre0 VCLK_1x_out_pre1 VCLK_1x_out} \
                    -group {VCLK_2x_base VCLK_2x_out_pre0 VCLK_2x_out_pre1 VCLK_2x_out} \
                    -group {VCLK_3x_base VCLK_3x_base VCLK_3x_out_pre0 VCLK_3x_out_pre1 VCLK_3x_out} \
                    -group {VCLK_testpattern_base VCLK_testpattern_out_pre VCLK_testpattern_out} \
                    -group {SYS_CLK CLK_4M CLK_16k CLK_25M}


#**************************************************************
# Set False Path
#**************************************************************

# some misc intput ports as false path
set_false_path -from [get_ports {nRST CTRL_i UseVGA_HVSync nFilterBypass nEN_RGsB nEN_YPbPr SL_str* n240p n480i_bob}]

# configuration registers as false path
set_false_path -from [get_registers {n64adv_ppu_u|cfg_* n64adv_ppu_u|Filter*}]
set_false_path -from [get_registers {n64adv_ppu_u|get_vinfo_u|*}] -to [get_registers {n64adv_ppu_u|linemult_u|*}]
set_false_path -to [get_registers {n64adv_controller_u|use_igr n64adv_controller_u|OSDInfo* n64adv_controller_u|OutConfigSet*}]
set_false_path -from [get_registers {n64adv_ppu_u|linemult_u|SL_rval*}]

# tell the timer to not analyse these paths of the linemult unit in direct mode
set list_direct_false_from [list [get_registers {n64adv_ppu_u|linemult_u|nVDSYNC_dbl}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|nVS_i_buf}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|nHS_i_buf}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|hstart_rx*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|hstop_rx*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|wren}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|wrpage*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|wrhcnt*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|wraddr*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|line_overflow_r}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|valid_line_r}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|line_width*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|newFrame*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|FrameID}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|SL_rval*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|start_rdproc}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|linemult_sel_buf*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|*nVRST_Tx_o*}] \
                                 [get_registers {n64adv_ppu_u|linemult_u|videobuffer_u|*}]]
foreach from_path $list_direct_false_from {
  set_false_path -from $from_path -to [get_clocks {VCLK_1x_out_pre0}]
}

set list_direct_false_to [list [get_registers {n64adv_ppu_u|linemult_u|linemult_sel_buf*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|sync4tx_u|*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|hstart_tx*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|hstop_tx*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|nHS_width*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|pic_shift*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|nVS_width*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|nVS_delay*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rden*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rdrun*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rdcnt*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rdpage*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rdhcnt*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rdvcnt*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|rdaddr*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|start_rdproc_tx_resynced_pre}] \
                               [get_registers {n64adv_ppu_u|linemult_u|newFrame*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|FrameID}] \
                               [get_registers {n64adv_ppu_u|linemult_u|videobuffer_u|*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|*_mult*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|drawSL*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|*_sl*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|*_pp*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|Y_ref*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|SLHyb_rval*}] \
                               [get_registers {n64adv_ppu_u|linemult_u|SLHyb_str*}]]
foreach to_path $list_direct_false_to {
  set_false_path -from [get_clocks {VCLK_1x_base}] -to $to_path
}

# some misc output ports as false path
set_false_path -to [get_ports {nRST}]
set_false_path -to [get_ports {nHSYNC* nVSYNC* nCSYNC}]

# false path for some clock analysis
set_false_path -from [get_clocks {VCLK_1x_base}] -to [get_registers {n64adv_ppu_u|testpattern_u|*}]
set_false_path -from [get_registers {n64adv_ppu_u|testpattern_u|*}] -to [get_clocks {VCLK_1x_out_pre1}]
set_false_path -from [get_registers {n64adv_ppu_u|deblur_management_u|* n64adv_ppu_u|video_demux_u|* n64adv_ppu_u|osd_injection_u|* n64adv_ppu_u|gamma_module_u|* n64adv_ppu_u|linemult_u|*}] \
               -to [get_clocks {VCLK_testpattern_base}]


#**************************************************************
# Set Multicycle Path
#**************************************************************
# (not exhaustive at all)

set dbl_cycle_paths_from [list [get_ports {VD_i[*]}] \
                               [get_registers {n64adv_ppu_u|video_demux_u|*}] \
                               [get_registers {n64adv_ppu_u|get_vinfo_u|*}] \
                               [get_registers {n64adv_ppu_u|deblur_management_u|*}] \
                         ]
set dbl_cycle_paths_to [list [get_registers {n64adv_ppu_u|video_demux_u|*}] \
                             [get_registers {n64adv_ppu_u|get_vinfo_u|*}] \
                             [get_registers {n64adv_ppu_u|deblur_management_u|*}] \
                       ]

foreach from_path $dbl_cycle_paths_from {
  foreach to_path $dbl_cycle_paths_to {
    if {!($from_path == "[get_registers {n64adv_ppu_u|deblur_management_u|*}]" && $to_path == "[get_registers {n64adv_ppu_u|get_vinfo_u|*}]")} {
      set_multicycle_path -from $from_path -to $to_path -setup 2
      set_multicycle_path -from $from_path -to $to_path -hold 1
    }
  }
}

# revert some multicycle paths to their default values
set_multicycle_path -from [get_registers {n64adv_ppu_u|deblur_management_u|gradient_changes[*]}] -setup 1
set_multicycle_path -from [get_registers {n64adv_ppu_u|deblur_management_u|gradient_changes[*]}] -hold 0
set_multicycle_path -from [get_registers {n64adv_ppu_u|video_demux_u|vdata_r_0[*]}] -to [get_registers {n64adv_ppu_u|video_demux_u|vdata_r_1[*]}] -setup 1
set_multicycle_path -from [get_registers {n64adv_ppu_u|video_demux_u|vdata_r_0[*]}] -to [get_registers {n64adv_ppu_u|video_demux_u|vdata_r_1[*]}] -hold 0


#**************************************************************
# Set Maximum Delay
#**************************************************************

# overconstraining path between clock input, pll outputs, muxes and output (only for fitter)
if {[string equal $::quartus(nameofexecutable) "quartus_fit"]} {
  set_max_delay -from $vclk_input -to $vclk_mux_1_out 0.000
  set_max_delay -from $vclk_pll_3x_out -to $vclk_mux_0_out 0.000
  set_max_delay -from $vclk_mux_0_out -to $vclk_mux_1_out 0.000
  set_max_delay -from $vclk_mux_1_out -to $vclk_out 0.000
}


#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

