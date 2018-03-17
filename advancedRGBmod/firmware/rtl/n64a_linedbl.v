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
// Module Name:    n64a_linedbl
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Max10, Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    simple line-multiplying
//
// Dependencies: vh/n64a_params.vh
//               ip/altpll_0.qip
//               ip/ram2port_0.qip
//
// Revision: 1.2
// Features: linebuffer for - NTSC 240p -> 480p rate conversion
//                          - PAL  288p -> 576p rate conversion
//           injection of scanlines on demand in three selectable intensities
//
//////////////////////////////////////////////////////////////////////////////////


module n64a_linedbl(
  VCLK_in,
  VCLK_out,
  nRST,

  vinfo_dbl,

  vdata_i,
  vdata_o
);

`include "vh/n64a_params.vh"

localparam ram_depth = 11; // plus 1 due to oversampling

input  VCLK_in;
output VCLK_out;
input  nRST;

input [13:0] vinfo_dbl; // [nLinedbl,SLhyb_str (5bits),SL_str (4bits),SL_id,SL_en,PAL,interlaced]

input  [`VDATA_I_FU_SLICE] vdata_i;
output [`VDATA_O_FU_SLICE] vdata_o;


// pre-assignments

wire nVS_i = vdata_i[3*color_width_i+3];
wire nHS_i = vdata_i[3*color_width_i+1];

wire [color_width_i-1:0] R_i = vdata_i[`VDATA_I_RE_SLICE];
wire [color_width_i-1:0] G_i = vdata_i[`VDATA_I_GR_SLICE];
wire [color_width_i-1:0] B_i = vdata_i[`VDATA_I_BL_SLICE];

wire nENABLE_linedbl   = vinfo_dbl[13] | ~rdrun[1];
wire [4:0] SLHyb_depth = vinfo_dbl[12:8];
wire [3:0] SL_str      = vinfo_dbl[ 7:4];
wire       SL_id       = vinfo_dbl[ 3];
wire       SL_en       = vinfo_dbl[ 2];
wire       pal_mode    = vinfo_dbl[ 1];
wire       n64_480i    = vinfo_dbl[ 0];

// start of rtl


reg div_2x = 1'b0;

always @(posedge VCLK_in) begin
  div_2x <= ~div_2x;
end


wire PX_CLK_4x, PLL_locked;
altpll_0 vid_pll(
  .inclk0(VCLK_in),
  .areset(~nRST),
  .locked(PLL_locked),
  .c0(PX_CLK_4x)
);

wire RST = ~(nRST && PLL_locked);


reg [ram_depth-1:0] hstart = `HSTART_NTSC_480I;
reg [ram_depth-1:0] hstop  = `HSTOP_NTSC;

reg [6:0] nHS_width = `HS_WIDTH_NTSC_480I;


reg                 wren   = 1'b0;
reg                 wrline = 1'b0;
reg [ram_depth-1:0] wrhcnt = {ram_depth{1'b1}};
reg [ram_depth-1:0] wraddr = {ram_depth{1'b0}};

wire line_overflow = &{wrhcnt[ram_depth-1],wrhcnt[ram_depth-2],wrhcnt[ram_depth-5]};  // close approach for NTSC and PAL
wire valid_line    = wrhcnt > hstop;                                                  // for evaluation


reg [ram_depth-1:0] line_width[0:1];
initial begin
   line_width[1] = {ram_depth{1'b0}};
   line_width[0] = {ram_depth{1'b0}};
end

reg  nVS_i_buf = 1'b0;
reg  nHS_i_buf = 1'b0;


reg [1:0]     newFrame = 2'b0;
reg [1:0] start_rdproc = 2'b00;
reg [1:0]  stop_rdproc = 2'b00;


always @(posedge VCLK_in) begin
  if (!div_2x) begin
    if (nVS_i_buf & ~nVS_i) begin
      // trigger new frame
      newFrame[0] <= ~newFrame[1];

      // trigger read start
      if (&{nHS_i_buf,~nHS_i,~line_overflow,valid_line})
        start_rdproc[0] <= ~start_rdproc[1];

      // set new info
      case({pal_mode,n64_480i})
        2'b00: begin
            hstart    <= `HSTART_NTSC_240P;
            hstop     <= `HSTOP_NTSC;
            nHS_width <= `HS_WIDTH_NTSC_240P;
          end
        2'b01: begin
            hstart    <= `HSTART_NTSC_480I;
            hstop     <= `HSTOP_NTSC;
            nHS_width <= `HS_WIDTH_NTSC_480I;
          end
        2'b10: begin
            hstart    <= `HSTART_PAL_288P;
            hstop     <= `HSTOP_PAL;
            nHS_width <= `HS_WIDTH_PAL_288P;
          end
        2'b11: begin
            hstart    <= `HSTART_PAL_576I;
            hstop     <= `HSTOP_PAL;
            nHS_width <= `HS_WIDTH_PAL_576I;
          end
      endcase
    end

    if (nHS_i_buf & !nHS_i) begin // negedge nHSYNC -> reset wrhcnt and toggle wrline
      line_width[wrline] <= wrhcnt[ram_depth-1:0];

      wrhcnt <= {ram_depth{1'b0}};
      wrline <= ~wrline;
    end else if (~line_overflow) begin
      wrhcnt <= wrhcnt + 1'b1;
    end

    if (wrhcnt == hstart) begin
      wren   <= 1'b1;
      wraddr <= {ram_depth{1'b0}};
    end else if (wrhcnt > hstart && wrhcnt < hstop) begin
      wraddr <= wraddr + 1'b1;
    end else begin
      wren   <= 1'b0;
      wraddr <= {ram_depth{1'b0}};
    end

    nVS_i_buf <= nVS_i;
    nHS_i_buf <= nHS_i;
  end

  if (RST) begin
        newFrame[0] <= newFrame[1];
    start_rdproc[0] <= start_rdproc[1];
     stop_rdproc[0] <= ~stop_rdproc[1];

    wren   <= 1'b0;
    wrline <= 1'b0;
    wrhcnt <= {ram_depth{1'b1}};
  end
end


reg           [2:0] rden     = 3'b0;
reg           [1:0] rdrun    = 2'b00;
reg                 rdcnt    = 1'b0;
reg                 rdline   = 1'b0;
reg [ram_depth-1:0] rdhcnt   = {ram_depth{1'b1}};
reg [ram_depth-1:0] rdaddr   = {ram_depth{1'b0}};

always @(posedge PX_CLK_4x) begin
  if (rdrun[1]) begin
    if (rdhcnt == line_width[rdline]) begin
      rdhcnt   <= {ram_depth{1'b0}};
      if (rdcnt)
        rdline <= wrline;
      rdcnt <= ~rdcnt;
    end else begin
      rdhcnt <= rdhcnt + 1'b1;
    end
    if (line_overflow || &{nHS_i_buf,~nHS_i,~valid_line}) begin
      rdrun <= 2'b00;
    end

    if (rdhcnt == hstart) begin
      rden[0] <= 1'b1;
      rdaddr  <= {ram_depth{1'b0}};
    end else if (rdhcnt > hstart && rdhcnt < hstop) begin
      rdaddr <= rdaddr + 1'b1;
    end else begin
      rden[0] <= 1'b0;
      rdaddr  <= {ram_depth{1'b0}};
    end
  end else if (rdrun[0] && wrhcnt[3]) begin
    rdrun[1] <= 1'b1;
    rdcnt    <= 1'b1;
    rdline   <= ~wrline;
    rdhcnt   <= {ram_depth{1'b0}};
  end else if (^start_rdproc) begin
    rdrun[0] <= 1'b1;
  end
  
  rden[2:1] <= rden[1:0];

  if (^stop_rdproc) begin
    rden  <= 3'b0;
    rdrun <= 2'b0;
  end

  start_rdproc[1] <= start_rdproc[0];
   stop_rdproc[1] <=  stop_rdproc[0];
end


wire [color_width_i-1:0] R_buf, G_buf, B_buf;

ram2port_0 videobuffer_0(
  .data({R_i,G_i,B_i}),
  .rdaddress(rdaddr),
  .rdclock(PX_CLK_4x),
  .rden(rden[0]),
  .wraddress(wraddr),
  .wrclock(VCLK_in),
  .wren(&{wren,~line_overflow,~div_2x}),
  .q({R_buf,G_buf,B_buf})
);


reg     rdcnt_buf = 1'b0;
reg [7:0] nHS_cnt = 8'd0;
reg [1:0] nVS_cnt = 2'b0;

wire CSen_lineend = ((rdhcnt + 2'b11) > (line_width[rdline] - nHS_width));

reg               [3:0] S_ldb;
reg [color_width_i-1:0] R_ldb, G_ldb, B_ldb;
reg [color_width_i+1:0] Y_ldb;

reg [7:0] SL_rval;

always @(posedge PX_CLK_4x) begin
  if (rdcnt_buf ^ rdcnt) begin
    S_ldb[0] <= 1'b0;
    S_ldb[1] <= 1'b0;
    S_ldb[2] <= 1'b1; // dummy

    nHS_cnt <= nHS_width;

    if (^newFrame) begin
      nVS_cnt  <= `VS_WIDTH;
      S_ldb[3]   <= 1'b0;
      newFrame[1] <= newFrame[0];
      SL_rval <= ((SL_str+8'h01)<<4)-1'b1;
    end else if (|nVS_cnt) begin
      nVS_cnt <= nVS_cnt - 1'b1;
    end else begin
      S_ldb[3] <= 1'b1;
    end
  end else begin
    if (|nHS_cnt) begin
      nHS_cnt <= nHS_cnt - 1'b1;
    end else begin
      S_ldb[1] <= 1'b1;
      if (S_ldb[3])
        S_ldb[0] <= 1'b1;
    end
    
    if (CSen_lineend) begin
      S_ldb[0] <= 1'b1;
    end
  end

  rdcnt_buf <= rdcnt;

  if (rden[2]) begin
	  R_ldb <= R_buf;
	  G_ldb <= G_buf;
	  B_ldb <= B_buf;
	  Y_ldb <= {2'b00,R_buf} + {1'b0,G_buf,1'b0} + {2'b00,B_buf};
  end else begin
	  R_ldb <= { color_width_i   {1'b0}};
	  G_ldb <= { color_width_i   {1'b0}};;
	  B_ldb <= { color_width_i   {1'b0}};;
	  Y_ldb <= {(color_width_i+2){1'b0}};;
  end

end

// post-processing (scanline generation)

reg               [3:0] S_pp[0:4], S_o /* synthesis ramstyle = "logic" */;
reg [color_width_i-1:0] R_pp[0:4], G_pp[0:4], B_pp[0:4] /* synthesis ramstyle = "logic" */;
reg [color_width_o-1:0] R_o, G_o, B_o /* synthesis ramstyle = "logic" */;

wire [8:0] Y_ref_pre;
lpm_mult_0 calc_SLHyb_ref_pre(
  .clock(PX_CLK_4x),
  .dataa(Y_ldb),
  .datab(SLHyb_depth),
  .result(Y_ref_pre),
  .aclr(nENABLE_linedbl)
);

wire [8:0] Y_ref;
lpm_mult_1 calc_SLHyb_ref(
  .clock(PX_CLK_4x),
  .dataa(Y_ref_pre),
  .datab(SL_rval),
  .result(Y_ref),
  .aclr(nENABLE_linedbl)
);

reg [7:0] SLHyb_rval;
reg [7:0] SLHyb_str;

wire [color_width_o-1:0] R_sl, G_sl, B_sl;

lpm_mult_2 calc_R_sl(
  .clock(PX_CLK_4x),
  .dataa(R_pp[3]),
  .datab(SLHyb_str),
  .result(R_sl),
  .aclr(nENABLE_linedbl)
);

lpm_mult_2 calc_G_sl(
  .clock(PX_CLK_4x),
  .dataa(G_pp[3]),
  .datab(SLHyb_str),
  .result(G_sl),
  .aclr(nENABLE_linedbl)
);

lpm_mult_2 calc_B_sl(
  .clock(PX_CLK_4x),
  .dataa(B_pp[3]),
  .datab(SLHyb_str),
  .result(B_sl),
  .aclr(nENABLE_linedbl)
);


integer pp_idx;

always @(posedge PX_CLK_4x) begin
  S_pp[0] <= S_ldb;
  R_pp[0] <= R_ldb;
  G_pp[0] <= G_ldb;
  B_pp[0] <= B_ldb;
  for (pp_idx = 0; pp_idx < 4; pp_idx = pp_idx + 1) begin
    S_pp[pp_idx+1] <= S_pp[pp_idx];
    R_pp[pp_idx+1] <= R_pp[pp_idx];
    G_pp[pp_idx+1] <= G_pp[pp_idx];
    B_pp[pp_idx+1] <= B_pp[pp_idx];
  end
  S_o <= S_pp[4];

  // hybrid strength reference (2 pp stages)
  
  // adaptation of sl_str. (2 pp stages)
  SLHyb_rval <= {1'b0,SL_rval} < Y_ref ? 8'h0 : SL_rval - Y_ref[7:0];
  SLHyb_str  <= 8'hff - SLHyb_rval;
  
  // calculate SL (1 pp stage)
  
  // set scanline
  if (SL_en & (rdcnt ^ (!SL_id))) begin
    R_o <= R_sl;
    G_o <= G_sl;
    B_o <= B_sl;
  end else begin
    R_o <= {R_pp[4],R_pp[4][color_width_i-1]};
    G_o <= {G_pp[4],G_pp[4][color_width_i-1]};
	 B_o <= {B_pp[4],B_pp[4][color_width_i-1]};
  end

  // use standard input if no line-doubling
  if (nENABLE_linedbl) begin
    S_o <= vdata_i[`VDATA_I_SY_SLICE];
    R_o <= {R_i,R_i[color_width_i-1]};
    G_o <= {G_i,G_i[color_width_i-1]};
    B_o <= {B_i,B_i[color_width_i-1]};
  end
end


// post-assignment

assign VCLK_out = PX_CLK_4x;
assign vdata_o = {S_o,R_o,G_o,B_o};

endmodule 