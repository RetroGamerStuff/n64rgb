%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This file is part of the N64 RGB/YPbPr DAC project.
%
% Copyright (C) 2015-2021 by Peter Bartmann <borti4938@gmail.com>
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
% along with this program.  If not, see <http:/www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This simple script reverts the bit order of the raw binary file which is
% outputted by Quartus Prime Lite. The output is written to a new binary
% file. It can be used to flash the SPI (U5) before soldering it to the
% board. This may simplify production runs.
% 
% If you are working on your own DIY board, you can use this file to. The
% MiniPro TL866CS/A comes in my mind when thinking of cheap programmer
% hardware for DIY. Unfortunately the MiniPro software does not support the
% IS25LP016D but similar replacements. E.g. you can load "A25L016 @SOP8" as
% target IC, load the *_spi and uncheck "Check ID" under options. Then you
% should be able to successfully flash the SPI for your project.
%

%% Cyclone 10LP

fileNameOpenC10 = 'n64adv_10cl010ye144.rbf';                % file-name of raw binary
targetFolderC10 = '../output_files/n64adv1/10cl010ye144/';  % target folder for binary file
fileNameWriteC10 = 'n64adv_10cl010ye144_spi.bin';           % file-name to write

fileID = fopen([targetFolderC10 fileNameOpenC10]);
data_C10 = uint8(fread(fileID));
fclose(fileID);

data_C10 = dec2bin(data_C10);
data_C10 = data_C10(:,8:-1:1);
data_C10 = uint8(bin2dec(data_C10));

fileID = fopen([targetFolderC10 fileNameWriteC10],'w');
fwrite(fileID,data_C10);
fclose(fileID);


%% Cyclone IV E

fileNameOpenCIV = 'n64adv_ep4ce10e22.rbf';                % file-name of raw binary
targetFolderCIV = '../output_files/n64adv1/ep4ce10e22/';  % target folder for binary file n64adv_ep4ce10e22
fileNameWriteCIV = 'n64adv_ep4ce10e22_spi.bin';           % file-name to write

fileID = fopen([targetFolderCIV fileNameOpenCIV]);
data_CIV = uint8(fread(fileID));
fclose(fileID);

data_CIV = dec2bin(data_CIV);
data_CIV = data_CIV(:,8:-1:1);
data_CIV = uint8(bin2dec(data_CIV));

fileID = fopen([targetFolderCIV fileNameWriteCIV],'w');
fwrite(fileID,data_CIV);
fclose(fileID);