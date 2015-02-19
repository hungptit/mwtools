function [test] = tdworks(point)
% Test suite for issues with dworks crossing system boundaries
% for automatic conversion tool for model reference
%

% Copyright 2004-2009 The MathWorks, Inc.
%  $Revision: 1.1.4.7 $
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
  testparams = qe_model_reference(testparams,'setup');
  testparams.convFeature = slfeature('ModelreferenceconversionTesting');
  slfeature('ModelreferenceconversionTesting',0);

function test = cleanup(test,testparams)
%CLEANUP Close figure windows created by setup.
  testparams = qe_model_reference(testparams,'cleanup');
  slfeature('ModelreferenceconversionTesting', testparams.convFeature);
  
%-----------------------------------------------------------------------------
function test = lvlTwoDworksCrossing1(test, testparams)
% Test that get_param(gcb,'CompiledRTWSystemInfo') returns the number of 
% dworks crossing system boundaries.
  
  model = 'mconversionDworkCrossing1';
  load_system(model);

  eval([model,'([],[],[],''compile'');']);
  
  sysInfo = get_param([model,'/DoesNotHaveDwork'],...
                      'CompiledRTWSystemInfo');
  test = qeverify(test,{sysInfo(5),0,'==','testdesc',...
                      'There should be no dworks crossing system boundaries'});
  
  sysInfo = get_param([model,'/DoesNotHaveDwork/HasOneCrossingDwork'],...
                      'CompiledRTWSystemInfo');
  test = qeverify(test,{sysInfo(5),1,'==','testdesc',...
                      'There should be one dwork crossing system boundaries'});

  eval([model,'([],[],[],''term'');']);

  close_system(model);
  
%-------------------------------------------------------------------------------
function test = lvlTwoDworksCrossing2(test, testparams)
% Test that get_param(gcb,'CompiledRTWSystemInfo') returns the number of 
% dworks crossing system boundaries. The subsystem with the dwork crossing is
% is marked as function not auto   
  
  model = 'mconversionDworkCrossing2';
  load_system(model);

  eval([model,'([],[],[],''compile'');']);
  
  sysInfo = get_param([model,'/DoesNotHaveDwork'],...
                      'CompiledRTWSystemInfo');
  test = qeverify(test,{sysInfo(5),0,'==','testdesc',...
                      'There should be no dworks crossing system boundaries'});
  
  sysInfo = get_param([model,'/DoesNotHaveDwork/HasOneCrossingDwork'],...
                      'CompiledRTWSystemInfo');
  test = qeverify(test,{sysInfo(5),1,'==','testdesc',...
                      'There should be one dwork crossing system boundaries'});

  eval([model,'([],[],[],''term'');']);

  close_system(model);
  
