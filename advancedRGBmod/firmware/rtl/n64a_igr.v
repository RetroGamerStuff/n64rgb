//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2016-2018 by Peter Bartmann <borti4938@gmx.de>
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
// Module Name:    n64a_igr
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: universial (PLL and 50MHz clock required)
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies: vh/igr_params.vh
//               ip/altpll_1.qip
//
// Revision: 3.0
// Features: console reset
//           override heuristic for deblur (resets on each reset and power cycle)
//           activate / deactivate de-blur in 240p (a change overrides heuristic for de-blur)
//           activate / deactivate 15bit mode (no selectable default since 2.6)
//           selectable defaults
//           defaults set on each power cycle and on each reset
//
//////////////////////////////////////////////////////////////////////////////////


module n64a_igr (
  SYS_CLK,
  nRST,

  CTRL,

  Default_DeBlur,
  Default_nForceDeBlur,

  nDeBlur,
  nForceDeBlur,
  n15bit_mode
);

`include "vh/igr_params.vh"


input SYS_CLK;
inout nRST;

input CTRL;

input Default_nForceDeBlur;
input Default_DeBlur;

output reg nDeBlur      = 1'b1;
output reg nForceDeBlur = 1'b1;
output reg n15bit_mode  = 1'b1;


wire CLK_4M, CLK_16k;

altpll_1 sys_pll(
  .inclk0(SYS_CLK),
  .c0(CLK_4M),
  .c1(CLK_16k)
);


reg [1:0] rd_state  = 2'b0; // state machine

localparam ST_WAIT4N64 = 2'b00; // wait for N64 sending request to controller
localparam ST_N64_RD   = 2'b01; // N64 request sniffing
localparam ST_CTRL_RD  = 2'b10; // controller response

reg [11:0] wait_cnt  = 12'b0; // counter for wait state (needs appr. 1.0ms at CLK_4M clock to fill up from 0 to 4095)

reg [ 3:0] sampl_p_n64  =  4'h8; // wait_cnt increased a few times since neg. edge -> sample data
reg [ 3:0] sampl_p_ctrl =  4'h8; // (9 by default -> delay somewhere around 2.25us)

reg [ 2:0] ctrl_hist =  3'h7;

wire ctrl_negedge = ctrl_hist[2] & ~ctrl_hist[1];
wire ctrl_bit     = ctrl_hist[1];

reg [15:0] serial_data = 16'h0;
reg  [3:0] data_cnt    =  3'b000;

reg initiate_nrst = 1'b0;

reg nfirstboot = 1'b0;


// controller data bits:
//  0: 7 - A, B, Z, St, Du, Dd, Dl, Dr
//  8:15 - 'Joystick reset', (0), L, R, Cu, Cd, Cl, Cr
// 16:23 - X axis
// 24:31 - Y axis
// 32    - Stop bit
// (bits[0:15] used here)

always @(posedge CLK_4M) begin
  case (rd_state)
    ST_WAIT4N64:
      if (&wait_cnt) begin // waiting duration ends (exit wait state only if CTRL was high for a certain duration)
        rd_state <= ST_N64_RD;
        data_cnt <= 3'b000;
      end
    ST_N64_RD: begin
      if (wait_cnt[7:0] == {4'h0,sampl_p_n64}) begin // sample data
        if (data_cnt[3]) // eight bits read
          if (ctrl_bit & (serial_data[13:6] == 8'b10000000)) begin // check command and stop bit
          // trick: the 2 LSB command bits lies where controller produces unused constant values
          //         -> (hopefully) no exchange with controller response
            rd_state <= ST_CTRL_RD;
            data_cnt <=  3'b000;
          end else
            rd_state <= ST_WAIT4N64;
        else begin
          serial_data[13:6] <= {ctrl_bit,serial_data[13:7]};
          data_cnt          <= data_cnt + 1'b1;
        end
      end
      if (|data_cnt & ctrl_negedge)
        sampl_p_n64 <= wait_cnt[4:1];
    end
    ST_CTRL_RD: begin
      if (wait_cnt[7:0] == {4'h0,sampl_p_ctrl}) begin // sample data
        if (&data_cnt) begin // sixteen bits read (analog values of stick not point of interest)
          rd_state <= ST_WAIT4N64;
          case ({ctrl_bit,serial_data[15:1]})
            `IGR_DEBLUR_OFF: begin
              nForceDeBlur <= 1'b0;
              nDeBlur      <= 1'b1;
            end
            `IGR_DEBLUR_ON: begin
              nForceDeBlur <= 1'b0;
              nDeBlur      <= 1'b0;
            end
            `IGR_15BITMODE_OFF: begin
              n15bit_mode <= 1'b1;
            end
            `IGR_15BITMODE_ON: begin
              n15bit_mode <= 1'b0;
            end
            `IGR_RESET: begin
              initiate_nrst <= 1'b1;
            end
          endcase
        end else begin
          data_cnt    <= data_cnt + 1'b1;
          serial_data <= {ctrl_bit,serial_data[15:1]};
        end
      end
      if (|data_cnt & ctrl_negedge)
        sampl_p_ctrl <= wait_cnt[4:1];
    end
    default: begin
      rd_state <= ST_WAIT4N64;
    end
  endcase

  if (ctrl_negedge) begin    // counter resets on neg. edge
    wait_cnt <= 12'h000;
  end else begin
    if (~&wait_cnt) // saturate counter if needed
      wait_cnt <= wait_cnt + 1'b1;
    else            // counter saturated
      rd_state <= ST_WAIT4N64;
  end

  ctrl_hist <= {ctrl_hist[1:0],CTRL};

  if (!nRST) begin
    nForceDeBlur <= Default_nForceDeBlur;

    rd_state      <= ST_WAIT4N64;
    wait_cnt      <= 12'h000;
    ctrl_hist     <=  3'h7;
    initiate_nrst <=  1'b0;
  end

  if (!nfirstboot) begin
    nfirstboot   <=  1'b1;
    nDeBlur      <= ~Default_DeBlur;
    nForceDeBlur <=  Default_nForceDeBlur;
  end
end

reg       drv_rst =  1'b0;
reg [9:0] rst_cnt = 10'b0; // ~64ms are needed to count from max downto 0 with CLK_16k.

always @(posedge CLK_16k) begin
  if (initiate_nrst == 1'b1) begin
    drv_rst <= 1'b1;      // reset system
    rst_cnt <= 10'h3ff;
  end else if (|rst_cnt) // decrement as long as rst_cnt is not zero
    rst_cnt <= rst_cnt - 1'b1;
  else
    drv_rst <= 1'b0; // end of reset
end

assign nRST = drv_rst ? 1'b0 : 1'bz;

endmodule
