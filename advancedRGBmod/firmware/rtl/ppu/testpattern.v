//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the N64 RGB/YPbPr DAC project.
//
// Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmail.com>
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
  nVDSYNC,
  nRST,

  vmode,
  Sync_in,
  vdata_out
);

`include "vh/n64adv_vparams.vh"

input VCLK;
input nVDSYNC;
input nRST;

input vmode;
input [3:0] Sync_in;
output reg [`VDATA_O_FU_SLICE] vdata_out = {vdata_width_o{1'b0}};



wire posedge_nVSYNC = !vdata_out[3*color_width_o+3] &  Sync_in[3];
wire posedge_nHSYNC = !vdata_out[3*color_width_o+1] &  Sync_in[1];
// wire posedge_nCSYNC = !vdata_out[3*color_width_o  ] &  Sync_in[0];

reg [8:0] vcnt = 9'b0;
reg [9:0] hcnt = 10'b0;

wire [8:0] pattern_vstart = vmode ? 9'd22 : 9'd18;
wire [8:0] pattern_vstop = vmode ? 9'd296 : 9'd248;
wire [9:0] pattern_hstart = vmode ? (`HSTART_PAL-10'd61) : (`HSTART_NTSC-11'd56);
wire [9:0] pattern_hstop = vmode ? (`HSTOP_PAL-10'd61) : (`HSTOP_NTSC-11'd56);

always @(posedge VCLK or negedge nRST)
  if (!nRST) begin
    vdata_out <= {vdata_width_o{1'b0}};

    vcnt <= 9'b0;
    hcnt <= 10'b0;
  end else begin
    if (!nVDSYNC) begin
      if (posedge_nHSYNC) begin
        hcnt <= 10'b0;
        vcnt <= &vcnt ? vcnt : vcnt + 1'b1;
      end else begin
        hcnt <= &hcnt ? hcnt : hcnt + 1'b1;
      end
      if (posedge_nVSYNC)
        vcnt <= 9'b0;

      if ((vcnt > pattern_vstart) && (vcnt < pattern_vstop)) begin
        if ((hcnt > pattern_hstart) && (hcnt < pattern_hstop))
          vdata_out[`VDATA_O_CO_SLICE] <= {3*color_width_o{~vdata_out[0]}};
        else
          vdata_out[`VDATA_O_CO_SLICE] <= {3*color_width_o{1'b0}};

        if (hcnt == pattern_hstart)
          vdata_out[`VDATA_O_CO_SLICE] <= {3*color_width_o{vcnt[0]}};
      end else begin
        vdata_out[`VDATA_O_CO_SLICE] <= {3*color_width_o{1'b0}};
      end

      vdata_out[`VDATA_O_SY_SLICE] <= Sync_in;
    end
  end

endmodule
