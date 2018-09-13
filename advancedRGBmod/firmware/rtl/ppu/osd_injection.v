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
// Module Name:    osd_injection
// Project Name:   N64 Advanced HDMI Mod
// Target Devices: universial (PLL and 27MHz clock required)
// Tool versions:  Altera Quartus Prime
// Description:
//
//////////////////////////////////////////////////////////////////////////////////


module osd_injection (
  OSDCLK,
  OSDWrVector,
  OSDInfo,

  VCLK,
  nVDSYNC,
  nVRST,

  video_data_i,
  video_data_o
);


`include "vh/n64adv_cparams.vh"
`include "vh/n64adv_vparams.vh"


input OSDCLK;
input [24:0] OSDWrVector;
input [ 2:0] OSDInfo;

input VCLK;
input nVDSYNC;
input nVRST;

input      [`VDATA_I_FU_SLICE] video_data_i;
output reg [`VDATA_I_FU_SLICE] video_data_o = {vdata_width_i{1'b0}};


// start of rtl

wire [ 1:0] vd_wrctrl = OSDWrVector[24:23];
wire [ 9:0] vd_wraddr = OSDWrVector[22:13];
wire [12:0] vd_wrdata = OSDWrVector[12: 0];

wire show_osd_logo = OSDInfo[2];
wire show_osd      = OSDInfo[1];


wire nHSYNC_cur = video_data_i[3*color_width_i+1];
wire nVSYNC_cur = video_data_i[3*color_width_i+3];

wire negedge_nHSYNC =  nHSYNC_pre & !nHSYNC_cur;
wire negedge_nVSYNC =  nVSYNC_pre & !nVSYNC_cur;


// Display OSD Menu
// ================

// concept:
// - OSD is virtual screen of size 12x48 chars; each char stored in 2bit color + 8bit ASCCI-code.
//   (for simplicity atm. RAM has 48x16 words)
// - content is mapped into memory and written by NIOSII processor
// - Font is looked up in an extra ROM

reg nHSYNC_pre = 1'b0;
reg nVSYNC_pre = 1'b0;

reg [9:0] h_cnt = 10'h0;
reg [7:0] v_cnt =  8'h0;

reg [7:0] logo_h_cnt = 8'h0;
reg [3:0] logo_v_cnt = 4'h0;

reg [8:0] txt_h_cnt = {{6{1'b1}},`OSD_FONT_WIDTH};  // 2:0 - font width reservation (allows for max 8p wide font); used for pixel selection
                                                    // 8:3 - indexing the char in each row
reg [7:0] txt_v_cnt = 8'h0; // 3:0 - font hight reservation (allows for max 16p hight font); used for addr. font pixel row
                            // 7:4 - selects the row of chars

reg [5:0] draw_osd_window = 6'h0;   // font and char memory
reg [5:0]       draw_logo = 6'h0;   // show logo
reg [5:0]        en_txtrd = 6'h0;   // introduce five delay taps
reg [3:1]       en_fontrd = 3'b000; // read font

always @(posedge VCLK) begin
  if (!nVDSYNC) begin
    h_cnt <= ~&h_cnt ? h_cnt + 1'b1 : h_cnt;

    if (negedge_nHSYNC) begin
      h_cnt <= 10'h0;
      v_cnt <= ~&v_cnt ? v_cnt + 1'b1 : v_cnt;

      if (v_cnt <= `OSD_LOGO_V_START | v_cnt >= `OSD_LOGO_V_STOP)
        logo_v_cnt <= 3'h0;
      else if (~&logo_v_cnt)
        logo_v_cnt <= logo_v_cnt + 1'b1;

      logo_h_cnt <= 8'h0;

      if (v_cnt <= `OSD_TXT_V_START | v_cnt >= `OSD_TXT_V_STOP) begin
        txt_v_cnt <= 7'h0;
      end else if (~&txt_v_cnt[7:4]) begin
        if (txt_v_cnt[3:0] == `OSD_FONT_HEIGHT) begin
          txt_v_cnt[3:0] <= 4'h0;
          txt_v_cnt[7:4] <= txt_v_cnt[7:4] + 1'b1;
        end else
          txt_v_cnt <= txt_v_cnt + 1'b1;
      end
    end
    if (negedge_nVSYNC)
      v_cnt <= 8'h0;

    if (draw_logo[1]) begin
      if (~&logo_h_cnt)
        logo_h_cnt <= logo_h_cnt + 1'b1;
    end

    if (en_txtrd[0]) begin
      if (txt_h_cnt[2:0] == `OSD_FONT_WIDTH) begin
        txt_h_cnt[2:0] <= 3'h0;
        txt_h_cnt[8:3] <= txt_h_cnt[8:3] + 1'b1;
      end else
        txt_h_cnt <= txt_h_cnt + 1'b1;
    end else begin
      txt_h_cnt <= {{6{1'b1}},`OSD_FONT_WIDTH};
    end

    nHSYNC_pre <= nHSYNC_cur;
    nVSYNC_pre <= nVSYNC_cur;
  end

  draw_logo[5:1] <= draw_logo[4:0];
  draw_logo[  0] <= show_osd_logo &&
                    (h_cnt > `OSD_LOGO_H_START) && (~&logo_h_cnt) &&
                    (v_cnt > `OSD_LOGO_V_START) && (v_cnt < `OSD_LOGO_V_STOP);

  draw_osd_window[5:1] <= draw_osd_window[4:0];
  draw_osd_window[  0] <= (h_cnt > `OSD_WINDOW_H_START) && (h_cnt < `OSD_WINDOW_H_STOP) &&
                          (v_cnt > `OSD_WINDOW_V_START) && (v_cnt < `OSD_WINDOW_V_STOP);

  en_txtrd[5:1]  <= en_txtrd[4:0];
  en_txtrd[  0]  <= (h_cnt > `OSD_TXT_H_START) && (h_cnt < `OSD_TXT_H_STOP) &&
                   (v_cnt > `OSD_TXT_V_START) && (v_cnt < `OSD_TXT_V_STOP);
  en_fontrd[3:2] <= en_fontrd[2:1];
  en_fontrd[  1] <= en_txtrd[0] && (txt_h_cnt[2:0] == `OSD_FONT_WIDTH);

  if (!nVRST) begin
    h_cnt <= 10'h0;
    v_cnt <=  8'h0;

    logo_h_cnt <= 8'h0;
    logo_v_cnt <= 4'h0;
    txt_h_cnt  <= {{6{1'b1}},`OSD_FONT_WIDTH};
    txt_v_cnt  <= 8'h0;

    draw_logo       <= 6'h0;
    draw_osd_window <= 6'h0;
    en_txtrd        <= 6'h0;
    en_fontrd       <= 3'b000;
  end
end

localparam [1023:0] logo = 1024'h1FF7FCFF9C1B033FE7FD8180300FF3833FF7FEFFDE1B037FEFFD8180301FFBC330300600DF1B03606C0D8180FFD81BE33037FE00DB9BFF606C0DFF80FFDFFB733037FE00D9DBFF606C0DFF8030CFFB3B30300600D8FB03606C0D818030C01B1F3FF7FEFFD87BFF606FFDFF8030CFFB0F1FF7FCFF9839FE6067FCFF0030CFF307;

reg [4:0] act_logo_px = 5'b00000;

always @(posedge VCLK) begin
  act_logo_px[4:1] <= act_logo_px[3:0];
  act_logo_px[  0] <= logo[{logo_v_cnt[3:1],logo_h_cnt[7:1]}];

  if (!nVRST)
    act_logo_px <= 5'b00000;
end

wire [5:0] txt_xrdaddr = txt_h_cnt[8:3];  // allows for max 64 chars each row
wire [3:0] txt_yrdaddr = txt_v_cnt[7:4];  // allows for max 16 rows
                                          // (initialized memories allow for
                                          //  maximum sizes as 1M9K is used anyway)
wire [1:0] background_tmp;
wire [3:0] font_color_tmp;
wire [6:0] font_addr_lsb;

ram2port #(
  .num_of_pages(`MAX_CHARS_PER_ROW+1),
  .pagesize(`MAX_TEXT_ROWS+1),
  .data_width(7)
)
vd_text_u(
  .wrCLK(OSDCLK),
  .wren(vd_wrctrl[0]),
  .wrpage(vd_wraddr[9:4]),
  .wraddr(vd_wraddr[3:0]),
  .wrdata(vd_wrdata[6:0]),
  .rdCLK(VCLK),
  .rden(en_fontrd[1]),
  .rdpage(txt_xrdaddr),
  .rdaddr(txt_yrdaddr),
  .rddata(font_addr_lsb)
);

ram2port #(
  .num_of_pages(`MAX_CHARS_PER_ROW+1),
  .pagesize(`MAX_TEXT_ROWS+1),
  .data_width(6)
)vd_color_u(
  .wrCLK(OSDCLK),
  .wren(vd_wrctrl[1]),
  .wrpage(vd_wraddr[9:4]),
  .wraddr(vd_wraddr[3:0]),
  .wrdata(vd_wrdata[12:7]),
  .rdCLK(VCLK),
  .rden(en_fontrd[1]),
  .rdpage(txt_xrdaddr),
  .rdaddr(txt_yrdaddr),
  .rddata({background_tmp,font_color_tmp})
);


reg [3:0] background_color_del = 4'h0;
reg [7:0] font_addr_msb        = 8'h0;
reg [7:0] font_color_del       = 8'h0;

always @(posedge VCLK) begin  // delay font selection according to memory delay of chars and color
  background_color_del <= {background_color_del[1:0],background_tmp};
  font_addr_msb  <= {font_addr_msb [3:0],txt_v_cnt[3:0]};
  font_color_del <= {font_color_del[3:0],font_color_tmp};

  if (!nVRST) begin
    background_color_del <= 4'h0;
    font_addr_msb        <= 8'h0;
    font_color_del       <= 8'h0;
  end
end

wire [1:0] background_color = background_color_del[3:2];
wire [3:0] font_color       = font_color_del[7:4];
wire [7:0] font_word;

font_rom font_rom_u(
  .CLK(VCLK),
  .nRST(nVRST),
  .char_addr(font_addr_lsb),
  .char_line(font_addr_msb[7:4]),
  .rden(en_fontrd[3]),
  .rddata(font_word)
);

reg [11:0] font_pixel_select = 12'h0;

always @(posedge VCLK) begin
  font_pixel_select  <= {font_pixel_select [8:0],txt_h_cnt[2:0]};

  if (!nVRST)
    font_pixel_select  <= 12'h0;
end

wire act_char_px = (font_color == `FONTCOLOR_NON) ? 1'b0 : font_word[font_pixel_select[11:9]];

wire [8:0] window_bg_color = (background_color == `OSD_BACKGROUND_WHITE) ? `OSD_WINDOW_BGCOLOR_WHITE   :
                             (background_color == `OSD_BACKGROUND_GREY)  ? `OSD_WINDOW_BGCOLOR_GREY    :
                             (background_color == `OSD_BACKGROUND_BLACK) ? `OSD_WINDOW_BGCOLOR_BLACK   :
                                                                           `OSD_WINDOW_BGCOLOR_DARKBLUE;

wire [8:0] window_bg_color_default = `OSD_WINDOW_BGCOLOR_DARKBLUE;

wire [8:0] window_bg_color_cur = (en_txtrd[5] & !draw_logo[5]) ? window_bg_color : window_bg_color_default;

wire [`VDATA_I_CO_SLICE] txt_color = (font_color == `FONTCOLOR_WHITE)       ? `OSD_TXT_COLOR_WHITE       :
                                     (font_color == `FONTCOLOR_BLACK)       ? `OSD_TXT_COLOR_BLACK       :
                                     (font_color == `FONTCOLOR_GREY)        ? `OSD_TXT_COLOR_GREY        :
                                     (font_color == `FONTCOLOR_LIGHTGREY)   ? `OSD_TXT_COLOR_LIGHTGREY   :
                                     (font_color == `FONTCOLOR_WHITE)       ? `OSD_TXT_COLOR_WHITE       :
                                     (font_color == `FONTCOLOR_RED)         ? `OSD_TXT_COLOR_RED         :
                                     (font_color == `FONTCOLOR_GREEN)       ? `OSD_TXT_COLOR_GREEN       :
                                     (font_color == `FONTCOLOR_BLUE)        ? `OSD_TXT_COLOR_BLUE        :
                                     (font_color == `FONTCOLOR_YELLOW)      ? `OSD_TXT_COLOR_YELLOW      :
                                     (font_color == `FONTCOLOR_CYAN)        ? `OSD_TXT_COLOR_CYAN        :
                                     (font_color == `FONTCOLOR_MAGENTA)     ? `OSD_TXT_COLOR_MAGENTA     :
                                     (font_color == `FONTCOLOR_DARKORANGE)  ? `OSD_TXT_COLOR_DARKORANGE  :
                                     (font_color == `FONTCOLOR_TOMATO)      ? `OSD_TXT_COLOR_TOMATO      :
                                     (font_color == `FONTCOLOR_DARKMAGENTA) ? `OSD_TXT_COLOR_DARKMAGENTA :
                                     (font_color == `FONTCOLOR_NAVAJOWHITE) ? `OSD_TXT_COLOR_NAVAJOWHITE :
                                                                              `OSD_TXT_COLOR_DARKGOLD    ;

always @(posedge VCLK) begin
  // pass through sync
  video_data_o[`VDATA_I_SY_SLICE] <= video_data_i[`VDATA_I_SY_SLICE];

  // draw menu window if needed
  if (show_osd & draw_osd_window[5]) begin
    if (draw_logo[5] & act_logo_px[4])
      video_data_o[`VDATA_I_CO_SLICE] <= `OSD_LOGO_COLOR;
    else if (&{en_txtrd[5],!draw_logo[5],|font_color,act_char_px})
        video_data_o[`VDATA_I_CO_SLICE] <= txt_color;
    else begin
    // modify red
      video_data_o[3*color_width_i-1:3*color_width_i-3] <= window_bg_color_cur[8:6];
      video_data_o[3*color_width_i-4:2*color_width_i  ] <= video_data_i[3*color_width_i-1:2*color_width_i+3];
    // modify green
      video_data_o[2*color_width_i-1:2*color_width_i-3] <= window_bg_color_cur[5:3];
      video_data_o[2*color_width_i-4:  color_width_i  ] <= video_data_i[2*color_width_i-1:color_width_i+3];
    // modify blue
      video_data_o[color_width_i-1:color_width_i-3] <= window_bg_color_cur[2:0];
      video_data_o[color_width_i-4:              0] <= video_data_i[color_width_i-1:3];
    end
  end else begin
    video_data_o[`VDATA_I_CO_SLICE] <= video_data_i[`VDATA_I_CO_SLICE];
  end
  
  if (!nVRST)
    video_data_o <= {vdata_width_i{1'b0}};
end

endmodule
