function [test] = tsubsys2model4(point)
% Test suite for Simulink.SubSystem.convertToModelReference
%

% Copyright 2003-2011 The MathWorks, Inc.
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
  testparams.rec = qeinitpm;
  
function test = cleanup(test,testparams)
%CLEANUP Close figure windows created by setup.
  test_cleanup(testparams);
  testparams = slrtw_remove_drool(testparams,'cleanup');
  qeinitpm(testparams.rec);
  

%-----------------------------------------------------------------------------
function test_cleanup(testparams)
testparams.wsVars_after = evalin('base', 'who;');
extra = setdiff(testparams.wsVars_after, testparams.wsVars_before); %#ok
if(~isempty(extra))
    assignin('base', 'extra', extra);
    evalin('base', 'clear(extra{1:end})');
    evalin('base', 'clear extra');
end % if
    
%CLEANUP 

function i_jump
%I_JUMP Throw error to cause jump out of try scope.

   if isempty(lasterr)
     error('')
   else
     error(lasterr)
   end

%end i_jump

function msgIdPref = msgIdPref_l
   msgIdPref = 'Simulink:modelReference:convertToModelReference_';
%endfunction  msgIdPref_l


%------------------------------------------------------------------------------

% **************************************************
% *                                                *
% *      This test is full.  Please do not add     *
% *        additional testpoints to the file.      *
% *                                                *
% **************************************************

%% -----------------------------------------------------------------------
function test = lvlTwo11(test, testparams)
% Make sure we are using back propagated bus objects
  
  mdl    = 'mbackprop1';
  subsys = 'mbackprop1/down_stream';
  mdlRef = 'myNewModel';
  baseFileName = 'bbackprop1';
  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef, ...
                      'BuildTarget', 'Sim');
  
%endfunction

%------------------------------------------------------------------------------

% **************************************************
% *                                                *
% *      This test is full.  Please do not add     *
% *        additional testpoints to the file.      *
% *                                                *
% **************************************************

