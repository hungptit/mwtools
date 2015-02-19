function [test] = tvalidname(point)
% Test suite for geeting valid model name given a subsystem block name
%

% Copyright 2003-2008 The MathWorks, Inc.
%  $Revision: 1.1.4.3 $
%
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.
testharness;                     % Run the tests and report results
  
  
function [test,testparams]=setup(test,testparams)
%SETUP Create variables, figures, pcode, etc., for use in all testpoints.
% Store these in fields of the testparams structure.
% Even if you do not intend to use testparams, create the
% variable anyway to avoid bogus warnings in function calls.
  testparams = slrtw_remove_drool(testparams,'setup');
  testparams.wsVars_before = evalin('base', 'who;'); 
  
function test = cleanup(test,testparams)
%CLEANUP Close figure windows created by setup.
  testparams = slrtw_remove_drool(testparams,'cleanup');

%CLEANUP 


%------------------------------------------------------------------------------
function test = lvlTwo1(test, testparams)
% Test for valid name
newName = 'ssBlock';
expName = 'ssBlock';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo2(test, testparams)
% Test for block with space in the name

newName = ' ssBlock ';
expName = 'ssBlock';
test = test_name_l(test, [], newName, expName);

%endfunction


%------------------------------------------------------------------------------
function test = lvlTwo3(test, testparams)
% Test for block with invalid characters

newName = ' ------- ';
expName = 'modelref';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo4(test, testparams)
% Test for block with more that namelengthmax:63 chars

newName = 'a12345678901234567890123456789012345678901234567890123456789012345';
expName = 'a123456789012345678901234567';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo5(test, testparams)
% Test for block with more than MaxIdLength > 63

load_system('mvalidname');
set_param('mvalidname', 'MaxIdLength', '70');
newName = 'a12345678901234567890123456789012345678901234567890123456789012345678901234567890';
expName = 'a12345678901234567890123456789012345678901234567890123456789';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo6(test, testparams)
% Test for block with invalid char in middle

newName = 'my model name \ ';
expName = 'my_model_name';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo7(test, testparams)
% Test for block with double invalid char in middle

newName = 'my   model name';
expName = 'my_model_name';
test = test_name_l(test, [], newName, expName);

%endfunction

%----------------------1--------------------------------------------------------
function test = lvlTwo8(test, testparams)
% Test for block with name 'matrix'

newName = 'matrix';
expName = 'matrix0';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo9(test, testparams)
% Test for block with name 'vector'

newName = 'vector';
expName = 'vector0';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo10(test, testparams)
% This tests assumes that mconversion1 to mconversion12
% exist and mconversion13 does not exist. Therefore, the new suggested name
% will be mconversion13.

newName = 'mconversion1';
expName = 'mconversion13';
test = test_name_l(test, [], newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo11(test, testparams)
% This for valid name with a model in memory.

new_system('myModelInMem');
newName = 'myModelInMem';
expName = 'myModelInMem0';
test = test_name_l(test, [], newName, expName);
close_system('myModelInMem');

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo12(test, testparams)
% Test for code coverage: maxIteration is specified
%
newName = 'mconversion1';
expName = '';
test = test_name_l(test, 2, newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo13(test, testparams)
% Test for simulink as the model name

newName = 'simulink';
expName = 'simulink0';
test = test_name_l(test, 2, newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo14(test, testparams)
% Test for model with new line in the name

newName = sprintf('my name with \n new line');
expName = 'my_name_with_new_line';
test = test_name_l(test, 2, newName, expName);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo15(test, testparams)
% Test for model with the same name as a directory on path

d = qeWorkingDir; %#ok
mkdir('foo');
newName = 'foo';
expName = newName;
test = test_name_l(test, 2, newName, expName);

%endfunction



%------------------------------------------------------------------------------
function test = test_name_l(test, maxIter, name, expName)

lasterr('');
actName = '';
try
    load_system('mvalidname');
    blkH = get_param('mvalidname/ssBlock', 'handle');
    
    set_param(blkH,'name', name);
    if isempty(maxIter)
        actName = sl('slss2mdl_util','get_valid_mdl_name', blkH);
    else
        actName = sl('slss2mdl_util','get_valid_mdl_name', blkH, maxIter);
    end
end
test = qeverify(test, {lasterr, ''}, {actName, expName});
close_system('mvalidname', 0);
%endfunction
