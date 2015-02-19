function [LSB,steps_x,steps_y,line_xy] = sl_addamask(NumberOfConverterBits,Vmin,Vmax,OutputDataType)
%SL_ADDAMASK mask callback for Simulink A/D and D/A converters
%

% $Revision: 1.1.4.1 $ $Date: 2003/07/29 05:06:22 $
% Copyright 1990-2003 The MathWorks, Inc.

blk = gcb;
numValues = pow2(NumberOfConverterBits);
LSB = (Vmax - Vmin)/numValues;

%--- Treat floating point outputs so that they truncate properly

if strcmp(OutputDataType, 'double') || strcmp(OutputDataType, 'single'),
 set_param([blk,'/truncate'], 'DataType', 'int32');
 set_param([blk,'/cast'], 'DataType', OutputDataType);
else
 set_param([blk,'/truncate'], 'DataType', OutputDataType);
 set_param([blk,'/cast'], 'DataType', OutputDataType);
end

%--- Mask drawing data

b = 0.10;
m = 0.20;
steps_x = b + m*[0,0.5,0.5,1.5,1.5,2.5,2.5,3.5,4];
steps_y = b + m*[0,0,1,1,2,2,3,3,3];
line_xy = b + m*[0,4];
