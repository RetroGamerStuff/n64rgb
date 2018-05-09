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
// Module Name:    n64_vdemux
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    demux the video data from the input data stream
//
// Dependencies: vh/n64a_params.vh
//               rtl/n64a_gamma_table.v
//
// Revision: 2.1
// Features: Gamma correction via lookup table
//
//////////////////////////////////////////////////////////////////////////////////


module n64_vdemux(
  VCLK,
  nDSYNC,
  nRST,

  D_i,
  demuxparams_i,
  gammaparams_i,

  vdata_r_0,
  vdata_r_1,
  vdata_r_6
);

`include "vh/n64a_params.vh"

input VCLK;
input nDSYNC;
input nRST;

input  [color_width_i-1:0] D_i;
input  [              4:0] demuxparams_i;
input  [              3:0] gammaparams_i;

output reg [`VDATA_I_FU_SLICE] vdata_r_0 = {vdata_width_i{1'b0}}; // buffer for sync, red, green and blue
output reg [`VDATA_I_FU_SLICE] vdata_r_1 = {vdata_width_i{1'b0}}; // (unpacked array types in ports requires system verilog)
output reg [`VDATA_I_FU_SLICE] vdata_r_6 = {vdata_width_i{1'b0}};


// unpack deblur info

wire [1:0] data_cnt    = demuxparams_i[4:3];
wire       vmode       = demuxparams_i[  2];
wire       ndo_deblur  = demuxparams_i[  1];
wire       n15bit_mode = demuxparams_i[  0];

wire       en_gamma_boost     = ~(gammaparams_i == `GAMMA_TABLE_OFF);
wire [3:0] gamma_rom_page_tmp =  (gammaparams_i < `GAMMA_TABLE_OFF) ? gammaparams_i       :
                                                                      gammaparams_i - 1'b1;
wire [2:0] gamma_rom_page     = gamma_rom_page_tmp[2:0];

wire posedge_nCSYNC = !vdata_r_0[3*color_width_i] &  D_i[0];


// start of rtl

reg nblank_rgb = 1'b1;

always @(posedge VCLK)
  if (!nDSYNC) begin
    if (ndo_deblur) begin
      nblank_rgb <= 1'b1;
    end else begin
      if(posedge_nCSYNC) // posedge nCSYNC -> reset blanking
        nblank_rgb <= vmode;
      else
        nblank_rgb <= ~nblank_rgb;
    end
  end


reg [`VDATA_I_SY_SLICE] vdata_sync_delay [2:5]; // used to compensate delay due to gamma table
reg [`VDATA_I_CO_SLICE] vdata_post_gamma;       // red, green and blue (post gamma table)

reg  [color_width_i-1:0] gamma_vdata_in = {color_width_i{1'b0}};
wire [color_width_i-1:0] gamma_vdata_out;

integer i;
initial begin
  for (i = 2; i < 6; i = i+1) begin
         vdata_sync_delay[i] = 4'h0;
  end
  vdata_post_gamma = {3*color_width_i{1'b0}};
end


always @(posedge VCLK) begin // data register management
  if (!nDSYNC) begin
    // shift data to output registers
    if (ndo_deblur)
      vdata_r_1[`VDATA_I_SY_SLICE] <= vdata_r_0[`VDATA_I_SY_SLICE];

    if (nblank_rgb)  // deblur active: pass RGB only if not blanked
      vdata_r_1[`VDATA_I_CO_SLICE] <= vdata_r_0[`VDATA_I_CO_SLICE];

    vdata_post_gamma[`VDATA_I_RE_SLICE] <= gamma_vdata_out;
           vdata_r_0[`VDATA_I_SY_SLICE] <= D_i[3:0];
  end else begin
    // demux of RGB
    case(data_cnt)
      2'b01: begin
        vdata_post_gamma[`VDATA_I_GR_SLICE] <= gamma_vdata_out;
                             gamma_vdata_in <= vdata_r_1[`VDATA_I_RE_SLICE];
               vdata_r_0[`VDATA_I_RE_SLICE] <= n15bit_mode ? D_i : {D_i[6:2], 2'b00};
      end
      2'b10: begin
        vdata_post_gamma[`VDATA_I_BL_SLICE] <= gamma_vdata_out;
                             gamma_vdata_in <= vdata_r_1[`VDATA_I_GR_SLICE];
               vdata_r_0[`VDATA_I_GR_SLICE] <= n15bit_mode ? D_i : {D_i[6:2], 2'b00};
        if (!ndo_deblur)
          vdata_r_1[`VDATA_I_SY_SLICE] <= vdata_r_0[`VDATA_I_SY_SLICE];
      end
      2'b11: begin
                      gamma_vdata_in <= vdata_r_1[`VDATA_I_BL_SLICE];
        vdata_r_0[`VDATA_I_BL_SLICE] <= n15bit_mode ? D_i : {D_i[6:2], 2'b00};
      end
    endcase
  end

  vdata_r_6 <= {vdata_sync_delay[5][`VDATA_I_SY_SLICE],vdata_post_gamma};

  for (i = 5; i > 2; i = i-1)
    vdata_sync_delay[i] <= vdata_sync_delay[i-1];
  vdata_sync_delay[2] <= vdata_r_1[`VDATA_I_SY_SLICE];

  if (!nRST) begin
    vdata_r_6  <= {vdata_width_i{1'b0}};
    for (i = 2; i < 6; i = i+1)
      vdata_sync_delay[i] <= 4'h0;
    vdata_r_0  <= {vdata_width_i{1'b0}};

    gamma_vdata_in   <=   {color_width_i{1'b0}};
    vdata_post_gamma <= {3*color_width_i{1'b0}};
  end
end

n64a_gamma_table gamma_table_u(
  .VCLK(VCLK),
  .gamma_val(gamma_rom_page),
  .vdata_in(gamma_vdata_in),
  .nbypass(en_gamma_boost),
  .vdata_out(gamma_vdata_out)
);

endmodule
