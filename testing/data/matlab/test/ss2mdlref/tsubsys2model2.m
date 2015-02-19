function test = tsubsys2model2(point)
% Test suite for Simulink.SubSystem.convertToModelReference
%

% Copyright 2003-2012 The MathWorks, Inc.
%   
%
% -------------------------------------------------------------------------
% Run the tests and report results
testharness;

% -------------------------------------------------------------------------
function [test, testparams] = setup(test, testparams)
% SETUP Create variables, figures, pcode, etc., for use in all testpoints.
% Store these in fields of the testparams structure even if you do not 
% intend to use testparams, create the variable anyway to avoid bogus 
% warnings in function calls.

testparams.wsVars_before = evalin('base', 'who;');
testparams = qe_model_reference(testparams, 'setup');

% -------------------------------------------------------------------------
function test = cleanup(test, testparams)
% CLEANUP Close figure windows created by setup.

test_cleanup(testparams);
%testparams = slrtw_remove_drool(testparams, 'cleanup');
testparams = qe_model_reference(testparams, 'cleanup');

% -------------------------------------------------------------------------
function test_cleanup(testparams)
testparams.wsVars_after = evalin('base', 'who;');
extra = setdiff(testparams.wsVars_after, testparams.wsVars_before);
assignin('base', 'extra', extra);
evalin('base', 'clear(extra{1:end})');
evalin('base', 'clear extra');

% -------------------------------------------------------------------------
% CLEANUP
function i_jump
% I_JUMP Throw error to cause jump out of try scope.

if isempty(lasterr)
    error('');
else
    error(lasterr);
end

% end i_jump
% -------------------------------------------------------------------------
function test = lvlTwo1(test, testparams)
% Test discrete, zero offset, sample time specified on the Subsystem
% must be used to setup the fixed step size of the new model
model = 'msamp1';

lasterr('');
needCleanup = 0;

try
    [startdir, testdir, test, hadErr] = rtwtestsetup(test);
    if hadErr
        i_jump; 
    end
    needCleanup = 1;
    load_system(model);
    
    % get the fixed step size from subsystem
    ssBlk = 'msamp1/Subsystem';
    Simulink.SubSystem.convertToModelReference(ssBlk, 'mynewmdl', ...
        'ReplaceSubsystem', true);
    fixedStep = get_param('mynewmdl', 'FixedStep');
    test = qeverify(test, {fixedStep, '3'});

    % Remain as model's fixed step size
    ssBlk = 'msamp1/Subsystem1';
    Simulink.SubSystem.convertToModelReference(ssBlk, 'mynewmdl1', ...
        'ReplaceSubsystem', true);
    fixedStep = get_param('mynewmdl1', 'FixedStep');
    test = qeverify(test, {fixedStep, '0.1'});

catch
    disp(['*>Test failed due to error:', lasterr]);
    test = qeverify(test, {1, 0, '==', 'testdesc', ['Failed '...
        'when converting a model to using model reference']});
end
rtwtestclosemodel('mynewmdl1');
rtwtestclosemodel('mynewmdl');
rtwtestclosemodel(model);
if needCleanup
    rtwtestcleanup(startdir, testdir);
end

% -------------------------------------------------------------------------
function test = lvlTwo2(test, testparams)
% Convert a fcn-call subsystem to Model

mdl = 'msl_subsys_fcncall1';
subsys = 'msl_subsys_fcncall1/g';
mdlRef = 'myNewModel';
baseFileName = 'bsl_subsys_fcncall1';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

% -------------------------------------------------------------------------
function test = lvlTwo3(test, testparams)
% Convert a fcn-call subsystem to Model
mdl = 'mfcnreset_periodic1';
subsys = 'mfcnreset_periodic1/FCSS';
mdlRef = 'myNewModel';
baseFileName = 'bfcnreset_periodic1';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

% -------------------------------------------------------------------------
function test = lvlTwo4(test, testparams)
% Test for model workspace (Model File data source)

mdl = 'mmdlwks1';
subsys = 'mmdlwks1/SS';
mdlRef = 'myNewModel';

baseFileName = 'bmdlwks1';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

% -------------------------------------------------------------------------
function test = lvlTwo5(test, testparams)
% Test for model workspace (MATLAB file data source)

mdl = 'mmdlwks2';
subsys = 'mmdlwks2/SS';
mdlRef = 'myNewModel';

baseFileName = 'bmdlwks2';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

% -------------------------------------------------------------------------
function test = lvlTwo6(test, testparams)
% Test for model workspace (MAT-file data source)

mdl = 'mmdlwks3';
subsys = 'mmdlwks3/SS';
mdlRef = 'myNewModel';

baseFileName = 'bmdlwks3';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

% -------------------------------------------------------------------------
function test = lvlTwo7(test, testparams)
% Test for N-D signals at subsystem boundaries
mdl    = 'msubsys2model7';
subsys = 'msubsys2model7/NDSubsys';
mdlRef = 'msubsys2model7nd';

baseFileName = 'bsubsys2model7';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

% -------------------------------------------------------------------------
%function test = lvlTwo_g269585(test, testparams)
% Test for geck g269585
%model = 'mpulse_vector_SimOnly_1';
%expErrId = 'Simulink:modelReference:convertToModelReference_outportInvalidUpStreamSampleTimeErr';
%origListOfOpenModels = find_system('flat');
%tmpDir = qeWorkingDir();%#ok
%load_system(model);

%ssBlk = 'mpulse_vector_SimOnly_1/Enabled Subsystem/Subsystem';

%qeVerifyError(expErrId, @() Simulink.SubSystem.convertToModelReference(ssBlk, 'mynewmodel',...
%    'ReplaceSubsystem',true));

%close_NewOpenModels(origListOfOpenModels);
        
% -------------------------------------------------------------------------
function test = lvlTwo_g327337(test, testparams)
% Test for geck g327337
% When converting a subsystem, report an error if it contains a fcn-call
% subsystem, and its trigger port "State when enabling" parameter is set to
% inherit.

model = 'mg327337';
needCleanup = 0;
lasterr('');

try
    [startdir, testdir, test, hadErr] = rtwtestsetup(test);
    if hadErr, i_jump; end;
    needCleanup = 1;
    lasterr('');
    load_system(model);
    expErrId = 'Simulink:modelReference:convertToModelReference_InvalidSubsystemFcnCallSSWithInheritState';

    % Atomic subsystem has a fcn-call subsystem.
    % Compiled "State when enabling" for the fcn-call subsystem is held
    try
        ssBlk = 'mg327337/held1/SS';
        Simulink.SubSystem.convertToModelReference(ssBlk, 'ss_held', ...
            'ReplaceSubsystem', false);
    end
    err = lasterror;
    test = qeverify(test, {err.identifier, expErrId});

    % Converting a fcn-call subsystem
    try
        ssBlk = 'mg327337/held1/SS/SS_fcn_call';
        Simulink.SubSystem.convertToModelReference(ssBlk, 'ss_held_fcncall', ...
            'ReplaceSubsystem', false);
    end
    err = lasterror;
    test = qeverify(test, {err.identifier, expErrId});

    % Atomic subsystem has a fcn-call subsystem.
    % Compiled "State when enabling" for the fcn-call subsystem is reset
    try
        ssBlk = 'mg327337/reset1/SS';
        Simulink.SubSystem.convertToModelReference(ssBlk, 'ss_reset', ...
            'ReplaceSubsystem', false);
    end
    err = lasterror;
    test = qeverify(test, {err.identifier, expErrId});


    % Atomic subsystem has a fcn-call subsystem inside a trigger subsystem
    % Compiled "State when enabling" for the fcn-call subsystem is reset
    try
        ssBlk = 'mg327337/held3/Subsystem';
        Simulink.SubSystem.convertToModelReference(ssBlk, 'ss_reset', ...
            'ReplaceSubsystem', false);
    end
    err = lasterror;
    test = qeverify(test, {err.identifier, expErrId});

    lasterr('');
catch
    disp(['*>Test failed due to error:', lasterr]);
    test = qeverify(test, {1,0,'==','testdesc',['Failed '...
        'when converting a model to using model reference']});
end
test = qeverify(test, {lasterr, ''});

rtwtestclosemodel('mg327337');
if needCleanup
    rtwtestcleanup(startdir, testdir);
end

% -------------------------------------------------------------------------
function test = lvlTwo_g337440(test, testparams)
% Test for geck g337440
tmpDir = qeWorkingDir();%#ok
model = 'mg337440';
newmodel = 'my_new_model';
load_system(model);
oc1 = onCleanup(@() rtwtestclosemodel(model));
oc2 = onCleanup(@() rtwtestclosemodel(newmodel));

ssBlk = 'mg337440/Atomic Subsystem';
Simulink.SubSystem.convertToModelReference(ssBlk, 'mynewmodel');

close_system(model, 0);
close_system('mynewmodel',0);

% -------------------------------------------------------------------------
function test = lvlTwo_g374610(test, testparams)
% Test for geck g374610. Verity that async fcn-call subsystem is
% converted to modelref without warning meesage
tmpDir = qeWorkingDir();%#ok
model = 'mg374610';
newmodel = 'my_new_model';

load_system(model);
oc1 = onCleanup(@() rtwtestclosemodel(model));
oc2 = onCleanup(@() rtwtestclosemodel(newmodel));

ssBlk = 'mg374610/FS_2';
Simulink.SubSystem.convertToModelReference(ssBlk, newmodel);

close_system(model, 0);

% -------------------------------------------------------------------------
function test = lvlTwo_g403805(test, testparams)
% Test that the conversion works even if the model has the sample 
% time constraint set to SampleTimeIndependent.

mdl = 'mg403805';
subsys = 'mg403805/Blob Analysis';
mdlRef = 'my_new_model';

baseFileName = 'bg403805';
test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

%-------------------------------------------------------------------------
function test = lvlTwo_g467984(test, testparams)
% If the original model is variable-step, we change the converted model to
% fixed-step if the subsystem is a function-call subsystem.
  
  origListOfOpenModels = find_system('flat');

  mdl = 'mg467984';
  subsys1 = [mdl '/fcss1'];
  subsys2 = [mdl '/fcss2'];
  
  load_system(mdl);
  
  [startdir, testdir, test, hadErr] = rtwtestsetup(test);
  if hadErr, error('Error setting up testdir'); end
  
  % fcss1 has a triggered sample time and fcss2 has a periodic sample time,
  % make sure each fcn-call subsystem can be converted without error
  Simulink.SubSystem.convertToModelReference(subsys1, 'newModel1');
  Simulink.SubSystem.convertToModelReference(subsys2, 'newModel2');

  close_NewOpenModels(origListOfOpenModels);  
  rtwtestcleanup(startdir, testdir);
  
%endfunction  

%-------------------------------------------------------------------------
function test = lvlTwo_g493754(test, testparams)
% If the new model name is the name of a directory, we should not error out.
  origListOfOpenModels = find_system('flat');
  
  model = 'mg493754';
  load_system(model);
  subsystem = [model '/Subsystem'];
  
  % cd to a tempdir and create a subdir named foo
  d = qeWorkingDir; %#ok
  mkdir('foo');
  
  % try to convert to an existing subdirectory
  Simulink.SubSystem.convertToModelReference(subsystem, 'foo');
  
  close_NewOpenModels(origListOfOpenModels);  

%endfunction

%-------------------------------------------------------------------------
% function test = lvlTwo_g508648(test, testparams)
% % There was a bug because of a clash between a function argument in 
% % convertToModelReference and the name of the model.  I don't think there's
% % a reliable way of getting the arguments to the function, so for now
% % we'll just name the model 'subsys'.
%   origListOfOpenModels = find_system('flat');
  
%   % To make sure this test is still testing the original error, we need
%   % to make sure that the model name is the same as a variable name in
%   % the function.  
%   convertFile = fullfile(matlabroot, 'toolbox', 'simulink', 'simulink', ...
%                          '@Simulink', '@SubSystem', 'convertToModelReference.m');
  
%   fid = fopen(convertFile, 'r');
%   % Look for the function that puts the model in compile mode and return
%   % the first argument to that function.  If the name of that function
%   % changes please update this pattern.
%   pattern = '.*convert';
%   mdlName = '';
%   while 1
%     tline = fgetl(fid);
%     if ~ischar(tline), break, end
%     varname = regexp(tline, pattern, 'match');
%     if ~isempty(varname)
%       mdlName = varname{1};
%     end
%   end
  
%   fclose(fid);

%   if isempty(mdlName)
%     err = ['Did not find the expected function with this pattern ', ...
%            sprintf('\n'), pattern, sprintf('\n'), ...
%            'in the file ', convertFile, sprintf('\n'), ...
%           'Please update the pattern so that it finds the function that ', ...
%           'puts the model into compiled mode'];
%     test = qeverify(test, {mdlName, 'mdl', '==', 'testdesc', err});
%     return;
%   end
  
%   d = qeWorkingDir; %#ok

%   % Resave the model msubsys to the name we found above.
%   model = 'msubsys';
%   load_system(model);
%   save_system(model, mdlName);
%   subsystem = [mdlName '/Subsystem'];
  
  
%   % try to convert to an existing subdirectory
%   Simulink.SubSystem.convertToModelReference(subsystem, 'newmdlref');
  
%   close_NewOpenModels(origListOfOpenModels);  

%endfunction

%-------------------------------------------------------------------------
function test = lvlTwo_g546934(test, testparams)
  model = 'mconversion_error';
  subsys = [model '/' 'Subsystem'];
  newmodel = 'my_new_model';
  load_system(model);
  c1 = onCleanup(@() rtwtestclosemodel(model));
  c2 = onCleanup(@() rtwtestclosemodel(newmodel));  
  
  obj = qeWorkingDir(); %#ok
  
  test = qeVerifyError('Simulink:modelReference:convertToModelReference_FailedToBuildTarget', ...
                       @() Simulink.SubSystem.convertToModelReference(...
                           subsys, newmodel, 'ReplaceSubsystem', true, 'Force', true, ...
                           'BuildTarget','Sim'));
%endfunction

  
  
%-------------------------------------------------------------------------

function close_NewOpenModels(origListOfOpenModels)
    newListOfOpenModels = find_system('flat');
    
    modelsToClose = '';
    
    for newMdlIdx = 1:length(newListOfOpenModels)
        found = false;
        for origMdlIdx = 1:length(origListOfOpenModels)
            if strcmp(newListOfOpenModels{newMdlIdx},origListOfOpenModels{origMdlIdx})
                found = true;
            end
        end
        if ~found
            modelsToClose{end+1} = newListOfOpenModels{newMdlIdx};
        end
    end
    
    for mdlIdx = 1:length(modelsToClose)
        rtwtestclosemodel(modelsToClose{mdlIdx});
    end
  
%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs1(test, testparams)
% Test for simple variable-size signal input

  mdl = 'mvardims_SS2MdlRefConv';
  subsys = 'mvardims_SS2MdlRefConv/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_SS2MdlRefConv';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
                
%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs2(test, testparams)
% Test for input to subsystem that is a virtual bus composed of a 
% variable-size input

  mdl = 'mvardims_virtualBus';
  subsys = 'mvardims_virtualBus/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_virtualBus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
              
                
%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs3(test, testparams)
% Test for input to subsystem that is a virtual bus (with bus object) 
% composed of a variable-size input

  mdl = 'mvardims_virtualBuswBO';
  subsys = 'mvardims_virtualBuswBO/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_virtualBuswBO';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

                
%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs4(test, testparams)
% Test for input to subsystem that is a nonvirtual bus composed of a 
% variable-size input

  mdl = 'mvardims_nonvirtualBus';
  subsys = 'mvardims_nonvirtualBus/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_nonvirtualBus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
                
%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs5(test, testparams)
% Test for input to subsystem that is a nested virtual bus composed of a 
% variable-size input

  mdl = 'mvardims_nestedvirtualBus';
  subsys = 'mvardims_nestedvirtualBus/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_nestedvirtualBus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
                
%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs6(test, testparams)
% Test for input to subsystem that is a nested virtual bus with bus object
% composed of a variable-size input

  mdl = 'mvardims_nestedvirtualBuswBO';
  subsys = 'mvardims_nestedvirtualBuswBO/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_nestedvirtualBuswBO';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
                
%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs7(test, testparams)
% Test for input to subsystem that is a nested nonvirtual bus
% composed of a variable-size input

  mdl = 'mvardims_nestednonvirtualBus';
  subsys = 'mvardims_nestednonvirtualBus/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_nestednonvirtualBus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs8(test, testparams)
% Test for input to triggered subsystem that is a nonvirtual bus
% composed of a variable-size input

  mdl = 'mvardims_nonvirtualBus_triggSS';
  subsys = 'mvardims_nonvirtualBus_triggSS/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_nonvirtualBus_triggSS';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

%------------------------------------------------------------------------------
function test = lvlTwo_varsize_inputs9(test, testparams)
% Test for input to funcation-call subsystem that is a nonvirtual bus
% composed of a variable-size input

  mdl = 'mvardims_nonvirtualBus_fcnCallSS';
  subsys = 'mvardims_nonvirtualBus_fcnCallSS/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bvardims_nonvirtualBus_fcnCallSS';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
  
%------------------------------------------------------------------------------
function test = lvlTwoTriggered(test, testparams)
% Test that the conversion script works for triggered subsystems
  model = 'mtriggersubsys';
  subsys = [model '/Subsystem'];
  mdlRef = 'my_new_model';  
  baseFileName = 'btriggersubsys';
  test = htest_subsys2model(test, baseFileName, model, subsys, mdlRef);
%endfunction
  
