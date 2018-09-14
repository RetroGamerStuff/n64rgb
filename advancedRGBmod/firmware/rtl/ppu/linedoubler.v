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
// Module Name:    linedoubler
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    simple line-multiplying
//
//////////////////////////////////////////////////////////////////////////////////


module linedoubler(
  VCLK,
  nRST,
  VCLK_Tx,

  vinfo_dbl,

  vdata_i,
  vdata_o
);

`include "vh/n64adv_vparams.vh"

localparam hcnt_witdh  = $clog2(`PIXEL_PER_LINE_2x_MAX);
localparam Y_width     = color_width_o+1;
localparam SLHyb_width = 8; // do not change this localparam!

input VCLK;
input nRST;
input VCLK_Tx;

input [15:0] vinfo_dbl; // [nLinedbl,SL_in_osd,SLhyb_str (5bits),SL_str (4bits),SL_method,SL_id,SL_en,PAL,interlaced]

input  [`VDATA_I_FU_SLICE] vdata_i;
output [`VDATA_O_FU_SLICE] vdata_o;


// pre-assignments

wire nVS_i = vdata_i[3*color_width_i+3];
wire nHS_i = vdata_i[3*color_width_i+1];

wire [color_width_i-1:0] R_i = vdata_i[`VDATA_I_RE_SLICE];
wire [color_width_i-1:0] G_i = vdata_i[`VDATA_I_GR_SLICE];
wire [color_width_i-1:0] B_i = vdata_i[`VDATA_I_BL_SLICE];

wire nENABLE_linedbl   = vinfo_dbl[15] | ~rdrun[1];
wire       SL_in_osd   = vinfo_dbl[14];
wire [4:0] SLHyb_depth = vinfo_dbl[13:9];
wire [3:0] SL_str      = vinfo_dbl[ 8:5];
wire       SL_method   = vinfo_dbl[ 4];
wire       SL_id       = vinfo_dbl[ 3];
wire       SL_en       = vinfo_dbl[ 2];
wire       pal_mode    = vinfo_dbl[ 1];
wire       n64_480i    = vinfo_dbl[ 0];


// start of rtl

reg nVDSYNC_dbl = 1'b0;

reg  nVS_i_buf = 1'b0;
reg  nHS_i_buf = 1'b0;

wire negedge_nHSYNC =  nHS_i_buf & !nHS_i;
//wire posedge_nHSYNC = !nHS_i_buf &  nHS_i;
wire negedge_nVSYNC =  nVS_i_buf & !nVS_i;

reg [SLHyb_width-1:0] SL_rval = {SLHyb_width{1'b0}};

always @(posedge VCLK) begin
  nVDSYNC_dbl <= ~nVDSYNC_dbl;

  if (!nVDSYNC_dbl) begin
    nHS_i_buf <= nHS_i;
    nVS_i_buf <= nVS_i;

    if (negedge_nVSYNC) begin
      FrameID <= negedge_nHSYNC; // negedge at nHSYNC, too -> odd frame
      SL_rval <= ((SL_str+8'h01)<<4)-1'b1;
    end
  end

  if (!nRST) begin
    nVDSYNC_dbl <= 1'b0;
    nHS_i_buf  <= 1'b0;
    nVS_i_buf  <= 1'b0;
    FrameID    <= 1'b0;
  end
end


reg [hcnt_witdh-1:0] hstart = `HSTART_NTSC;
reg [hcnt_witdh-1:0] hstop  = `HSTOP_NTSC;

reg [6:0] nHS_width = `HS_WIDTH_NTSC_480I;


localparam pcnt_width = $clog2(`BUF_NUM_OF_PAGES);

reg                  wren   = 1'b0;
reg [pcnt_width-1:0] wrpage = {pcnt_width{1'b0}};
reg [hcnt_witdh-1:0] wrhcnt = {hcnt_witdh{1'b1}};
reg [hcnt_witdh-1:0] wraddr = {hcnt_witdh{1'b0}};

wire line_overflow = wrhcnt == `PIXEL_PER_LINE_2x_MAX;  // close approach for NTSC and PAL (equals 1600)
wire valid_line    = wrhcnt > hstop;                    // for evaluation


reg [hcnt_witdh-1:0] line_width[0:`BUF_NUM_OF_PAGES-1];
integer int_idx;
initial begin
  for (int_idx = 0; int_idx < `BUF_NUM_OF_PAGES; int_idx = int_idx+1)
    line_width[int_idx] = {hcnt_witdh{1'b0}};
end


reg [1:0]     newFrame = 2'b00;
reg            FrameID = 1'b0;
reg [1:0] start_rdproc = 2'b00;


always @(posedge VCLK) begin
  if (!nVDSYNC_dbl) begin
    if (negedge_nVSYNC) begin
      // trigger new frame
      newFrame[0] <= ~newFrame[1];

      // set new info
      if (pal_mode) begin
        hstart    <= `HSTART_PAL;
        hstop     <= `HSTOP_PAL;
        nHS_width <= n64_480i ? `HS_WIDTH_PAL_576I : `HS_WIDTH_PAL_288P;
      end else begin
        hstart    <= `HSTART_NTSC;
        hstop     <= `HSTOP_NTSC;
        nHS_width <= n64_480i ? `HS_WIDTH_NTSC_480I : `HS_WIDTH_NTSC_240P;
      end

      if (negedge_nHSYNC)
        start_rdproc[0] <= ~start_rdproc[1];  // trigger read start
    end


    if (negedge_nHSYNC) begin // negedge nHSYNC -> reset wrhcnt and inc. wrpage
      line_width[wrpage] <= wrhcnt;
      wrhcnt <= {hcnt_witdh{1'b0}};
      if (wrpage == `BUF_NUM_OF_PAGES-1)
        wrpage = {pcnt_width{1'b0}};
      else
        wrpage <= wrpage + 1'b1;
      if (!rdrun[0])
        wrpage <= {pcnt_width{1'b0}};
    end else if (~line_overflow) begin
      wrhcnt <= wrhcnt + 1'b1;
    end

    if (wrhcnt == hstart) begin
      wren   <= 1'b1;
      wraddr <= {hcnt_witdh{1'b0}};
    end else if (wrhcnt > hstart && wrhcnt < hstop) begin
      wraddr <= wraddr + 1'b1;
    end else begin
      wren   <= 1'b0;
      wraddr <= {hcnt_witdh{1'b0}};
    end
  end

  if (!nRST) begin
        newFrame[0] <= newFrame[1];
    start_rdproc[0] <= start_rdproc[1];

    wren   <= 1'b0;
    wrpage <= {pcnt_width{1'b0}};
    wrhcnt <= {hcnt_witdh{1'b1}};
  end
end


reg            [2:0] rden    = 3'b0;
reg            [1:0] rdrun   = 2'b00;
reg                  rdcnt   = 1'b0;
reg [pcnt_width-1:0] rdpage  = {pcnt_width{1'b0}};
reg [hcnt_witdh-1:0] rdhcnt  = {hcnt_witdh{1'b1}};
reg [hcnt_witdh-1:0] rdaddr  = {hcnt_witdh{1'b0}};

always @(posedge VCLK_Tx) begin
  if (rdrun[1]) begin
    if (rdhcnt == line_width[rdpage_pp1]) begin
      rdhcnt   <= {hcnt_witdh{1'b0}};
      if (rdcnt)
        if (rdpage == `BUF_NUM_OF_PAGES-1)
          rdpage = {pcnt_width{1'b0}};
        else
          rdpage <= rdpage + 1'b1;
      rdcnt <= ~rdcnt;
    end else begin
      rdhcnt <= rdhcnt + 1'b1;
    end

    if (rdhcnt == hstart) begin
      rden[0] <= 1'b1;
      rdaddr  <= {hcnt_witdh{1'b0}};
    end else if (rdhcnt > hstart && rdhcnt < hstop) begin
      rdaddr <= rdaddr + 1'b1;
    end else begin
      rden[0] <= 1'b0;
      rdaddr  <= {hcnt_witdh{1'b0}};
    end
  end else if (rdrun[0] && !nVDSYNC_dbl &&
               (wrpage == (`BUF_NUM_OF_PAGES>>1))) begin
    rdrun[1] <= 1'b1;
    rdcnt    <= 1'b0;
    rdpage   <= 2'b00;
    rdhcnt   <= {hcnt_witdh{1'b0}};
  end else if (^start_rdproc) begin
    rdrun[0] <= 1'b1;
  end
  
  rden[2:1] <= rden[1:0];
  if (!nRST || line_overflow ||
      (negedge_nHSYNC & !valid_line)) begin // reset conditions
    rden  <= 3'b0;
    rdrun <= 2'b0;
  end

  start_rdproc[1] <= start_rdproc[0];
end

wire [pcnt_width-1:0] rdpage_pp0 = (FrameID | !n64_480i) ? rdpage : rdpage - !rdcnt;
wire [pcnt_width-1:0] rdpage_pp1 = rdpage_pp0 >= `BUF_NUM_OF_PAGES ? `BUF_NUM_OF_PAGES-1 : rdpage_pp0;
wire [pcnt_width-1:0] rdpage_pp2 = (!SL_method | n64_480i) ? rdpage_pp1 :  // do not allow advanced scanlines in 480i linex2 mode
                                   !rdaddr[0] ? rdpage_pp1 : !SL_id ? rdpage_pp1 - 1'b1 : rdpage_pp1 + 1'b1;
wire [pcnt_width-1:0] rdpage_pp3 = !SL_id ? (rdpage_pp2 >= `BUF_NUM_OF_PAGES ? `BUF_NUM_OF_PAGES-1 : rdpage_pp2) :
                                            (rdpage_pp2 == `BUF_NUM_OF_PAGES ?  {pcnt_width{1'b0}} : rdpage_pp2);

wire [color_width_i-1:0] R_buf, G_buf, B_buf;

ram2port #(
  .num_of_pages(`BUF_NUM_OF_PAGES),
  .pagesize(`BUF_DEPTH_PER_PAGE),
  .data_width(3*color_width_i)
) videobuffer_u(
  .wrCLK(VCLK),
  .wren(&{wren,!line_overflow,nVDSYNC_dbl,!wraddr[0]}),
  .wrpage(wrpage),
  .wraddr(wraddr[hcnt_witdh-1:1]),
  .wrdata({R_i,G_i,B_i}),
  .rdCLK(VCLK_Tx),
  .rden(rden[0]),
  .rdpage(rdpage_pp3),
  .rdaddr(rdaddr[hcnt_witdh-1:1]),
  .rddata({R_buf,G_buf,B_buf})
);


localparam vs_delay_w = $clog2(`BUF_NUM_OF_PAGES);

reg [           3:0] rdcnt_buf      = 4'h0;
reg [vs_delay_w-1:0] newFrame_delay = {vs_delay_w{1'b0}};
reg [           8:0] vcnt_dbl       = 9'd0;

reg [7:0] nHS_cnt = 8'd0;
reg [1:0] nVS_cnt = 2'b0;

wire CSen_lineend = ((rdhcnt + 2'b11) > (line_width[rdpage_pp1] - nHS_width));

wire is_OSD_area = (  rdhcnt > ({`OSD_WINDOW_H_START,1'b0}+4'h8)) && (  rdhcnt < ({`OSD_WINDOW_H_STOP,1'b0}+4'h7)) &&
                   (vcnt_dbl > ({`OSD_WINDOW_V_START,1'b0}+4'h2)) && (vcnt_dbl < ({`OSD_WINDOW_V_STOP,1'b0}+4'h1));

reg                     drawSL;
reg               [3:0] S_dbl;
reg [color_width_i-1:0] R_dbl_pre, G_dbl_pre, B_dbl_pre,
                        R_sl_pre,  G_sl_pre,  B_sl_pre;
reg [color_width_i-1:0] R_dbl, G_dbl, B_dbl;
reg [Y_width-1:0] Y_sl_pre;

wire [color_width_i-1:0] R_avg = {1'b0,R_dbl_pre[color_width_i-1:1]} + {1'b0,R_buf[color_width_i-1:1]} + (R_dbl_pre[0] ^ R_buf[0]);
wire [color_width_i-1:0] G_avg = {1'b0,G_dbl_pre[color_width_i-1:1]} + {1'b0,G_buf[color_width_i-1:1]} + (G_dbl_pre[0] ^ G_buf[0]);
wire [color_width_i-1:0] B_avg = {1'b0,B_dbl_pre[color_width_i-1:1]} + {1'b0,B_buf[color_width_i-1:1]} + (B_dbl_pre[0] ^ B_buf[0]);

always @(posedge VCLK_Tx) begin
  if (^rdcnt_buf[3:2]) begin
    S_dbl[0] <= 1'b0;
    S_dbl[1] <= 1'b0;
    S_dbl[2] <= 1'b1; // dummy

   vcnt_dbl <= ~&vcnt_dbl ? vcnt_dbl + 1'b1 : vcnt_dbl;
    nHS_cnt <= nHS_width;
    
    if (newFrame_delay == `BUF_NUM_OF_PAGES) begin
      vcnt_dbl    <= 9'd0;
      nVS_cnt     <= `VS_WIDTH;
      S_dbl[3]    <= 1'b0;
    end else if (|nVS_cnt) begin
      nVS_cnt <= nVS_cnt - 1'b1;
    end else begin
      S_dbl[3] <= 1'b1;
    end

    newFrame_delay <= |newFrame_delay ? newFrame_delay + 1'b1 : {vs_delay_w{1'b0}};
    if (^newFrame) begin
      newFrame_delay <= {{(vs_delay_w-1){1'b0}},1'b1};
      newFrame[1] <= newFrame[0];
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

  rdcnt_buf <= {rdcnt_buf[2:0],rdcnt};
  drawSL <= (is_OSD_area ? SL_in_osd : 1'b1) && SL_en && (rdcnt ^ (!SL_id));

  if (rden[2]) begin
    if (!rdaddr[0]) begin // reading buffer has exactly two delay steps - so we can safetely use !rdaddr[0]
      R_dbl_pre <= R_buf;
      G_dbl_pre <= G_buf;
      B_dbl_pre <= B_buf;
    end else begin
      R_sl_pre <= R_avg;
      G_sl_pre <= G_avg;
      B_sl_pre <= B_avg;
      Y_sl_pre <= {2'b00,R_avg} + {1'b0,G_avg,1'b0} + {2'b00,B_avg};
    end
  end else begin
    R_dbl_pre <= {color_width_i{1'b0}};
    G_dbl_pre <= {color_width_i{1'b0}};
    B_dbl_pre <= {color_width_i{1'b0}};
    R_dbl_pre <= {color_width_i{1'b0}};
    G_dbl_pre <= {color_width_i{1'b0}};
    B_dbl_pre <= {color_width_i{1'b0}};
    R_sl_pre  <= {color_width_i{1'b0}};
    G_sl_pre  <= {color_width_i{1'b0}};
    B_sl_pre  <= {color_width_i{1'b0}};
    Y_sl_pre  <= {      Y_width{1'b0}};
  end

  R_dbl <= R_dbl_pre;
  G_dbl <= G_dbl_pre;
  B_dbl <= B_dbl_pre;
end


// post-processing (scanline generation)

reg                     dSL_pp[0:4]                     /* synthesis ramstyle = "logic" */;
reg               [3:0] S_pp[0:4], S_o                  /* synthesis ramstyle = "logic" */;
reg [color_width_i-1:0] R_sl_pre_pp[0:3],
                        G_sl_pre_pp[0:3],
                        B_sl_pre_pp[0:3]                /* synthesis ramstyle = "logic" */;
reg [color_width_i-1:0] R_pp[0:4], G_pp[0:4], B_pp[0:4] /* synthesis ramstyle = "logic" */;
reg [color_width_o-1:0] R_o, G_o, B_o                   /* synthesis ramstyle = "logic" */;

wire [Y_width+4:0] Y_ref_pre_full = Y_sl_pre * (* multstyle = "dsp" *) SLHyb_depth;
reg  [Y_width-1:0] Y_ref_pre;

wire [Y_width+SLHyb_width-1:0] Y_ref_full = Y_ref_pre * (* multstyle = "dsp" *) SL_rval;
reg  [Y_width-1:0] Y_ref;

reg [SLHyb_width-1:0] SLHyb_rval;
reg [SLHyb_width-1:0] SLHyb_str;

wire [color_width_o+SLHyb_width-2:0] R_sl_full = R_sl_pre_pp[3] * (* multstyle = "dsp" *) SLHyb_str;
wire [color_width_o+SLHyb_width-2:0] G_sl_full = G_sl_pre_pp[3] * (* multstyle = "dsp" *) SLHyb_str;
wire [color_width_o+SLHyb_width-2:0] B_sl_full = B_sl_pre_pp[3] * (* multstyle = "dsp" *) SLHyb_str;
reg  [color_width_o-1:0]             R_sl, G_sl, B_sl;

integer pp_idx;

always @(posedge VCLK_Tx) begin
       dSL_pp[0] <= drawSL;
         S_pp[0] <= S_dbl;
  R_sl_pre_pp[0] <= R_sl_pre;
  G_sl_pre_pp[0] <= G_sl_pre;
  B_sl_pre_pp[0] <= B_sl_pre;
         R_pp[0] <= R_dbl;
         G_pp[0] <= G_dbl;
         B_pp[0] <= B_dbl;
  for (pp_idx = 0; pp_idx < 3; pp_idx = pp_idx + 1) begin
           dSL_pp[pp_idx+1] <=      dSL_pp[pp_idx];
             S_pp[pp_idx+1] <=        S_pp[pp_idx];
      R_sl_pre_pp[pp_idx+1] <= R_sl_pre_pp[pp_idx];
      G_sl_pre_pp[pp_idx+1] <= G_sl_pre_pp[pp_idx];
      B_sl_pre_pp[pp_idx+1] <= B_sl_pre_pp[pp_idx];
             R_pp[pp_idx+1] <=        R_pp[pp_idx];
             G_pp[pp_idx+1] <=        G_pp[pp_idx];
             B_pp[pp_idx+1] <=        B_pp[pp_idx];
  end
  dSL_pp[4] <= dSL_pp[3];
    S_pp[4] <=   S_pp[3];
    R_pp[4] <=   R_pp[3];
    G_pp[4] <=   G_pp[3];
    B_pp[4] <=   B_pp[3];
     S_o    <=   S_pp[4];

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

assign vdata_o = {S_o,R_o,G_o,B_o};

endmodule 