function [test] = tsubsys2model_ctrlports(point)
% 
% Tests for converting cond-execed subsystems to models
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.

% Copyright 2011 The MathWorks, Inc.
  testharness;
end

% -----------------------------------------------------------------------  
function [test,testparams]=setup(test, testparams)
% SETUP setup features
    testparams = slrtw_remove_drool(testparams, 'setup');
end %setup


% -----------------------------------------------------------------------
function test = cleanup(test, testparams)
% CLEANUP cleanup
    slrtw_remove_drool(testparams, 'cleanup');
end % cleanup

%------------------------------------------------------------------------------
% Make sure we can convert a subsystem with no inputs and outputs, but having
% a control port.
function test = lvlTwoEnabPort_noio(test, testparams)
    model = 'menabport_noio';
    
    obj = qeWorkingDir; %#ok
    
    load_system(model);
    
    subsys = [model '/Subsystem'];
    
    mdlRef = 'myNewModel';
    baseFileName = 'benabport_noio';
    test = htest_subsys2model(test, baseFileName, model, subsys, mdlRef);
    
end

%------------------------------------------------------------------------------
% Make sure we can convert a subsystem with no inputs and outputs, but having
% a control port.
function test = lvlTwoTrigPort_noio(test, testparams)
    model = 'mtrigport_noio';
    
    obj = qeWorkingDir; %#ok
    
    load_system(model);
    
    subsys = [model '/Subsystem'];
    
    mdlRef = 'myNewModel';
    baseFileName = 'btrigport_noio';
    test = htest_subsys2model(test, baseFileName, model, subsys, mdlRef);
    
end

%------------------------------------------------------------------------------
% Make sure we can convert a subsystem with no inputs and outputs, but having
% a control port.
function test = lvlTwoEnabTrigPort_noio(test, testparams)
    model = 'menabtrig_noio';
    
    obj = qeWorkingDir; %#ok
    
    load_system(model);
    
    subsys = [model '/Subsystem'];
    
    mdlRef = 'myNewModel';
    baseFileName = 'benabtrig_noio';
    test = htest_subsys2model(test, baseFileName, model, subsys, mdlRef);
    
end
