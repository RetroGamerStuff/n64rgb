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
// Module Name:    n64a_testpattern
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV:    EP4CE6E22   , EP4CE10E22
//                 Cyclone 10 LP: 10CL006YE144, 10CL010YE144
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies: vh/n64a_params.vh
//
// Features: testpattern
//
//////////////////////////////////////////////////////////////////////////////////

module n64a_testpattern(
  VCLK,
  nDSYNC,
  nRST,

  vmode,
  Sync_in,
  vdata_out
);

`include "vh/n64a_params.vh"

input VCLK;
input nDSYNC;
input nRST;

input vmode;
input [3:0] Sync_in;
output [`VDATA_O_FU_SLICE] vdata_out;



wire posedge_nVSYNC = !vdata_out[3*color_width_o+3] &  Sync_in[3];
wire posedge_nHSYNC = !vdata_out[3*color_width_o+1] &  Sync_in[1];
wire posedge_nCSYNC = !vdata_out[3*color_width_o  ] &  Sync_in[0];

reg [8:0] vcnt = 9'b0;
reg [9:0] hcnt = 10'b0;

wire [8:0] pattern_vstart = vmode ? `TESTPAT_VSTART_PAL : `TESTPAT_VSTART_NTSC;
wire [8:0] pattern_vstop  = vmode ? `TESTPAT_VSTOP_PAL  : `TESTPAT_VSTOP_NTSC;

reg [4:0] vdata_checkboard_fine = 5'd0;

always @(posedge VCLK) begin
  if (!nDSYNC) begin
    if (posedge_nHSYNC) begin
      hcnt <= 10'b0;
      vcnt <= &vcnt ? vcnt : vcnt + 1'b1;
    end else begin
      hcnt <= &hcnt ? hcnt : hcnt + 1'b1;
    end
    if (posedge_nVSYNC)
      vcnt <= 9'b0;

    if ((vcnt > pattern_vstart) && (vcnt < pattern_vstop)) begin
      if ((hcnt > `TESTPAT_HSTART) && (hcnt < `TESTPAT_HSTOP))
        vdata_checkboard_fine[0] <= ~vdata_checkboard_fine[0];
      else
        vdata_checkboard_fine[0] <= 1'b0;

      if (hcnt == `TESTPAT_HSTART)
        vdata_checkboard_fine[0] <= vcnt[0];
    end else begin
      vdata_checkboard_fine[0] <= 1'b0;
    end

    vdata_checkboard_fine[4:1] <= Sync_in;
  end
  if (!nRST) begin
    vdata_checkboard_fine <= 5'd0;

    vcnt <= 9'b0;
    hcnt <= 10'b0;
  end
end

assign vdata_out = {vdata_checkboard_fine[4:1],{3*color_width_o{vdata_checkboard_fine[0]}}};


endmodule
