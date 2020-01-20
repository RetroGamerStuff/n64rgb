%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This file is part of the N64 RGB/YPbPr DAC project.
%
% Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmail.com>
%
% N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http:%www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bit_width = 7;
max_value = 2^7-1;
rgb_i = 0:max_value;

gamma = [0.75 0.8 0.85 0.9 0.95 1.05 1.1 1.15];   % gamma values
fileName = 'gamma_table'; % file-name for verilog file (/wo extension)
targetFolder = '../rtl/misc';   % target folder for rom file

rgb_o = round(((rgb_i.'*ones(1,length(gamma)))/max_value).^...
               (ones(max_value+1,1)*gamma)*max_value);

rgb_o = reshape(rgb_o,1,[]);

fid = fopen([fileName '.v'],'w');

fprintf(fid, ...
 ['//////////////////////////////////////////////////////////////////////////////////\n' ...
  '//\n' ...
  '// This file is part of the N64 RGB/YPbPr DAC project.\n' ...
  '//\n' ...
  '// Copyright (C) 2015-2020 by Peter Bartmann <borti4938@gmail.com>\n' ...
  '//\n' ...
  '// N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify\n' ...
  '// it under the terms of the GNU General Public License as published by\n' ...
  '// the Free Software Foundation, either version 3 of the License, or\n' ...
  '// any later version.\n' ...
  '//\n' ...
  '// This program is distributed in the hope that it will be useful,\n' ...
  '// but WITHOUT ANY WARRANTY; without even the implied warranty of\n' ...
  '// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n' ...
  '// GNU General Public License for more details.\n' ...
  '//\n' ...
  '// You should have received a copy of the GNU General Public License\n' ...
  '// along with this program.  If not, see <http://www.gnu.org/licenses/>.\n' ...
  '//\n' ...
  '//////////////////////////////////////////////////////////////////////////////////\n' ...
  '//\n' ...
  '// Company:  Circuit-Board.de\n' ...
  '// Engineer: borti4938\n' ...
  '//\n' ...
  '// Module Name:    ' fileName '\n' ...
  '// Project Name:   N64 Advanced RGB/YPbPr DAC Mod\n' ...
  '// Target Devices: Max10, Cyclone IV and Cyclone 10 LP devices\n' ...
  '// Tool versions:  Altera Quartus Prime\n' ...
  '// Description:    simple line-multiplying\n' ...
  '//\n' ...
  '// Features: ip independent implementation of gamma rom\n' ...
  '//\n' ...
  '// This file is auto generated by script/gamma2rom.m\n' ...
  '//\n' ...
  '//////////////////////////////////////////////////////////////////////////////////\n' ...
  '\n\n'  ...
  'module ' fileName '(\n' ...
  '  VCLK,\n' ...
  '  nRST,\n' ...
  '  gamma_val,\n' ...
  '  vdata_in,\n' ...
  '  nbypass,\n' ...
  '  vdata_out\n' ...
  ');\n' ...
  '\n' ...
  '`include "vh/n64adv_vparams.vh"\n' ...
  '\n' ...
  'input                     VCLK;\n' ...
  'input [              2:0] gamma_val;\n' ...
  'input [color_width_i-1:0] vdata_in;\n' ...
  'input                     nbypass;\n' ...
  '\n' ...
  'output reg [color_width_i-1:0] vdata_out = {color_width_i{1''b0}};\n' ...
  '\n\n' ...
  'reg [color_width_i+2:0] addr_r = {(color_width_i+3){1''b0}};\n' ...
  'reg                  nbypass_r =  1''b0;\n' ...
  '\n' ...
  'always @(posedge VCLK or negedge nRST) begin\n' ...
  '  if (!nRST) begin\n' ...
  '    vdata_out <= {(color_width_i){1'b0}};\n' ...
  '       addr_r <= {(color_width_i+3){1'b0}};\n' ...
  '    nbypass_r <=  1'b0;\n' ...
  '  end else begin\n' ...
  '    addr_r <= {gamma_val,vdata_in};\n' ...
  '    nbypass_r <= nbypass;\n' ...
  '\n' ...
  '    case (addr_r)\n']);

for idx = 1:length(rgb_o)
  fprintf(fid,'      %04d: vdata_out <= %03d;\n', idx-1, rgb_o(idx));
end

fprintf(fid, ...
 ['    endcase\n' ...
  '\n' ...
  '    if (!nbypass_r)\n' ...
  '      vdata_out <= addr_r[color_width_i-1:0];\n' ...
  '  end\n' ...
  '\n' ...
  'endmodule\n']);

fclose(fid);
movefile([fileName '.v'],[targetFolder fileName '.v']); 