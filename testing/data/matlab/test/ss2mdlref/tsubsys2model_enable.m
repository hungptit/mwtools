function [test] = tsubsys2model_enable(point)
% 
% Tests for converting enabled subsystems to models
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.

% Copyright 2011-2012 The MathWorks, Inc.
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
function test = lvlTwoEnabledSubsys(test, testparams)
% Test that the conversion script works for enabled subsystems
  model = 'menablesubsys';
  subsys = [model '/Subsystem'];
  mdlRef = 'myNewModel';
  
  baseFileName = 'benablesubsys';
  test = htest_subsys2model(test, baseFileName, model, subsys, mdlRef);
end

%------------------------------------------------------------------------------
function test = lvlTwoEnabledTriggeredSubsys(test, testparams)
% Test that the conversion script works for enabled & triggered subsystems
  model = 'menabtrigsubsys';
  subsys = [model '/Subsystem'];
  mdlRef = 'myNewModel';
  
  baseFileName = 'benabtrigsubsys';
  test = htest_subsys2model(test, baseFileName, model, subsys, mdlRef);
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs.
function test = lvlTwoEnabledVarDims(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs in a virtual bus.
function test = lvlTwoEnabledVarDims_virtbus(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize_virtbus';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs in a nonvirtual bus.
function test = lvlTwoEnabledVarDims_nonvirtbus(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize_nonvirtbus';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs in a virtual bus w/ bus object
function test = lvlTwoEnabledVarDims_virtbuswbo(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize_virtbuswbo';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs in a nested virtual bus.
function test = lvlTwoEnabledVarDims_nestedvbus(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize_nestedvirtbus';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs in a nested virtual bus w/ bus object
function test = lvlTwoEnabledVarDims_nestedvbuswbo(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize_nestedvirtbuswbo';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%------------------------------------------------------------------------------
% Make sure we get an error when converting an enabled subsytem
% with vardims outputs in a nested nonvirtual bus.
function test = lvlTwoEnabledVarDims_nestednvbus(test, testparams)
    obj = qeWorkingDir(); %#ok

    model = 'mEnabSSVarSize_nestednvbus';
    newModel = 'mySS';
    subsys = [model, '/Subsystem'];
    load_system(model);

    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    qeVerifyError('Simulink:modelReference:variableDimsNotAllowedOnOutput',...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end
