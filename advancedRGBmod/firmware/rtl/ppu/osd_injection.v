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
// Module Name:    osd_injection
// Project Name:   N64 Advanced HDMI Mod
// Target Devices: universial (PLL and 27MHz clock required)
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


module osd_injection (
  OSDCLK,
  OSD_VSync,
  OSDWrVector,
  OSDInfo,

  VCLK,
  nVRST,

  vdata_valid_i,
  vdata_i,
  vdata_valid_o,
  vdata_o
);


`include "vh/n64adv_cparams.vh"
`include "vh/n64adv_vparams.vh"


input OSDCLK;
output reg OSD_VSync;
input [24:0] OSDWrVector;
input [ 1:0] OSDInfo;

input VCLK;
input nVRST;

input vdata_valid_i;
input [`VDATA_I_FU_SLICE] vdata_i;
output reg vdata_valid_o = 1'b0;
output reg [`VDATA_I_FU_SLICE] vdata_o = {vdata_width_i{1'b0}};


// Display OSD Menu
// ================

// concept:
// - OSD is virtual screen of size 12x48 chars; each char stored in 2bit color + 8bit ASCCI-code.
//   (for simplicity atm. RAM has 48x16 words)
// - content is mapped into memory and written by NIOSII processor
// - Font is looked up in an extra ROM


// mist stuff (incl. unpacking inputs)
integer int_idx;
localparam vcnt_width = $clog2(`TOTAL_LINES_PAL_LX1);
localparam hcnt_width  = $clog2(`PIXEL_PER_LINE_MAX);

localparam [1023:0] n64adv_logo = 1024'h1FF7FCFF9C1B033FE7FD8180300FF3833FF7FEFFDE1B037FEFFD8180301FFBC330300600DF1B03606C0D8180FFD81BE33037FE00DB9BFF606C0DFF80FFDFFB733037FE00D9DBFF606C0DFF8030CFFB3B30300600D8FB03606C0D818030C01B1F3FF7FEFFD87BFF606FFDFF8030CFFB0F1FF7FCFF9839FE6067FCFF0030CFF307;
localparam logo_vcnt_width = 4;
localparam logo_hcnt_width = 8;

localparam osd_letter_vcnt_width = $clog2(`MAX_TEXT_ROWS);
localparam font_vcnt_width = $clog2(`OSD_FONT_HEIGHT);
localparam txt_vcnt_width = osd_letter_vcnt_width + font_vcnt_width;

localparam osd_letter_hcnt_width = $clog2(`MAX_CHARS_PER_ROW);
localparam font_hcnt_width = $clog2(`OSD_FONT_WIDTH);
localparam txt_hcnt_width = osd_letter_hcnt_width + font_hcnt_width;

localparam bg_color_sel_width = 2;
localparam bg_color_width = 9; // three bits per channel (do not change this)
localparam [bg_color_width-1:0] window_bg_color_default = `OSD_WINDOW_BGCOLOR_DARKBLUE;

localparam font_color_sel_width = 4;
localparam [`VDATA_I_CO_SLICE] font_color_default = `OSD_TXT_COLOR_WHITE;

localparam color_mem_width = bg_color_sel_width + font_color_sel_width;
localparam txt_mem_width = 7;


wire [                      1:0] vd_wrctrl       = OSDWrVector[24:23];
wire [osd_letter_hcnt_width-1:0] vd_wrpage       = OSDWrVector[22:17];
wire [osd_letter_vcnt_width-1:0] vd_wraddr       = OSDWrVector[16:13];
wire [      color_mem_width-1:0] vd_color_wrdata = OSDWrVector[12: 7];
wire [        txt_mem_width-1:0] vd_txt_wrdata   = OSDWrVector[ 6: 0];

wire show_osd_logo = OSDInfo[1];
wire show_osd      = OSDInfo[0];

wire nHSYNC_cur = vdata_i[3*color_width_i+1];
wire nVSYNC_cur = vdata_i[3*color_width_i+3];


// wires
wire negedge_nVSYNC, negedge_nHSYNC;

wire [osd_letter_hcnt_width-1:0] txt_xrdaddr;
wire [osd_letter_vcnt_width-1:0] txt_yrdaddr;
wire [bg_color_sel_width-1:0] bg_color_sel_tmp;
wire [font_color_sel_width-1:0] font_color_sel_tmp;
wire [txt_mem_width-1:0] font_char_select;

wire [`OSD_FONT_WIDTH:0] font_lineword_tmp;

wire [bg_color_width-1:0] window_bg_color_tmp;
wire [`VDATA_I_CO_SLICE] txt_color_tmp; 


// regs
reg nHSYNC_pre = 1'b0;
reg nVSYNC_pre = 1'b0;

reg [vcnt_width-1:0] vcnt = {vcnt_width{1'b0}};
reg [hcnt_width-1:0] hcnt = {hcnt_width{1'b0}};

reg [logo_vcnt_width-1:0] logo_vcnt = {logo_vcnt_width{1'b0}};
reg [logo_hcnt_width-1:0] logo_hcnt = {logo_hcnt_width{1'b0}};

reg [txt_vcnt_width-1:0] txt_vcnt = {txt_vcnt_width{1'b0}}; // MSB indexing actual letter (vertical count)
                                                            // LSB indexing actual (vertical) position in letter
reg [txt_hcnt_width-1:0] txt_hcnt; // MSB indexing actual letter (horizontal count)
                                   // LSB indexing actual (horizontal) position in letter
initial begin
  txt_hcnt[txt_hcnt_width-1:font_hcnt_width] = {osd_letter_hcnt_width{1'b1}};
  txt_hcnt[font_hcnt_width-1:0] = `OSD_FONT_WIDTH;
end

reg [7:0]   vdata_valid_L = 8'h0;
reg [`VDATA_I_FU_SLICE] vdata_L [0:7] /* synthesis ramstyle = "logic" */;
initial 
  for (int_idx = 0; int_idx < 8; int_idx = int_idx+1)
    vdata_L[int_idx] = {vdata_width_i{1'b0}};

reg [7:1] draw_osd_window = 7'h0; // draw window
reg [7:1]       draw_logo = 7'h0; // show logo
reg [7:1]     act_logo_px = 7'h0; // indicates an active pixel in logo
reg [7:1]        en_txtrd = 7'h0; // introduce six delay taps
reg [7:2]       en_fontrd = 6'h0; // read font


reg [bg_color_sel_width-1:0] bg_color_sel = {bg_color_sel_width{1'b0}};
reg [font_color_sel_width-1:0] font_color_sel = {font_color_sel_width{1'b0}};

reg [font_vcnt_width+1:0] font_pixel_select_4x = {font_vcnt_width+2{1'b0}};
reg [`OSD_FONT_WIDTH:0] font_lineword = {(`OSD_FONT_WIDTH+1){1'b0}};
reg act_char_px = 1'b0;

reg [bg_color_width-1:0] window_bg_color = window_bg_color_default;
reg [`VDATA_I_CO_SLICE] txt_color = font_color_default;


// start of rtl
assign negedge_nHSYNC =  nHSYNC_pre & !nHSYNC_cur;
assign negedge_nVSYNC =  nVSYNC_pre & !nVSYNC_cur;


always @(posedge VCLK or negedge nVRST)
  if (!nVRST) begin
    OSD_VSync <= 1'b0;
    
    nHSYNC_pre <= 1'b0;
    nVSYNC_pre <= 1'b0;
    
    vcnt <= {vcnt_width{1'b0}};
    hcnt <= {hcnt_width{1'b0}};

    logo_vcnt <= {logo_vcnt_width{1'b0}};
    logo_hcnt <= {logo_hcnt_width{1'b0}};
    
    txt_vcnt  <= {txt_vcnt_width{1'b0}};
    txt_hcnt[txt_hcnt_width-1:font_hcnt_width] <= {osd_letter_hcnt_width{1'b1}};
    txt_hcnt[font_hcnt_width-1:0] <= `OSD_FONT_WIDTH;

    vdata_valid_L   <= 8'h0;
    for (int_idx = 0; int_idx < 8; int_idx = int_idx+1)
      vdata_L[int_idx] <= {vdata_width_i{1'b0}};
    
    draw_osd_window <= 7'h0;
    draw_logo       <= 7'h0;
    act_logo_px     <= 7'h0;
    en_txtrd        <= 7'h0;
    en_fontrd       <= 6'h0;
  end else begin
    if (vdata_valid_i) begin
      if (negedge_nHSYNC) begin
//        vcnt <= ~&vcnt ? vcnt + 1'b1 : vcnt;  // saturate if needed
        vcnt <= vcnt + 1'b1;
        hcnt <= {hcnt_width{1'b0}};

        if (vcnt < `OSD_LOGO_VSTART | vcnt >= `OSD_LOGO_VSTOP)
          logo_vcnt <= {logo_vcnt_width{1'b0}};
//        else if (~&logo_vcnt)
//          logo_vcnt <= logo_vcnt + 1'b1;
        else
          logo_vcnt <= logo_vcnt + 1'b1;


        if (hcnt < `OSD_LOGO_HSTART | hcnt >= `OSD_LOGO_HSTOP)
          logo_hcnt <= {logo_hcnt_width{1'b0}};
        else
          logo_hcnt <= logo_hcnt + 1'b1;


        if (vcnt < `OSD_TXT_VSTART | vcnt >= `OSD_TXT_VSTOP) begin
          txt_vcnt <= {txt_vcnt_width{1'b0}};
        end else begin
          if (txt_vcnt[font_vcnt_width-1:0] < `OSD_FONT_HEIGHT) begin
            txt_vcnt <= txt_vcnt + 1'b1;
          end else begin
            txt_vcnt[txt_vcnt_width-1:font_vcnt_width] <= txt_vcnt[txt_vcnt_width-1:font_vcnt_width] + 1'b1;
            txt_vcnt[font_vcnt_width-1:0] <= 4'h0;
          end
        end
      end else begin
//        hcnt <= ~&hcnt ? hcnt + 1'b1 : hcnt;
        hcnt <= hcnt + 1'b1;
        
        if (draw_logo[4]) // vdata_valid_i is high every fourth clock cycle, so take draw_logo[4] (and not draw_logo[1])
//          if (~&logo_hcnt)
            logo_hcnt <= logo_hcnt + 1'b1;
        else
          logo_hcnt <= {logo_hcnt_width{1'b0}};
      end
      if (negedge_nVSYNC)
        vcnt <= {vcnt_width{1'b0}};

      nHSYNC_pre <= nHSYNC_cur;
      nVSYNC_pre <= nVSYNC_cur;
    end
    
    if (vdata_valid_L[1]) begin
      if (en_txtrd[1]) begin
        if (txt_hcnt[font_hcnt_width-1:0] < `OSD_FONT_WIDTH) begin
          txt_hcnt <= txt_hcnt + 1'b1;
        end else begin
          txt_hcnt[txt_hcnt_width-1:font_hcnt_width] <= txt_hcnt[txt_hcnt_width-1:font_hcnt_width] + 1'b1;
          txt_hcnt[font_hcnt_width-1:0] <= {font_hcnt_width{1'b0}};
        end
      end else begin
        txt_hcnt[txt_hcnt_width-1:font_hcnt_width] <= {osd_letter_hcnt_width{1'b1}};
        txt_hcnt[font_hcnt_width-1:0] <= `OSD_FONT_WIDTH;
      end
    end

    vdata_valid_L[7:1] <= vdata_valid_L[6:0];
    vdata_valid_L[0] <= vdata_valid_i;
    for (int_idx = 1; int_idx < 8; int_idx = int_idx+1)
      vdata_L[int_idx] <= vdata_L[int_idx-1];
    vdata_L[0] <= vdata_i;

    OSD_VSync <= (vcnt >= `OSD_WINDOW_VSTART) && (vcnt < `OSD_WINDOW_VSTOP);
    
    draw_osd_window[7:2] <= draw_osd_window[6:1];
    draw_osd_window[1] <= (vcnt >= `OSD_WINDOW_VSTART) && (vcnt < `OSD_WINDOW_VSTOP) &&
                          (hcnt >= `OSD_WINDOW_HSTART) && (hcnt < `OSD_WINDOW_HSTOP);
                          
    draw_logo[7:2] <= draw_logo[6:1];
    draw_logo[1] <= show_osd_logo &&
                    (vcnt >= `OSD_LOGO_VSTART) && (vcnt < `OSD_LOGO_VSTOP) &&
                    (hcnt >= `OSD_LOGO_HSTART) && (hcnt < `OSD_LOGO_HSTOP);

    act_logo_px[7:2] <= act_logo_px[6:1];
    act_logo_px[  1] <= n64adv_logo[{logo_vcnt[logo_vcnt_width-1:1],logo_hcnt[logo_hcnt_width-1:1]}];

    en_txtrd[7:2] <= en_txtrd[6:1];
    en_txtrd[1]  <= (vcnt >= `OSD_TXT_VSTART) && (vcnt < `OSD_TXT_VSTOP) &&
                    (hcnt >= `OSD_TXT_HSTART) && (hcnt < `OSD_TXT_HSTOP);
                    
    en_fontrd[7:3] <= en_fontrd[6:2];
    en_fontrd[2] <= en_txtrd[1] && (txt_hcnt[font_hcnt_width-1:0] == `OSD_FONT_WIDTH) && vdata_valid_L[1];
  end



assign txt_xrdaddr = txt_hcnt[txt_hcnt_width-1:font_hcnt_width];
assign txt_yrdaddr = txt_vcnt[txt_vcnt_width-1:font_vcnt_width];


ram2port #(
  .num_of_pages(`MAX_CHARS_PER_ROW+1),
  .pagesize(`MAX_TEXT_ROWS+1),
  .data_width(color_mem_width)
)vd_color_u(
  .wrCLK(OSDCLK),
  .wren(vd_wrctrl[1]),
  .wrpage(vd_wrpage),
  .wraddr(vd_wraddr),
  .wrdata(vd_color_wrdata),
  .rdCLK(VCLK),
  .rden(en_fontrd[2]),
  .rdpage(txt_xrdaddr),
  .rdaddr(txt_yrdaddr),
  .rddata({bg_color_sel_tmp,font_color_sel_tmp})
);

ram2port #(
  .num_of_pages(`MAX_CHARS_PER_ROW+1),
  .pagesize(`MAX_TEXT_ROWS+1),
  .data_width(txt_mem_width)
)
vd_text_u(
  .wrCLK(OSDCLK),
  .wren(vd_wrctrl[0]),
  .wrpage(vd_wrpage),
  .wraddr(vd_wraddr),
  .wrdata(vd_txt_wrdata),
  .rdCLK(VCLK),
  .rden(en_fontrd[2]),
  .rdpage(txt_xrdaddr),
  .rdaddr(txt_yrdaddr),
  .rddata(font_char_select)
);

always @(posedge VCLK or negedge nVRST) // delay font selection according to memory delay of chars and color
                                        // use the fact that pixel stays constant for´four clock cycles
  if (!nVRST) begin
    bg_color_sel <= {bg_color_sel_width{1'b0}};
    font_color_sel <= {font_color_sel_width{1'b0}};
  end else if (en_fontrd[4]) begin
    bg_color_sel   <= bg_color_sel_tmp;
    font_color_sel <= font_color_sel_tmp;
  end


font_rom font_rom_u(
  .CLK(VCLK),
  .nRST(nVRST),
  .char_addr(font_char_select),
  .char_line(txt_vcnt[font_vcnt_width-1:0]),
  .rden(en_fontrd[4]),
  .rddata(font_lineword_tmp)
);


assign window_bg_color_tmp = (bg_color_sel == `OSD_BACKGROUND_WHITE) ? `OSD_WINDOW_BGCOLOR_WHITE   :
                             (bg_color_sel == `OSD_BACKGROUND_GREY)  ? `OSD_WINDOW_BGCOLOR_GREY    :
                             (bg_color_sel == `OSD_BACKGROUND_BLACK) ? `OSD_WINDOW_BGCOLOR_BLACK   :
                                                                       `OSD_WINDOW_BGCOLOR_DARKBLUE;
assign txt_color_tmp = (font_color_sel == `FONTCOLOR_BLACK)       ? `OSD_TXT_COLOR_BLACK       :
                       (font_color_sel == `FONTCOLOR_GREY)        ? `OSD_TXT_COLOR_GREY        :
                       (font_color_sel == `FONTCOLOR_LIGHTGREY)   ? `OSD_TXT_COLOR_LIGHTGREY   :
                       (font_color_sel == `FONTCOLOR_WHITE)       ? `OSD_TXT_COLOR_WHITE       :
                       (font_color_sel == `FONTCOLOR_RED)         ? `OSD_TXT_COLOR_RED         :
                       (font_color_sel == `FONTCOLOR_GREEN)       ? `OSD_TXT_COLOR_GREEN       :
                       (font_color_sel == `FONTCOLOR_BLUE)        ? `OSD_TXT_COLOR_BLUE        :
                       (font_color_sel == `FONTCOLOR_YELLOW)      ? `OSD_TXT_COLOR_YELLOW      :
                       (font_color_sel == `FONTCOLOR_CYAN)        ? `OSD_TXT_COLOR_CYAN        :
                       (font_color_sel == `FONTCOLOR_MAGENTA)     ? `OSD_TXT_COLOR_MAGENTA     :
                       (font_color_sel == `FONTCOLOR_DARKORANGE)  ? `OSD_TXT_COLOR_DARKORANGE  :
                       (font_color_sel == `FONTCOLOR_TOMATO)      ? `OSD_TXT_COLOR_TOMATO      :
                       (font_color_sel == `FONTCOLOR_DARKMAGENTA) ? `OSD_TXT_COLOR_DARKMAGENTA :
                       (font_color_sel == `FONTCOLOR_NAVAJOWHITE) ? `OSD_TXT_COLOR_NAVAJOWHITE :
                       (font_color_sel == `FONTCOLOR_DARKGOLD)    ? `OSD_TXT_COLOR_DARKGOLD    :
                                                                    font_color_default         ;

always @(posedge VCLK or negedge nVRST)
  if (!nVRST) begin
    font_pixel_select_4x <= {font_vcnt_width+2{1'b0}};
    font_lineword <= {(`OSD_FONT_WIDTH+1){1'b0}};
    act_char_px <= 1'b0;
    window_bg_color <= window_bg_color_default;
    txt_color = font_color_default;
  end else begin
    if (|font_pixel_select_4x)
      font_pixel_select_4x <= font_pixel_select_4x + 1'b1;
    if (font_pixel_select_4x[font_vcnt_width+1:2] == `OSD_FONT_WIDTH && font_pixel_select_4x[1:0] == 2'b11)
      font_pixel_select_4x <= {font_vcnt_width+2{1'b0}};
    
    if (en_fontrd[6]) begin
      font_pixel_select_4x <= {{(font_vcnt_width+1){1'b0}},1'b1};
      font_lineword <= font_lineword_tmp;
      act_char_px <= (font_color_sel == `FONTCOLOR_NON) ? 1'b0 : font_lineword_tmp[0];
    end else if (en_txtrd[6]) begin
      if (vdata_valid_L[6])
        act_char_px <= (font_color_sel == `FONTCOLOR_NON) ? 1'b0 : font_lineword[font_pixel_select_4x[font_vcnt_width+1:2]];
      window_bg_color <= !draw_logo[6] ? window_bg_color_tmp : window_bg_color_default;
      txt_color <= txt_color_tmp;
    end
  end

always @(posedge VCLK or negedge nVRST)
  if (!nVRST) begin
    vdata_valid_o <= 1'b0;
    vdata_o <= {vdata_width_i{1'b0}};
  end else begin
    // pass through vdata valid and sync (don't care about modification delay (wich is simply a shift to the right for the menu)
    vdata_valid_o <= vdata_valid_L[7];
    vdata_o[`VDATA_I_SY_SLICE] <= vdata_L[7][`VDATA_I_SY_SLICE];

    // draw menu window if needed
    if (show_osd & draw_osd_window[7]) begin
      if (draw_logo[7] & act_logo_px[7])
        vdata_o[`VDATA_I_CO_SLICE] <= `OSD_LOGO_COLOR;
      else if (&{en_txtrd[7],!draw_logo[7],act_char_px})
          vdata_o[`VDATA_I_CO_SLICE] <= txt_color;
      else begin
      // modify red
        vdata_o[3*color_width_i-1:3*color_width_i-3] <= window_bg_color[bg_color_width-1:bg_color_width-3];
        vdata_o[3*color_width_i-4:2*color_width_i  ] <= vdata_L[7][3*color_width_i-1:2*color_width_i+3];
      // modify green
        vdata_o[2*color_width_i-1:2*color_width_i-3] <= window_bg_color[bg_color_width-4:bg_color_width-6];
        vdata_o[2*color_width_i-4:  color_width_i  ] <= vdata_L[7][2*color_width_i-1:color_width_i+3];
      // modify blue
        vdata_o[color_width_i-1:color_width_i-3] <= window_bg_color[bg_color_width-7:bg_color_width-9];
        vdata_o[color_width_i-4:              0] <= vdata_L[7][color_width_i-1:3];
      end
    end else begin
      vdata_o[`VDATA_I_CO_SLICE] <= vdata_L[7][`VDATA_I_CO_SLICE];
    end
  end

endmodule
