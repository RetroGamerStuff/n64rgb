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
// Module Name:    n64a_gamma_module
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    gamma correction (nDSYNC synchronuous)
//
// Dependencies: vh/n64a_params.vh
//               rtl/n64a_gamma_table.v
//
// Revision: 2.0
// Features: Gamma correction via lookup table
//
//////////////////////////////////////////////////////////////////////////////////

module n64a_gamma_module(
  VCLK,
  nDSYNC,
  nRST,
  gammaparams_i,
  video_data_i,
  video_data_o
);

`include "vh/n64a_params.vh"

input VCLK;
input nDSYNC;
input nRST;

input [ 3:0] gammaparams_i;

input      [`VDATA_I_FU_SLICE] video_data_i;
output reg [`VDATA_I_FU_SLICE] video_data_o = {vdata_width_i{1'b0}};


// translate gamma table
wire       en_gamma_boost     = ~(gammaparams_i == `GAMMA_TABLE_OFF);
wire [3:0] gamma_rom_page_tmp =  (gammaparams_i < `GAMMA_TABLE_OFF) ? gammaparams_i       :
                                                                      gammaparams_i - 1'b1;
wire [2:0] gamma_rom_page = gamma_rom_page_tmp[2:0];


// connect data tables (output has delay of two)
wire [`VDATA_I_CO_SLICE] gamma_vdata_out;

n64a_gamma_table gamma_table_re(
  .VCLK(VCLK),
  .gamma_val(gamma_rom_page),
  .vdata_in(video_data_i[`VDATA_I_RE_SLICE]),
  .nbypass(en_gamma_boost),
  .vdata_out(gamma_vdata_out[`VDATA_I_RE_SLICE])
);

n64a_gamma_table gamma_table_gr(
  .VCLK(VCLK),
  .gamma_val(gamma_rom_page),
  .vdata_in(video_data_i[`VDATA_I_GR_SLICE]),
  .nbypass(en_gamma_boost),
  .vdata_out(gamma_vdata_out[`VDATA_I_GR_SLICE])
);

n64a_gamma_table gamma_table_bl(
  .VCLK(VCLK),
  .gamma_val(gamma_rom_page),
  .vdata_in(video_data_i[`VDATA_I_BL_SLICE]),
  .nbypass(en_gamma_boost),
  .vdata_out(gamma_vdata_out[`VDATA_I_BL_SLICE])
);


// registered output
always @(posedge VCLK) begin
  if (!nDSYNC)
    video_data_o <= {video_data_i[`VDATA_I_SY_SLICE],gamma_vdata_out};
  if (!nRST)
    video_data_o <= {vdata_width_i{1'b0}};
end

 


endmodule
