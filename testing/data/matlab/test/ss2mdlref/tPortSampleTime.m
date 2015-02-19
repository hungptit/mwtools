function [test] = tportSampleTime(point) %#ok - mlint
%
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.

% Copyright 2008 The MathWorks, Inc.
  testharness;
end


%% -----------------------------------------------------------------------  
function [test,testparams]=setup(test,testparams)
    testparams = slrtw_remove_drool(testparams, 'setup');
end % setup


%% -----------------------------------------------------------------------
function test = cleanup(test,testparams)
    slrtw_remove_drool(testparams, 'cleanup');
end % cleanup


%% -----------------------------------------------------------------------
function [test, testparams] = unitsetup(test, testparams) %#ok - mlint
    testparams.needCleanup = 0;
    [testparams.startdir, testparams.testdir, test, hadErr] =  rtwtestsetup(test);
    if (hadErr)
        error('rtwtestsetup failed');
    end
    testparams.needCleanup = 1;
end %unitsetup


%% -----------------------------------------------------------------------
function test = unitcleanup(test, testparams) %#ok - mlint
    if testparams.needCleanup
        rtwtestcleanup(testparams.startdir, testparams.testdir);
    end
end % unitcleanup


%% -----------------------------------------------------------------------
function test = lvlTwo_inportInherited(test, testparams) %#ok - mlint
% Test that the set value is not copied over on inherited sample times
    model  = 'mPortSampleTime';
    subsys = 'subsys';
    port   = 'In2';
       
    load_system(model);
    
    assignin('base', 'paramIn',  -1);
    assignin('base', 'paramOut', .1);
    
    subsysBlock  = [model, '/', subsys];
    newModel     = 'mynewmdl';
    newPortBlock = [newModel, '/', port];
    Simulink.SubSystem.convertToModelReference(subsysBlock, newModel, 'ReplaceSubsystem', false);

    newVal  = get_param(newPortBlock, 'SampleTime');

    % newVal should be a string of the form [ <period>, <offset> ]
    % use eval to turn this into an array, which we can check for
    % correctness
    newVal = eval(newVal);
    test = qeverify(test, {newVal, [evalin('base', 'paramOut'), 0], '=='});
    
    rtwtestclosemodel(model);
    rtwtestclosemodel(newModel);
    rtwtestclosemodel('untitled');
    evalin('base', 'clear(''paramIn'')');
    evalin('base', 'clear(''paramOut'')');
end % lvlTwo_inportInherited


%% -----------------------------------------------------------------------
function test = lvlTwo_inportNotInherited(test, testparams) %#ok - mlint
% Test that the set value is copied over on non-inherited sample times
    model  = 'mPortSampleTime';
    subsys = 'subsys';
    port   = 'In2';
       
    load_system(model);
    
    assignin('base', 'paramIn',  .1);
    assignin('base', 'paramOut', .1);
    
    subsysBlock  = [model, '/', subsys];
    portBlock    = [model, '/', subsys, '/', port];
    newModel     = 'mynewmdl';
    newPortBlock = [newModel, '/', port];
    Simulink.SubSystem.convertToModelReference(subsysBlock, newModel, 'ReplaceSubsystem', false);

    origVal = get_param(portBlock,    'SampleTime');
    newVal  = get_param(newPortBlock, 'SampleTime');
    
    test = qeverify(test, {newVal, origVal, '=='});
    
    rtwtestclosemodel(model);
    rtwtestclosemodel(newModel);
    rtwtestclosemodel('untitled');
    evalin('base', 'clear(''paramIn'')');
    evalin('base', 'clear(''paramOut'')');
end % lvlTwo_inportNotnherited


%% -----------------------------------------------------------------------
function test = lvlTwo_outportInherited(test, testparams) %#ok - mlint
% Test that the set value is not copied over on inherited sample times
    model  = 'mPortSampleTime';
    subsys = 'subsys';
    port   = 'Out2';
       
    load_system(model);
    
    assignin('base', 'paramIn',  .1);
    assignin('base', 'paramOut', -1);
    
    subsysBlock  = [model, '/', subsys];
    newModel     = 'mynewmdl';
    newPortBlock = [newModel, '/', port];
    Simulink.SubSystem.convertToModelReference(subsysBlock, newModel, 'ReplaceSubsystem', false);

    newVal  = get_param(newPortBlock, 'SampleTime');

    % newVal should be a string of the form [ <period>, <offset> ]
    % use eval to turn this into an array, which we can check for
    % correctness
    newVal = eval(newVal);
    test = qeverify(test, {newVal, [evalin('base', 'paramIn'), 0], '=='});
    
    rtwtestclosemodel(model);
    rtwtestclosemodel(newModel);
    rtwtestclosemodel('untitled');
    evalin('base', 'clear(''paramIn'')');
    evalin('base', 'clear(''paramOut'')');
end % lvlTwo_outportInherited


%% -----------------------------------------------------------------------
function test = lvlTwo_outportNotInherited(test, testparams) %#ok - mlint
% Test that the set value is copied over on non-inherited sample times
    model  = 'mPortSampleTime';
    subsys = 'subsys';
    port   = 'Out2';
       
    load_system(model);
    
    assignin('base', 'paramIn',  .1);
    assignin('base', 'paramOut', .1);
    
    subsysBlock  = [model, '/', subsys];
    portBlock    = [model, '/', subsys, '/', port];
    newModel     = 'mynewmdl';
    newPortBlock = [newModel, '/', port];
    Simulink.SubSystem.convertToModelReference(subsysBlock, newModel, 'ReplaceSubsystem', false);

    origVal = get_param(portBlock,    'SampleTime');
    newVal  = get_param(newPortBlock, 'SampleTime');
    
    test = qeverify(test, {newVal, origVal, '=='});
    
    rtwtestclosemodel(model);
    rtwtestclosemodel(newModel);
    rtwtestclosemodel('untitled');
    evalin('base', 'clear(''paramIn'')');
    evalin('base', 'clear(''paramOut'')');
end % lvlTwo_outportNotnherited