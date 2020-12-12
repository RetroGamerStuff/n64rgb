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
// Module Name:    linemult
// Project Name:   N64 Advanced RGB/YPbPr DAC Mod
// Target Devices: Cyclone IV and Cyclone 10 LP devices
// Tool versions:  Altera Quartus Prime
// Description:    simple line-multiplying
//
//////////////////////////////////////////////////////////////////////////////////


module linemult(
  VCLK_i,
  nVRST_i,
  vdata_valid_i,
  vdata_i,

  vinfo_mult,

  VCLK_o,
  nVRST_o,
  vdata_valid_o,
  vdata_o
);

`include "vh/n64adv_vparams.vh"

input VCLK_i;
input nVRST_i;
input vdata_valid_i; // for new implementation
input  [`VDATA_I_FU_SLICE] vdata_i;

input [20:0] vinfo_mult; // [nLineMult (2bits),lx_ifix (1bit),SLhyb_str (5bits),SL_str (8bits),SL_method,SL_id,SL_en,palmode,n64_480i]

input  VCLK_o;
input  nVRST_o;
output reg vdata_valid_o = 1'b1; // for new implementation
output reg [`VDATA_O_FU_SLICE] vdata_o = {vdata_width_o{1'b0}};


// mist stuff
localparam hcnt_width  = $clog2(`PIXEL_PER_LINE_MAX_2x);
localparam pcnt_width = $clog2(`BUF_NUM_OF_PAGES);
localparam Y_width     = color_width_o+1;
localparam SLHyb_width = 8; // do not change this localparam!

integer int_idx, pp_idx;


// configuration
wire   nENABLE_linemult = ~^vinfo_mult[20:19];
wire [1:0] linemult_sel =   vinfo_mult[20:19]; // LineX3 not allowed in PAL mode (covered in upper module)
wire       bob_480i_fix =   vinfo_mult[18];
wire [4:0]  SLHyb_depth =   vinfo_mult[17:13];
wire [7:0]       SL_str =   vinfo_mult[12:5];
wire          SL_method =   vinfo_mult[ 4];
wire              SL_id =   vinfo_mult[ 3];
wire              SL_en =   vinfo_mult[ 2];
wire            palmode =   vinfo_mult[ 1];
wire           n64_480i =   vinfo_mult[ 0];

// wires
wire nVS_i, nHS_i;
wire negedge_nHSYNC, negedge_nVSYNC;

wire line_overflow, valid_line;

wire [pcnt_width-1:0] wrpage_o_resynced;
wire wrhcnt_b6_o_resynced, valid_line_r_o_resynced, start_rdproc_o_resynced;


wire [          10:0] vpos_o;
wire [hcnt_width-1:0] hpos_o;

wire [pcnt_width-1:0] rdpage_pp0, rdpage_pp1, rdpage_pp2, rdpage_pp3;
wire [color_width_i-1:0] R_buf, G_buf, B_buf;


wire [color_width_i-1:0] R_avg, G_avg, B_avg;


wire [Y_width+4:0] Y_ref_pre_full;
wire [Y_width+SLHyb_width-1:0] Y_ref_full;
wire [color_width_o+SLHyb_width-2:0] R_sl_full, G_sl_full, B_sl_full;


// regs
reg nVDSYNC_dbl = 1'b0;
reg nVS_i_buf = 1'b0;
reg nHS_i_buf = 1'b0;
reg FrameID = 1'b0;


reg [hcnt_width-1:0] hstart_i = `HSTART_NTSC_2x;
reg [hcnt_width-1:0] hstop_i  = `HSTOP_NTSC_2x;

reg                  wren   = 1'b0;
reg [pcnt_width-1:0] wrpage = {pcnt_width{1'b0}};
reg [hcnt_width-1:0] wrhcnt = {hcnt_width{1'b1}};
reg [hcnt_width-1:0] wraddr = {hcnt_width{1'b0}};

reg line_overflow_r = 1'b0;
reg valid_line_r    = 1'b0;

reg [hcnt_width-1:0] line_width[0:`BUF_NUM_OF_PAGES-1];
initial begin
  for (int_idx = 0; int_idx < `BUF_NUM_OF_PAGES; int_idx = int_idx+1)
    line_width[int_idx] = {hcnt_width{1'b0}};
end


reg [1:0] newFrame = 2'b0; // newFrame[1] used by reading process, newFrame[0] by writing process
reg start_rdproc = 1'b0;


reg [hcnt_width-1:0] hstart_o = `HSTART_NTSC_2x;
reg [hcnt_width-1:0] hstop_o  = `HSTOP_NTSC_2x;

reg [ 6:0] nHS_width = `HS_WIDTH_NTSC_LX2_2x;
reg [ 4:0] pic_shift = `H_SHIFT_NTSC_240P_LX2_2x;
reg [ 3:0] nVS_width = `VS_WIDTH_NTSC_LX2;
reg [10:0] nVS_delay = (`BUF_NUM_OF_PAGES>>1);

reg            [2:0] rden    = 3'b0;
reg            [1:0] rdrun   = 2'b00;
reg            [1:0] rdcnt   = 2'b0;
reg [pcnt_width-1:0] rdpage  = {pcnt_width{1'b0}};
reg [hcnt_width-1:0] rdhcnt  = {hcnt_width{1'b0}};
reg [          10:0] rdvcnt  = 11'd0;
reg [hcnt_width-1:0] rdaddr  = {hcnt_width{1'b0}};

reg start_rdproc_o_resynced_pre = 1'b0;

reg nHSYNC_mult = 1'b0;
reg nVSYNC_mult = 1'b0;
reg nCSYNC_mult = 1'b0;


reg                     drawSL [0:2];
reg               [3:0] S_mult [0:2];
reg [color_width_i-1:0] R_mult_pre = {color_width_i{1'b0}};
reg [color_width_i-1:0] G_mult_pre = {color_width_i{1'b0}};
reg [color_width_i-1:0] B_mult_pre = {color_width_i{1'b0}};
reg [color_width_i-1:0] R_sl_pre = {color_width_i{1'b0}};
reg [color_width_i-1:0] G_sl_pre = {color_width_i{1'b0}};
reg [color_width_i-1:0] B_sl_pre = {color_width_i{1'b0}};
reg [color_width_i-1:0] R_mult = {color_width_i{1'b0}};
reg [color_width_i-1:0] G_mult = {color_width_i{1'b0}};
reg [color_width_i-1:0] B_mult = {color_width_i{1'b0}};
reg [Y_width-1:0] Y_sl_pre = {Y_width{1'b0}};

initial begin
  for (int_idx = 0; int_idx < 3; int_idx = int_idx+1) begin
    drawSL[int_idx] = 1'b0;
    S_mult[int_idx] = 4'b0000;
  end
end


reg                     dSL_pp[0:4]                     /* synthesis ramstyle = "logic" */;
reg               [3:0] S_pp[0:4]                       /* synthesis ramstyle = "logic" */;
reg [color_width_i-1:0] R_sl_pre_pp[0:3],
                        G_sl_pre_pp[0:3],
                        B_sl_pre_pp[0:3]                /* synthesis ramstyle = "logic" */;
reg [color_width_i-1:0] R_pp[0:4], G_pp[0:4], B_pp[0:4] /* synthesis ramstyle = "logic" */;

initial begin
  for (int_idx = 0; int_idx < 5; int_idx = int_idx+1) begin
    dSL_pp[int_idx] <= 1'b0;
    S_pp[int_idx] <= 4'b0000;
    R_pp[int_idx] <= {color_width_i{1'b0}};
    G_pp[int_idx] <= {color_width_i{1'b0}};
    B_pp[int_idx] <= {color_width_i{1'b0}};
  end
  for (int_idx = 0; int_idx < 4; int_idx = int_idx+1) begin
    R_sl_pre_pp[int_idx] <= {color_width_i{1'b0}};
    G_sl_pre_pp[int_idx] <= {color_width_i{1'b0}};
    B_sl_pre_pp[int_idx] <= {color_width_i{1'b0}};
  end
end

reg [Y_width-1:0] Y_ref_pre = {Y_width{1'b0}};

reg [Y_width-1:0] Y_ref = {Y_width{1'b0}};

reg [SLHyb_width-1:0] SLHyb_rval = {SLHyb_width{1'b0}};
reg [SLHyb_width-1:0] SLHyb_str = {SLHyb_width{1'b0}};

reg [color_width_o-1:0] R_sl, G_sl, B_sl;
initial begin
  R_sl <= {color_width_o{1'b0}};
  G_sl <= {color_width_o{1'b0}};
  B_sl <= {color_width_o{1'b0}};
end



// start of rtl
assign nVS_i = vdata_i[3*color_width_i+3];
assign nHS_i = vdata_i[3*color_width_i+1];
assign negedge_nHSYNC =  nHS_i_buf & !nHS_i;
assign negedge_nVSYNC =  nVS_i_buf & !nVS_i;


always @(posedge VCLK_i or negedge nVRST_i)
  if (!nVRST_i) begin
    nVDSYNC_dbl <= 1'b0;
    nHS_i_buf   <= 1'b0;
    nVS_i_buf   <= 1'b0;
    FrameID     <= 1'b0;
  end else begin
    if (!nVDSYNC_dbl) begin
      nHS_i_buf <= nHS_i;
      nVS_i_buf <= nVS_i;

      if (negedge_nVSYNC) begin
        FrameID <= negedge_nHSYNC; // negedge at nHSYNC, too -> odd frame
      end
    end

    nVDSYNC_dbl <= ~nVDSYNC_dbl;
  end



assign line_overflow = wrhcnt == `PIXEL_PER_LINE_MAX_2x;  // close approach for NTSC and PAL (equals 1600)
assign valid_line    = wrhcnt > hstop_i & !line_overflow; // for evaluation

always @(posedge VCLK_i or negedge nVRST_i)
  if (!nVRST_i) begin
    wren   <= 1'b0;
    wrpage <= {pcnt_width{1'b0}};
    wrhcnt <= {hcnt_width{1'b1}};
    wraddr <= {hcnt_width{1'b0}};

    line_overflow_r <= 1'b0;
    valid_line_r    <= 1'b0;

     newFrame[0] <= 1'b0;
    start_rdproc <= 1'b0;
  end else if (!nVDSYNC_dbl) begin
    if (negedge_nVSYNC) begin
      // trigger new frame
      newFrame[0] <= ~newFrame[1];

      // set new info
      if (palmode) begin
        hstart_i <= `HSTART_PAL_2x;
        hstop_i  <= `HSTOP_PAL_2x;
      end else begin
        hstart_i <= `HSTART_NTSC_2x;
        hstop_i  <= `HSTOP_NTSC_2x;
      end

      if (negedge_nHSYNC)
        start_rdproc <= ~start_rdproc;  // trigger read start
    end

    if (negedge_nHSYNC) begin // negedge nHSYNC -> reset wrhcnt and inc. wrpage
      line_width[wrpage] <= wrhcnt;
      wrhcnt <= {hcnt_width{1'b0}};
      if (wrpage == `BUF_NUM_OF_PAGES-1)
        wrpage <= {pcnt_width{1'b0}};
      else
        wrpage <= wrpage + 1'b1;
      if (!rdrun[0])
        wrpage <= {pcnt_width{1'b0}};
      
      valid_line_r <= valid_line;
    end else if (~line_overflow) begin
      wrhcnt <= wrhcnt + 1'b1;
    end

    line_overflow_r <= line_overflow;

    if (wrhcnt == hstart_i) begin
      wren   <= 1'b1;
      wraddr <= {hcnt_width{1'b0}};
    end else if (wrhcnt > hstart_i && wrhcnt < hstop_i) begin
      wraddr <= wraddr + 1'b1;
    end else begin
      wren   <= 1'b0;
      wraddr <= {hcnt_width{1'b0}};
    end
  end


register_sync #(
  .reg_width(pcnt_width+3),
  .reg_preset({{pcnt_width{1'b0}},3'b000})
) sync4tx_u(
  .clk(VCLK_o),
  .clk_en(1'b1),
  .nrst(nVRST_o),
  .reg_i({wrpage,wrhcnt[6],valid_line_r,start_rdproc}),
  .reg_o({wrpage_o_resynced,wrhcnt_b6_o_resynced,valid_line_r_o_resynced,start_rdproc_o_resynced})
);



assign vpos_o = rdvcnt - nVS_delay;
assign hpos_o = rdhcnt;

always @(posedge VCLK_o or negedge nVRST_o)
  if (!nVRST_o) begin
    rden    <= 3'b0;
    rdrun   <= 2'b00;
    rdcnt   <= 2'b0;
    rdpage  <= {pcnt_width{1'b0}};
    rdhcnt  <= {hcnt_width{1'b0}};
    rdvcnt  <= 11'd0;
    rdaddr  <= {hcnt_width{1'b0}};

    newFrame[1] <= 1'b0;
    hstart_o <= `HSTART_NTSC_2x;
    hstop_o  <= `HSTOP_NTSC_2x;
    nHS_width <= `HS_WIDTH_NTSC_LX2_2x;
    pic_shift <= `H_SHIFT_NTSC_240P_LX2_2x;
    nVS_width <= `VS_WIDTH_NTSC_LX2;
    nVS_delay <= (`BUF_NUM_OF_PAGES>>1);
    start_rdproc_o_resynced_pre <= 1'b0;

    nHSYNC_mult <= 1'b0;
    nVSYNC_mult <= 1'b0;
    nCSYNC_mult <= 1'b0;
  end else begin
    if (rdrun[1]) begin
      if (rdhcnt == line_width[rdpage_pp1]) begin
        rdhcnt   <= {hcnt_width{1'b0}};
        if (rdcnt == linemult_sel) begin
          rdcnt <= 2'b00;
          if (rdpage == `BUF_NUM_OF_PAGES - 1)
            rdpage = {pcnt_width{1'b0}};
          else
            rdpage <= rdpage + 1'b1;
        end else begin
          rdcnt <= rdcnt + 1'b1;
        end
        
        if (^newFrame) begin
          // set new info (delayed)
          if (palmode) begin
            hstart_o <= `HSTART_PAL_2x;
            hstop_o  <= `HSTOP_PAL_2x;
            nHS_width <= `HS_WIDTH_PAL_LX2_2x;
            pic_shift <= n64_480i ? `H_SHIFT_PAL_576I_LX2_2x : `H_SHIFT_PAL_288P_LX2_2x;
            nVS_width <= `VS_WIDTH_PAL_LX2;
          end else begin
            hstart_o <= `HSTART_NTSC_2x;
            hstop_o  <= `HSTOP_NTSC_2x;
            if (linemult_sel[1]) begin
              nHS_width <= `HS_WIDTH_NTSC_LX3_2x;
              pic_shift <= `H_SHIFT_NTSC_240P_LX3_2x;
              nVS_width <= `VS_WIDTH_NTSC_LX3;
            end else begin
              nHS_width <= `HS_WIDTH_NTSC_LX2_2x;
              pic_shift <= n64_480i ? `H_SHIFT_NTSC_480I_LX2_2x : `H_SHIFT_NTSC_240P_LX2_2x;
              nVS_width <= `VS_WIDTH_NTSC_LX2;
            end
            rdpage   <= wrpage_o_resynced - 2'b10;
          end
          nVS_delay <= linemult_sel[1] ? 0 : (`BUF_NUM_OF_PAGES>>1);

          newFrame[1] <= newFrame[0];
          rdvcnt <= 11'd0;
        end else begin
          rdvcnt <= rdvcnt + 1'b1;
        end
      end else begin
        rdhcnt <= rdhcnt + 1'b1;
      end

      if (hpos_o == (hstart_o - pic_shift- 4)) begin // consider two cycle delay for reading BRAM and two cycle by registering afterwards
        rden[0] <= 1'b1;
        rdaddr  <= {hcnt_width{1'b0}};
      end else if (rden[0] && hpos_o < (hstop_o - pic_shift - 3)) begin // go one beyond taking care of advanced scanlines
        rdaddr <= rdaddr + 1'b1;
      end else begin
        rden[0] <= 1'b0;
        rdaddr  <= {hcnt_width{1'b0}};
      end
    end else if (rdrun[0] && wrhcnt_b6_o_resynced) begin
      rdrun[1] <= 1'b1;
      rdcnt    <= 2'b00;
      rdpage   <= wrpage_o_resynced - 2'b10;
      rdhcnt   <= {hcnt_width{1'b0}};
      rdvcnt   <= (`BUF_NUM_OF_PAGES>>1);
    end else if (start_rdproc_o_resynced ^ start_rdproc_o_resynced_pre) begin
      rdrun[0] <= 1'b1; // move to steady state
    end

    rden[2:1] <= rden[1:0];
    start_rdproc_o_resynced_pre <= start_rdproc_o_resynced;

    if (!valid_line_r_o_resynced || nENABLE_linemult) begin // additional reset conditions
      rden  <= 3'b0;
      rdrun <= 2'b0;
    end

    nHSYNC_mult <= hpos_o >= nHS_width;
    nVSYNC_mult <= vpos_o >= nVS_width;
    nCSYNC_mult <= vpos_o >= nVS_width ? hpos_o >= nHS_width : hpos_o > (line_width[rdpage_pp1] - nHS_width);
  end


assign rdpage_pp0 = (FrameID | !n64_480i | !bob_480i_fix) ? rdpage : rdpage - !rdcnt[0];
assign rdpage_pp1 = rdpage_pp0 >= `BUF_NUM_OF_PAGES ? `BUF_NUM_OF_PAGES-1 : rdpage_pp0;
assign rdpage_pp2 = (!SL_method | n64_480i) ? rdpage_pp1 :  // do not allow advanced scanlines in 480i linex2 mode
                                 !rdaddr[0] ? rdpage_pp1 : !SL_id ? rdpage_pp1 - 1'b1 : rdpage_pp1 + 1'b1;
assign rdpage_pp3 = !SL_id ? (rdpage_pp2 >= `BUF_NUM_OF_PAGES ? `BUF_NUM_OF_PAGES-1 : rdpage_pp2) :
                             (rdpage_pp2 == `BUF_NUM_OF_PAGES ?  {pcnt_width{1'b0}} : rdpage_pp2);

ram2port #(
  .num_of_pages(`BUF_NUM_OF_PAGES),
  .pagesize(`BUF_DEPTH_PER_PAGE),
  .data_width(3*color_width_i)
) videobuffer_u(
  .wrCLK(VCLK_i),
  .wren(&{wren,!line_overflow,nVDSYNC_dbl,!wraddr[0]}),
  .wrpage(wrpage),
  .wraddr(wraddr[hcnt_width-1:1]),
  .wrdata(vdata_i[`VDATA_I_CO_SLICE]),
  .rdCLK(VCLK_o),
  .rden(rden[0]), // leave reading open even if rdaddr[0] == 1 for advanced scanline method
  .rdpage(rdpage_pp3),
  .rdaddr(rdaddr[hcnt_width-1:1]),
  .rddata({R_buf,G_buf,B_buf})
);


assign R_avg = {1'b0,R_mult_pre[color_width_i-1:1]} + {1'b0,R_buf[color_width_i-1:1]} + (R_mult_pre[0] ^ R_buf[0]);
assign G_avg = {1'b0,G_mult_pre[color_width_i-1:1]} + {1'b0,G_buf[color_width_i-1:1]} + (G_mult_pre[0] ^ G_buf[0]);
assign B_avg = {1'b0,B_mult_pre[color_width_i-1:1]} + {1'b0,B_buf[color_width_i-1:1]} + (B_mult_pre[0] ^ B_buf[0]);

always @(posedge VCLK_o or negedge nVRST_o)
  if (!nVRST_o) begin
    for (int_idx = 0; int_idx < 3; int_idx = int_idx+1) begin
      drawSL[int_idx] <= 1'b0;
      S_mult[int_idx] <= 4'b0000;
    end
    R_mult_pre <= {color_width_i{1'b0}};
    G_mult_pre <= {color_width_i{1'b0}};
    B_mult_pre <= {color_width_i{1'b0}};
    R_sl_pre   <= {color_width_i{1'b0}};
    G_sl_pre   <= {color_width_i{1'b0}};
    B_sl_pre   <= {color_width_i{1'b0}};
    R_mult     <= {color_width_i{1'b0}};
    G_mult     <= {color_width_i{1'b0}};
    B_mult     <= {color_width_i{1'b0}};
    Y_sl_pre   <= {Y_width{1'b0}};
  end else begin
    S_mult[2] <=  S_mult[1];
    S_mult[1] <=  S_mult[0];
    S_mult[0] <= {nVSYNC_mult,1'b0, nHSYNC_mult,nCSYNC_mult};
    drawSL[2] <= drawSL[1];
    drawSL[1] <= drawSL[0];
    drawSL[0] <= SL_en && (linemult_sel == 2'b01 ? (rdcnt ^ (!SL_id)) :
                           SL_id ? (rdcnt == 2'b10) : (rdcnt == 2'b00));

    if (rden[2]) begin
      if (!rdaddr[0]) begin  // reading buffer has exactly two delay steps - so we can safetely use !rdaddr[0]
        R_mult_pre <= R_buf;
        G_mult_pre <= G_buf;
        B_mult_pre <= B_buf;
      end else begin
        R_sl_pre <= R_avg;
        G_sl_pre <= G_avg;
        B_sl_pre <= B_avg;
        Y_sl_pre <= {2'b00,R_avg} + {1'b0,G_avg,1'b0} + {2'b00,B_avg};
      end
    end else begin
      R_mult_pre <= {color_width_i{1'b0}};
      G_mult_pre <= {color_width_i{1'b0}};
      B_mult_pre <= {color_width_i{1'b0}};
      R_mult_pre <= {color_width_i{1'b0}};
      G_mult_pre <= {color_width_i{1'b0}};
      B_mult_pre <= {color_width_i{1'b0}};
      R_sl_pre   <= {color_width_i{1'b0}};
      G_sl_pre   <= {color_width_i{1'b0}};
      B_sl_pre   <= {color_width_i{1'b0}};
      Y_sl_pre   <= {      Y_width{1'b0}};
    end
    R_mult <= R_mult_pre;
    G_mult <= G_mult_pre;
    B_mult <= B_mult_pre;
  end


// post-processing (scanline generation)
assign Y_ref_pre_full = Y_sl_pre * (* multstyle = "dsp" *) SLHyb_depth;
assign Y_ref_full = Y_ref_pre * (* multstyle = "dsp" *) SL_str;
assign R_sl_full = R_sl_pre_pp[3] * (* multstyle = "dsp" *) SLHyb_str;
assign G_sl_full = G_sl_pre_pp[3] * (* multstyle = "dsp" *) SLHyb_str;
assign B_sl_full = B_sl_pre_pp[3] * (* multstyle = "dsp" *) SLHyb_str;

always @(posedge VCLK_o or negedge nVRST_o)
  if (!nVRST_o) begin
    for (int_idx = 0; int_idx < 5; int_idx = int_idx+1) begin
      dSL_pp[int_idx] <= 1'b0;
      S_pp[int_idx] <= 4'b0000;
      R_pp[int_idx] <= {color_width_i{1'b0}};
      G_pp[int_idx] <= {color_width_i{1'b0}};
      B_pp[int_idx] <= {color_width_i{1'b0}};
    end
    for (int_idx = 0; int_idx < 4; int_idx = int_idx+1) begin
      R_sl_pre_pp[int_idx] <= {color_width_i{1'b0}};
      G_sl_pre_pp[int_idx] <= {color_width_i{1'b0}};
      B_sl_pre_pp[int_idx] <= {color_width_i{1'b0}};
    end
    Y_ref_pre <= {Y_width{1'b0}};
    Y_ref <= {Y_width{1'b0}};

    SLHyb_rval <= {SLHyb_width{1'b0}};
    SLHyb_str <= {SLHyb_width{1'b0}};

    R_sl <= {color_width_o{1'b0}};
    G_sl <= {color_width_o{1'b0}};
    B_sl <= {color_width_o{1'b0}};

    vdata_o <= {vdata_width_o{1'b0}};
  end else begin
         dSL_pp[0] <= drawSL[2];
           S_pp[0] <= S_mult[2];
    R_sl_pre_pp[0] <= R_sl_pre;
    G_sl_pre_pp[0] <= G_sl_pre;
    B_sl_pre_pp[0] <= B_sl_pre;
           R_pp[0] <= R_mult;
           G_pp[0] <= G_mult;
           B_pp[0] <= B_mult;
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
        dSL_pp[4] <=     dSL_pp[3];
          S_pp[4] <=       S_pp[3];
          R_pp[4] <=       R_pp[3];
          G_pp[4] <=       G_pp[3];
          B_pp[4] <=       B_pp[3];


    // hybrid strength reference (2 pp stages)
    Y_ref_pre <= Y_ref_pre_full[Y_width+4:5];                   // stage [0]
    Y_ref     <= Y_ref_full[Y_width+SLHyb_width-1:SLHyb_width]; // stage [1]

    // adaptation of sl_str. (2 pp stages)
    SLHyb_rval <= {1'b0,SL_str} < Y_ref ? 8'h0 : SL_str - Y_ref[7:0]; // stage [2]
    SLHyb_str  <= 8'hff - SLHyb_rval;                                 // stage [3]
    
    // calculate SL (1 pp stage)
    R_sl <= R_sl_full[color_width_o+SLHyb_width-2:SLHyb_width-1]; // stage [4]
    G_sl <= G_sl_full[color_width_o+SLHyb_width-2:SLHyb_width-1]; // stage [4]
    B_sl <= B_sl_full[color_width_o+SLHyb_width-2:SLHyb_width-1]; // stage [4]

    // set output
    vdata_o[`VDATA_O_SY_SLICE] <= S_pp[4];
    if (dSL_pp[4])  // scanline
      vdata_o[`VDATA_O_CO_SLICE] <= {R_sl,G_sl,B_sl};
    else  // not a scanline
      vdata_o[`VDATA_O_CO_SLICE] <= {R_pp[4],R_pp[4][color_width_i-1],
                                     G_pp[4],G_pp[4][color_width_i-1],
                                     B_pp[4],B_pp[4][color_width_i-1]};

    // use standard input if no line-doubling
    if (nENABLE_linemult)
      vdata_o <= {vdata_i[`VDATA_I_SY_SLICE],
                  vdata_i[`VDATA_I_RE_SLICE],vdata_i[3*color_width_i-1],
                  vdata_i[`VDATA_I_GR_SLICE],vdata_i[2*color_width_i-1],
                  vdata_i[`VDATA_I_BL_SLICE],vdata_i[color_width_i-1]};
  end


endmodule 