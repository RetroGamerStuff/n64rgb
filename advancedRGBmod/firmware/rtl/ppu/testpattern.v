//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2021 by Peter Bartmann <borti4938@gmail.com>
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
// Module Name:    testpattern
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV:    EP4CE6E22   , EP4CE10E22
//                 Cyclone 10 LP: 10CL006YE144, 10CL010YE144
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////

module testpattern(
  VCLK,
  nRST,

  palmode,

  vdata_sync_valid_i,
  vdata_sync_i,
  vdata_valid_o,
  vdata_o
);

`include "vh/n64adv_vparams.vh"

input VCLK;
input nRST;

input palmode;

input vdata_sync_valid_i;
input [3:0] vdata_sync_i;
output reg vdata_valid_o = 1'b0;
output reg [`VDATA_I_FU_SLICE] vdata_o = {vdata_width_i{1'b0}};



wire negedge_nVSYNC = vdata_o[3*color_width_i+3] & !vdata_sync_i[3];
wire negedge_nHSYNC = vdata_o[3*color_width_i+1] & !vdata_sync_i[1];

reg [8:0] vcnt = 9'b0;
reg [9:0] hcnt = 10'b0;

wire [8:0] pattern_vstart = palmode ? `VSTART_PAL_LX1 : `VSTART_NTSC_LX1;
wire [8:0] pattern_vstop = palmode ? `VSTOP_PAL_LX1 : `VSTOP_NTSC_LX1;
wire [9:0] pattern_hstart = palmode ? `HSTART_PAL : `HSTART_NTSC;
wire [9:0] pattern_hstop = palmode ? `HSTOP_PAL : `HSTOP_NTSC;


always @(posedge VCLK or negedge nRST)
  if (!nRST) begin
    vdata_valid_o <= 1'b0;
    vdata_o <= {vdata_width_i{1'b0}};
    vcnt <= 9'b0;
    hcnt <= 10'b0;
  end else begin
    vdata_valid_o <= vdata_sync_valid_i;
    if (vdata_sync_valid_i) begin
      if (negedge_nHSYNC) begin
        hcnt <= 10'b0;
        vcnt <= &vcnt ? vcnt : vcnt + 1'b1;
      end else begin
        hcnt <= &hcnt ? hcnt : hcnt + 1'b1;
      end
      if (negedge_nVSYNC)
        vcnt <= 9'b0;

      if ((vcnt >= pattern_vstart) && (vcnt < pattern_vstop)) begin
        if ((hcnt > pattern_hstart) && (hcnt < pattern_hstop))
          vdata_o[`VDATA_I_CO_SLICE] <= {3*color_width_i{~vdata_o[0]}};
        else
          vdata_o[`VDATA_I_CO_SLICE] <= {3*color_width_i{1'b0}};

        if (hcnt == pattern_hstart)
          vdata_o[`VDATA_I_CO_SLICE] <= {3*color_width_i{vcnt[0]}};
      end else begin
        vdata_o[`VDATA_I_CO_SLICE] <= {3*color_width_i{1'b0}};
      end

      vdata_o[`VDATA_I_SY_SLICE] <= vdata_sync_i;
    end
  end

endmodule
