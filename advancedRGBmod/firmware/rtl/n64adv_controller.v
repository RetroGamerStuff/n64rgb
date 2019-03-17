//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2019 by Peter Bartmann <borti4938@gmx.de>
//
// N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////////
//
// Company:  Circuit-Board.de
// Engineer: borti4938
//
// Module Name:    n64adv_controller
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: universial (PLL and 50MHz clock required)
// Tool versions:  Altera Quartus Prime
// Description:
// Latest change: ip independet implementation of RAM
//
//////////////////////////////////////////////////////////////////////////////////


module n64adv_controller (
  CLKs,
  CLKs_valid,
  nRST,
  nSRST,

  CTRL,

  InfoSet,
  JumperCfgSet,
  OutConfigSet,
  OSDWrVector,
  OSDInfo,

  VCLK,
  nVDSYNC,
  VD_VSi,
  nVRST
);

parameter [11:0] hdl_fw = 12'h000; // number is a dummy; defined in and passed from top module

`include "vh/n64adv_cparams.vh"

input [2:0] CLKs;
input CLKs_valid;
inout nRST;
input [2:0] nSRST;

input CTRL;

input      [ 3:0] InfoSet;
input      [ 6:0] JumperCfgSet;
output reg [47:0] OutConfigSet;
output     [24:0] OSDWrVector;
output reg [ 1:0] OSDInfo;

input VCLK;
input nVDSYNC;
input VD_VSi;
input nVRST;


// start of rtl

wire CLK_4M = CLKs[2];
wire CLK_16k = CLKs[1];
wire CLK_25M = CLKs[0];

wire nSRST_4M = nSRST[2];
wire nSRST_25M = nSRST[0];


reg negedge_nVSYNC = 1'b0;
reg nVSYNC_cur = 1'b0;

always @(posedge VCLK or negedge nVRST)
  if (!nVRST) begin
    negedge_nVSYNC <= 1'b0;
    nVSYNC_cur <= 1'b0;
  end else if (!nVDSYNC) begin
    negedge_nVSYNC <=  nVSYNC_cur & !VD_VSi;
    nVSYNC_cur <= VD_VSi;
  end

wire nVSYNC_25M_resynced;
register_sync #(
  .reg_width(1),
  .reg_preset(1'b0)
) sync4cpu_u(
  .clk(CLK_25M),
  .clk_en(1'b1),
  .nrst(nSRST_25M),
  .reg_i({nVSYNC_cur}),
  .reg_o({nVSYNC_25M_resynced})
);

// Part 1: Instantiate NIOS II
// ===========================

reg [9:0] time_out = 10'd1023;
reg FallbackMode  = 1'b0;
reg FallbackMode_valid = 1'b0;

always @(posedge VCLK)
  if (!FallbackMode_valid) begin
    if (~|time_out) begin
      FallbackMode <= ~nVRST;
      FallbackMode_valid <= 1'b1;
    end
    time_out <= time_out - 10'd1;
  end

wire [ 9:0] vd_wraddr;
wire [ 1:0] vd_wrctrl;
wire [12:0] vd_wrdata;

wire [31:0] SysConfigSet0;
// general structure [31:16] misc, [15:0] video
// [31:24] {(5bits reserve),show_osd_logo,show_osd,mute_osd}
// [23:16] {(5bits reserve),use_igr,igr for 15bit mode and deblur (not used in logic)}
// [15: 8] {show_testpattern,(2bits reserve),FilterSet (3bits),YPbPr,RGsB}
// [ 7: 0] {gamma (4bits),(1bit reserve),VI-DeBlur (2bit), 15bit mode}
wire [31:0] SysConfigSet1;
// general structure [31:16] 240p settings, [15:0] 480i settings
// [31:16] 240p: {(1bit reserve),linemult (2bits),Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_Method,Sl_ID,Sl_En}
// [15: 0] 480i: {(1bit reserve),field_fix,bob_deint.,Sl_hybrid_depth (5bits),Sl_str (4bits),(1bit reserve),Sl_link,Sl_ID,Sl_En}


system_n64adv1 system_u(
  .clk_clk(CLK_25M),
  .rst_reset_n(nSRST_25M),
  .sync_in_export({new_ctrl_data[1],nVSYNC_25M_resynced}),
  .vd_wraddr_export(vd_wraddr),
  .vd_wrctrl_export(vd_wrctrl),
  .vd_wrdata_export(vd_wrdata),
  .ctrl_data_in_export(serial_data[1]),
  .jumper_cfg_set_in_export({1'b0,JumperCfgSet}),
  .info_set_in_export({2'b00,InfoSet,FallbackMode,FallbackMode_valid}),
  .cfg_set0_out_export(SysConfigSet0),
  .cfg_set1_out_export(SysConfigSet1),
  .hdl_fw_in_export(hdl_fw)
);

assign OSDWrVector = {vd_wrctrl,vd_wraddr,vd_wrdata};

reg use_igr = 1'b0;

always @(posedge VCLK)
  if ((!nVDSYNC & negedge_nVSYNC) | !nVRST) begin
    use_igr      <= SysConfigSet0[18];
    OutConfigSet <= {SysConfigSet0[15:0],SysConfigSet1};
    OSDInfo[1]   <= &{SysConfigSet0[26:25],!SysConfigSet0[24]};  // show logo only in OSD
    OSDInfo[0]   <= SysConfigSet0[25] & !SysConfigSet0[24];
  end



// Part 2: Controller Sniffing
// ===========================

reg [1:0]      rd_state  = 2'b0; // state machine

localparam ST_WAIT4N64 = 2'b00; // wait for N64 sending request to controller
localparam ST_N64_RD   = 2'b01; // N64 request sniffing
localparam ST_CTRL_RD  = 2'b10; // controller response

reg [5:0] wait_cnt      = 6'h0; // counter for wait state (needs appr. 16us at CLK_4M clock to fill up from 0 to 63)
reg [2:0] ctrl_hist     = 3'h7;
wire      ctrl_negedge  =  ctrl_hist[2] & !ctrl_hist[1];
wire      ctrl_posedge  = !ctrl_hist[2] &  ctrl_hist[1];

reg [5:0] ctrl_low_cnt = 6'h0;
wire      ctrl_bit     = ctrl_low_cnt < wait_cnt;

reg [31:0] serial_data[0:1];
reg [ 4:0] ctrl_data_cnt = 5'h0;
reg [ 1:0] new_ctrl_data = 2'b00;

initial begin
  serial_data[1] <= 32'h0;
  serial_data[0] <= 32'h0;
end


reg initiate_nrst = 1'b0;


// controller data bits:
//  0: 7 - A, B, Z, St, Du, Dd, Dl, Dr
//  8:15 - 'Joystick reset', (0), L, R, Cu, Cd, Cl, Cr
// 16:23 - X axis
// 24:31 - Y axis
// 32    - Stop bit

always @(posedge CLK_4M or negedge nSRST_4M)
  if (!nSRST_4M) begin
    rd_state       <= ST_WAIT4N64;
    wait_cnt       <=  5'h0;
    ctrl_hist      <=  3'h7;
    ctrl_low_cnt   <=  6'h0;
    serial_data[1] <= 32'h0;
    serial_data[0] <= 32'h0;
    ctrl_data_cnt  <=  5'h0;
    new_ctrl_data  <=  2'b0;
    initiate_nrst  <=  1'b0;
  end else begin
    case (rd_state)
      ST_WAIT4N64:
        if (&wait_cnt & ctrl_negedge) begin // waiting duration ends (exit wait state only if CTRL was high for a certain duration)
          rd_state       <= ST_N64_RD;
          serial_data[0] <= 32'h0;
          ctrl_data_cnt  <=  5'h0;
        end
      ST_N64_RD: begin
        if (ctrl_posedge)       // sample data part 1
          ctrl_low_cnt <= wait_cnt;
        if (ctrl_negedge) begin // sample data part 2
          if (!ctrl_data_cnt[3]) begin  // eight bits read
            serial_data[0][29:22] <= {ctrl_bit,serial_data[0][29:23]};
            ctrl_data_cnt         <=  ctrl_data_cnt + 1'b1;
          end else if (serial_data[0][29:22] == 8'b10000000) begin // check command
            rd_state       <= ST_CTRL_RD;
            serial_data[0] <= 32'h0;
            ctrl_data_cnt  <=  5'h0;
          end else begin
            rd_state <= ST_WAIT4N64;
          end
        end
      end
      ST_CTRL_RD: begin
        if (ctrl_posedge)       // sample data part 1
          ctrl_low_cnt <= wait_cnt;
        if (ctrl_negedge) begin // sample data part 2
          if (~&ctrl_data_cnt) begin  // still reading
            serial_data[0] <= {ctrl_bit,serial_data[0][31:1]};
            ctrl_data_cnt  <=  ctrl_data_cnt + 1'b1;
          end else begin  // thirtytwo bits read
            rd_state         <= ST_WAIT4N64;
            serial_data[1]   <= {ctrl_bit,serial_data[0][31:1]};
            new_ctrl_data[0] <= 1'b1;  // signalling new controller data available
          end
        end
      end
      default: begin
        rd_state <= ST_WAIT4N64;
      end
    endcase

    if (ctrl_negedge | ctrl_posedge) begin // counter reset
      wait_cnt <= 5'h0;
    end else begin
      if (~&wait_cnt) // saturate counter if needed
        wait_cnt <= wait_cnt + 1'b1;
      else  // counter saturated
        rd_state <= ST_WAIT4N64;
    end

    ctrl_hist <= {ctrl_hist[1:0],CTRL};

    if (new_ctrl_data[0]) begin
      new_ctrl_data  <= 2'b10;
      if (use_igr & (serial_data[1][15:0] == `IGR_RESET))
        initiate_nrst <= 1'b1;
    end

    if (negedge_nVSYNC)
      new_ctrl_data[1] <= 1'b0;
  end


// Part 3: Trigger Reset on Demand
// ===============================

reg       drv_rst =  1'b0;
reg [9:0] rst_cnt = 10'b0; // ~64ms are needed to count from max downto 0 with CLK_16k.

always @(posedge CLK_16k)
  if (initiate_nrst == 1'b1) begin
    drv_rst <= 1'b1;            // reset system
    rst_cnt <= 10'h3ff;
  end else if (|rst_cnt) begin  // decrement as long as rst_cnt is not zero
    rst_cnt <= rst_cnt - 1'b1;
  end else begin
    drv_rst <= 1'b0;            // end of reset
  end

assign nRST = drv_rst ? 1'b0 : 1'bz;

endmodule
