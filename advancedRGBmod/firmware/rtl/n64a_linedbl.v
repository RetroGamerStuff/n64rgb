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
// Target Devices: Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    simple line-multiplying
//
// Dependencies: vh/n64a_params.vh
//               ip/fpga_family/altpll_0.qip
//               rtl/n64a_ram2port.v
//
// Revision: 2.1
// Features: linebuffer for - NTSC 240p -> 480p rate conversion
//                          - PAL  288p -> 576p rate conversion
//           injection of scanlines
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

localparam ram_depth   = 11;
localparam Y_width     = color_width_o+1;
localparam SLHyb_width = 8; // do not change this localparam!

input  VCLK_in;
output VCLK_out;
input  nRST;

input [14:0] vinfo_dbl; // [nLinedbl,SL_in_osd,SLhyb_str (5bits),SL_str (4bits),SL_id,SL_en,PAL,interlaced]

input  [`VDATA_I_FU_SLICE] vdata_i;
output [`VDATA_O_FU_SLICE] vdata_o;


// pre-assignments

wire nVS_i = vdata_i[3*color_width_i+3];
wire nHS_i = vdata_i[3*color_width_i+1];

wire [color_width_i-1:0] R_i = vdata_i[`VDATA_I_RE_SLICE];
wire [color_width_i-1:0] G_i = vdata_i[`VDATA_I_GR_SLICE];
wire [color_width_i-1:0] B_i = vdata_i[`VDATA_I_BL_SLICE];

wire nENABLE_linedbl   = vinfo_dbl[14] | ~rdrun[1];
wire       SL_in_osd   = vinfo_dbl[13];
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


reg [ram_depth-1:0] hstart = `HSTART_NTSC;
reg [ram_depth-1:0] hstop  = `HSTOP_NTSC;

reg [6:0] nHS_width = `HS_WIDTH_NTSC_480I;


reg                 wren   = 1'b0;
reg                 wrline = 1'b0;
reg [ram_depth-1:0] wrhcnt = {ram_depth{1'b1}};
reg [ram_depth-1:0] wraddr = {ram_depth{1'b0}};

wire line_overflow = &{wrhcnt[ram_depth-1],wrhcnt[ram_depth-2],wrhcnt[ram_depth-5]};  // close approach for NTSC and PAL (equals 1600)
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
    if (nVS_i_buf & !nVS_i) begin
      // trigger new frame
      newFrame[0] <= ~newFrame[1];

      // trigger read start
      if (&{nHS_i_buf,!nHS_i,!line_overflow,valid_line})
        start_rdproc[0] <= ~start_rdproc[1];

      // set new info
//      case({pal_mode,n64_480i})
//        2'b00: begin
//            hstart    <= `HSTART_NTSC_240P;
//            hstop     <= `HSTOP_NTSC;
//            nHS_width <= `HS_WIDTH_NTSC_240P;
//          end
//        2'b01: begin
//            hstart    <= `HSTART_NTSC_480I;
//            hstop     <= `HSTOP_NTSC;
//            nHS_width <= `HS_WIDTH_NTSC_480I;
//          end
//        2'b10: begin
//            hstart    <= `HSTART_PAL_288P;
//            hstop     <= `HSTOP_PAL;
//            nHS_width <= `HS_WIDTH_PAL_288P;
//          end
//        2'b11: begin
//            hstart    <= `HSTART_PAL_576I;
//            hstop     <= `HSTOP_PAL;
//            nHS_width <= `HS_WIDTH_PAL_576I;
//          end
//      endcase
      if (pal_mode) begin
        hstart    <= `HSTART_PAL;
        hstop     <= `HSTOP_PAL;
        nHS_width <= n64_480i ? `HS_WIDTH_PAL_576I : `HS_WIDTH_PAL_288P;
      end else begin
        hstart    <= `HSTART_NTSC;
        hstop     <= `HSTOP_NTSC;
        nHS_width <= n64_480i ? `HS_WIDTH_NTSC_480I : `HS_WIDTH_NTSC_240P;
      end
    end

    if (nHS_i_buf & !nHS_i) begin // negedge nHSYNC -> reset wrhcnt and toggle wrline
      line_width[wrline] <= wrhcnt;

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
    if (line_overflow || &{nHS_i_buf,!nHS_i,!valid_line}) begin
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

n64a_ram2port #(.ram_depth(ram_depth), .data_width(3*color_width_i)) videobuffer_0(
  .wrCLK(VCLK_in),
  .wren(|{!wren,line_overflow,div_2x}), //  .wren(&{wren,!line_overflow,!div_2x}),
  .wraddr(wraddr),
  .wrdata({R_i,G_i,B_i}),
  .rdCLK(PX_CLK_4x),
  .rden(rden[0] && rdaddr[0]),
  .rdaddr(rdaddr),
  .rddata({R_buf,G_buf,B_buf})
);


reg      rdcnt_buf = 1'b0;
reg [8:0] vcnt_dbl = 9'd0;
reg [7:0]  nHS_cnt = 8'd0;
reg [1:0]  nVS_cnt = 2'b0;

wire CSen_lineend = ((rdhcnt + 2'b11) > (line_width[rdline] - nHS_width));

wire is_OSD_area = (rdhcnt[ram_depth-1:1] > (`OSD_WINDOW_H_START+2'b10)) && (rdhcnt[ram_depth-1:1] < (`OSD_WINDOW_H_STOP+2'b10)) &&
                   (vcnt_dbl[8:1] > (`OSD_WINDOW_V_START+2'b11)) && (vcnt_dbl[8:1] < (`OSD_WINDOW_V_STOP+2'b01));

reg                     drawSL;
reg               [3:0] S_dbl;
reg [color_width_i-1:0] R_dbl, G_dbl, B_dbl;
reg [Y_width-1:0] Y_dbl;

reg [SLHyb_width-1:0] SL_rval;

always @(posedge PX_CLK_4x) begin
  if (rdcnt_buf ^ rdcnt) begin
    S_dbl[0] <= 1'b0;
    S_dbl[1] <= 1'b0;
    S_dbl[2] <= 1'b1; // dummy

   vcnt_dbl <= ~&vcnt_dbl ? vcnt_dbl + 1'b1 : vcnt_dbl;
    nHS_cnt <= nHS_width;

    if (^newFrame) begin
      vcnt_dbl    <= 9'd0;
      nVS_cnt     <= `VS_WIDTH;
      S_dbl[3]    <= 1'b0;
      newFrame[1] <= newFrame[0];
      SL_rval <= ((SL_str+8'h01)<<4)-1'b1;
    end else if (|nVS_cnt) begin
      nVS_cnt <= nVS_cnt - 1'b1;
    end else begin
      S_dbl[3] <= 1'b1;
    end
  end else begin
    if (|nHS_cnt) begin
      nHS_cnt <= nHS_cnt - 1'b1;
    end else begin
      S_dbl[1] <= 1'b1;
      if (S_dbl[3])
        S_dbl[0] <= 1'b1;
    end
    
    if (CSen_lineend) begin
      S_dbl[0] <= 1'b1;
    end
  end

  rdcnt_buf <= rdcnt;
  drawSL <= (is_OSD_area ? SL_in_osd : 1'b1) && SL_en && (rdcnt ^ (!SL_id));

  if (rden[2]) begin
    R_dbl <= R_buf;
    G_dbl <= G_buf;
    B_dbl <= B_buf;
    Y_dbl <= {2'b00,R_buf} + {1'b0,G_buf,1'b0} + {2'b00,B_buf};
  end else begin
    R_dbl <= { color_width_i   {1'b0}};
    G_dbl <= { color_width_i   {1'b0}};;
    B_dbl <= { color_width_i   {1'b0}};;
    Y_dbl <= {(color_width_i+2){1'b0}};;
  end

end


// post-processing (scanline generation)

reg                     dSL_pp[0:4]                     /* synthesis ramstyle = "logic" */;
reg               [3:0] S_pp[0:4], S_o                  /* synthesis ramstyle = "logic" */;
reg [color_width_i-1:0] R_pp[0:4], G_pp[0:4], B_pp[0:4] /* synthesis ramstyle = "logic" */;
reg [color_width_o-1:0] R_o, G_o, B_o                   /* synthesis ramstyle = "logic" */;

wire [Y_width+4:0] Y_ref_pre_full = Y_dbl * SLHyb_depth;
reg  [Y_width-1:0] Y_ref_pre;

wire [Y_width+SLHyb_width-1:0] Y_ref_full = Y_ref_pre * SL_rval;
reg  [Y_width-1:0] Y_ref;

reg [SLHyb_width-1:0] SLHyb_rval;
reg [SLHyb_width-1:0] SLHyb_str;

wire [color_width_o+SLHyb_width-2:0] R_sl_full = R_pp[3] * SLHyb_str;
wire [color_width_o+SLHyb_width-2:0] G_sl_full = G_pp[3] * SLHyb_str;
wire [color_width_o+SLHyb_width-2:0] B_sl_full = B_pp[3] * SLHyb_str;
reg  [color_width_o-1:0]             R_sl, G_sl, B_sl;

integer pp_idx;

always @(posedge PX_CLK_4x) begin
  dSL_pp[0] <= drawSL;
    S_pp[0] <= S_dbl;
    R_pp[0] <= R_dbl;
    G_pp[0] <= G_dbl;
    B_pp[0] <= B_dbl;
  for (pp_idx = 0; pp_idx < 4; pp_idx = pp_idx + 1) begin
    dSL_pp[pp_idx+1] <= dSL_pp[pp_idx];
      S_pp[pp_idx+1] <=   S_pp[pp_idx];
      R_pp[pp_idx+1] <=   R_pp[pp_idx];
      G_pp[pp_idx+1] <=   G_pp[pp_idx];
      B_pp[pp_idx+1] <=   B_pp[pp_idx];
  end
  S_o <= S_pp[4];

  // hybrid strength reference (2 pp stages)
  Y_ref_pre <= Y_ref_pre_full[Y_width+4:5];
  Y_ref     <= Y_ref_full[Y_width+SLHyb_width-1:SLHyb_width];

  // adaptation of sl_str. (2 pp stages)
  SLHyb_rval <= {1'b0,SL_rval} < Y_ref ? 8'h0 : SL_rval - Y_ref[7:0];
  SLHyb_str  <= 8'hff - SLHyb_rval;
  
  // calculate SL (1 pp stage)
  R_sl <= R_sl_full[color_width_o+SLHyb_width-2:SLHyb_width-1];
  G_sl <= G_sl_full[color_width_o+SLHyb_width-2:SLHyb_width-1];
  B_sl <= B_sl_full[color_width_o+SLHyb_width-2:SLHyb_width-1];

  // set scanline
  if (dSL_pp[4]) begin
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