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
// Module Name:    n64a_deblur
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: universial
// Tool versions:  Altera Quartus Prime
// Description:    estimates whether N64 uses blur or not
//
//////////////////////////////////////////////////////////////////////////////////


module n64a_deblur (
  VCLK,
  nVDSYNC,

  nRST,

  vdata_pre,
  VD_i,
  vid_state_i,

  algorithmsettings_i,
  ndo_deblur
);

`include "vh/n64adv_vparams.vh"

input VCLK;
input nVDSYNC;

input nRST;

input [`VDATA_I_FU_SLICE] vdata_pre;
input [color_width_i-1:0] VD_i;
input [ 3:0] vid_state_i; // order: data_cnt,vmode,n64_480i

input  [15:0] algorithmsettings_i;  // {(1bit reserve), P2P-Sens, FrameCnt (3bit), Dead-Zone (3bit), (2bit reserve), Stability/TH (2bit), Reset (2bit), VI-DeBlur (2bit)}
output reg  ndo_deblur = 1'b0;


// some pre-assignments and definitions
wire [1:0] data_cnt = vid_state_i[3:2];
wire          vmode = vid_state_i[  1];
wire       n64_480i = vid_state_i[  0];

wire negedge_nVSYNC =  vdata_pre[3*color_width_i+3] & !VD_i[3];
wire posedge_nCSYNC = !vdata_pre[3*color_width_i  ] &  VD_i[0];

wire [2:0] Rcmp_pre = vdata_pre[3*color_width_i-1:3*color_width_i-3];
wire [2:0] Gcmp_pre = vdata_pre[2*color_width_i-1:2*color_width_i-3];
wire [2:0] Bcmp_pre = vdata_pre[  color_width_i-1:  color_width_i-3];

wire [2:0] Rcmp_cur = VD_i[color_width_i-1:color_width_i-3];
wire [2:0] Gcmp_cur = VD_i[color_width_i-1:color_width_i-3];
wire [2:0] Bcmp_cur = VD_i[color_width_i-1:color_width_i-3];


wire                p2p_sens =  ~algorithmsettings_i[14];
wire [2:0]   nblur_trend_add =   algorithmsettings_i[13:11] + 2'b10;
wire [2:0]   nblur_trend_sub =   nblur_trend_add - (algorithmsettings_i[10: 8] + 1'b1);
wire [3:0]      nblur_th_bit =   algorithmsettings_i[ 5:4] + 4'h6;
wire [1:0] nblur_rstalg_mode =   algorithmsettings_i[ 3:2];
wire              nDeBlurMan =  ~algorithmsettings_i[   1];
wire            nForceDeBlur = ~|algorithmsettings_i[ 1:0];


// some more definitions for the heuristics

localparam init_trend_p = 10'h100;  // initial value


// start of rtl

reg blur_pix = 1'b0;

always @(posedge VCLK or negedge nRST)
  if (!nRST)
    blur_pix <= 1'b0;
  else if (!nVDSYNC) begin
    if(posedge_nCSYNC) // posedge nCSYNC -> reset blanking
      blur_pix <= ~vmode;
    else
      blur_pix <= ~blur_pix;
  end

// wire nRST_Alg = nblur_rstalg_mode == 2'00  ? 1'b0 :
//                 nblur_rstalg_mode == 2'b01 ? n64_480i :
//                 nblur_rstalg_mode == 2'b10 ? nRST :
//                                              nRST & n64_480i;
wire nRST_Alg = &{~nblur_rstalg_mode || {nRST,n64_480i}};

reg run_estimation = 1'b0;  // do not use first frame after switching to 240p (e.g. from 480i)

reg [1:0] gradient[2:0];  // shows the (sharp) gradient direction between neighbored pixels
                          // gradient[x][1]   = 1 -> decreasing intensity
                          // gradient[x][0]   = 1 -> increasing intensity
                          // else                 -> constant
reg [1:0] gradient_changes = 2'b00; // value is 2'b11 if all gradients has been changed

reg [1:0] nblur_est_cnt     = 2'b00;  // register to estimate whether blur is used or not by the N64
reg [9:0] nblur_trend = 10'b0; // trend shows if the algorithm tends to estimate more blur enabled rather than disabled
                                      // this acts as like as a very simple mean filter
reg nblur_n64 = 1'b1;                 // blur effect is estimated to be off within the N64 if value is 1'b1

reg first_init_trendval = 1'b1;
reg init_trendval = 1'b1;

always @(posedge VCLK or negedge nRST_Alg)
  if (!nRST_Alg) begin
    run_estimation <= 1'b0;
    nblur_trend    <= 10'b0;
    nblur_n64      <= 1'b1;
    init_trendval  <= 1'b1;
  end else begin
    if (!n64_480i) begin
      if (!nVDSYNC) begin
        if(negedge_nVSYNC) begin  // negedge at nVSYNC detected - new frame
          if (run_estimation) begin
            if (nblur_est_cnt >= nblur_trend_add)                 // add to weight
              nblur_trend <= &nblur_trend ? nblur_trend :         // saturate if needed
                                            nblur_trend + 1'b1;
            else if (nblur_est_cnt <= nblur_trend_sub)            // subtract
              nblur_trend <= |nblur_trend ? nblur_trend - 1'b1 :  // saturate if needed
                                            nblur_trend;

            nblur_n64 <= nblur_trend[nblur_th_bit];
          end

          if (init_trendval) begin
            if (first_init_trendval)
              nblur_trend[8] <= 1'b1;
            else
              nblur_trend[nblur_th_bit] <= 1'b1;
            first_init_trendval <= 1'b0;
            init_trendval <= 1'b0;
          end

          nblur_est_cnt  <= 2'b00;
          run_estimation <= 1'b1;
        end

        if(!blur_pix) begin  // incomming (potential) blurry pixel
                             // (blur_pix changes on next @(negedge VCLK))

          if (gradient_changes >= {p2p_sens,1'b1}  && ~&nblur_est_cnt)  // evaluate gradients (and saturate counter if needed)
            nblur_est_cnt <= nblur_est_cnt +1'b1;

          gradient_changes    <= 2'b00; // reset of gradients
        end
      end else begin
        if (blur_pix) begin
          case(data_cnt)
            2'b01: gradient[2] <= {(Rcmp_pre < Rcmp_cur),(Rcmp_pre > Rcmp_cur)};
            2'b10: gradient[1] <= {(Gcmp_pre < Gcmp_cur),(Gcmp_pre > Gcmp_cur)};
            2'b11: gradient[0] <= {(Bcmp_pre < Bcmp_cur),(Bcmp_pre > Bcmp_cur)};
          endcase
        end else begin
          case(data_cnt)
            2'b01: if (&(gradient[2] ^ {(Rcmp_pre < Rcmp_cur),(Rcmp_pre > Rcmp_cur)})) gradient_changes <= 2'b01;
            2'b10: if (&(gradient[1] ^ {(Gcmp_pre < Gcmp_cur),(Gcmp_pre > Gcmp_cur)})) gradient_changes <= gradient_changes + 1'b1;
            2'b11: if (&(gradient[0] ^ {(Bcmp_pre < Bcmp_cur),(Bcmp_pre > Bcmp_cur)})) gradient_changes <= gradient_changes + 1'b1;
          endcase
        end
      end
    end else begin
      run_estimation <= 1'b0;
    end
  end


// finally the blanking management

always @(posedge VCLK or negedge nRST)
  if (!nRST) begin
    ndo_deblur <= 1'b0;
  end else if (!nVDSYNC) begin
    if (negedge_nVSYNC) begin // negedge at nVSYNC detected - new frame, new setting
      if (nForceDeBlur)
        ndo_deblur <= n64_480i | nblur_n64;
      else
        ndo_deblur <= n64_480i | nDeBlurMan;
    end
  end

endmodule
