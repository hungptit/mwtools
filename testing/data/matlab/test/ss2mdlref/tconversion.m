function [test] = tconversion(point)
% Test suite for automatic conversion tool for model reference
%

% Copyright 2003-2012 The MathWorks, Inc.
%  
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
  
  
%--------------------------------------------------------------------------
function test = lvlTwoEnumDT(test, testparams)
% Test that MCOS enums work with convert to model reference
%
%   Test Information
%   ----------------
%   Test Plan Location:
%   http://inside.mathworks.com/wiki/Enumerated_Data_Types_In_Simulink_Testplan
%   Test Plan Objective:        Simulink Usage (SU-31)
%   Feature:                    Enumerated Data Types
%   Test Type: 
%   Associated Gecks:           g406474
%   Keywords:
%   Targeted Source 
%   Associated Test Files:
%   Release:                    2008b
%   Dependencies:
%
%   Mike Wilkinson

  origModel = 'mconversion_enumDT';
  
  [startdir, testdir, test, hadErr] = rtwtestsetup(test);
  if hadErr
    error('rtwtestsetup failed');
  end
  
  
  load_system(origModel);
  
  modes{1}.SimOnly=1;
  test=rtwsim(test,origModel,modes,{'StayInCurrDir',1});
  
  load_system(origModel);
  set_param(origModel, 'InlineParams', 'on');
  set_param(origModel, 'SignalResolutionControl', 'UseLocalSettings');
  set_param(origModel, 'StrictBusMsg', 'ErrorLevel1');
  
  success = Simulink.SubSystem.convertToModelReference([origModel ,'/Atomic_MdlRef'],'AtomicBusSub','ReplaceSubsystem',true);

  test = qeverify(test,{success,true,'==','testdesc','Error trying to convert subsystem /Atomic_MdlRef to ModelRef'});
  
  success = Simulink.SubSystem.convertToModelReference([origModel ,'/Convert_MdlRef'],'AtomicEnumSub','ReplaceSubsystem',true);

  test = qeverify(test,{success,true,'==','testdesc','Error trying to convert subsystem /Convert_MdlRef to ModelRef'});
  
  test=rtwsim(test,origModel,modes,{'StayInCurrDir',1});
  
  
  evalin('base','clear BogusVar EnumType_Bus e1 e2;');
  rtwtestcleanup(startdir, testdir);
 
%------------------------------------------------------------------------- 
function test = lvlTwo_g615503(test, testparams)
    % Testing correct signal object propagation from subsystem to converted model block

    mdl = 'mg615503';

    tmpDir = qeWorkingDir(); %#ok

    sw1 = qeSuppressWarningID('Simulink:utility:slUtilityCompBusCannotUseSignalNameForBusName'); %#ok
    sw2 = qeSuppressWarningID('Simulink:Engine:SourceInheritedTS'); %#ok
    load_system(mdl);
    oc1 = onCleanup(@() rtwtestclosemodel(mdl));

    success = Simulink.SubSystem.convertToModelReference([mdl, '/ToConvert'], 'AfterConvert', 'ReplaceSubsystem', true);
    qeverify({success, true, '==', 'testdesc', 'Error during conversion to model reference'});

    oc2 = onCleanup(@() rtwtestclosemodel('AfterConvert'));

    sim(mdl);

    evalin('base', 'clear slBus3 slBus1 REG');
    
%------------------------------------------------------------------------- 
function test = lvlTwo_ConfigSetRef(test,testparams)
    subsys = 'mConfigSetRef';
    mdlRef = 'newmodel';
    tmpDir = qeWorkingDir();

    evalin('base','load(''dConfigSetRef.mat'')');
    oc1 = onCleanup(@() evalin('base','clear(''cs'')'));

    load_system(subsys);          
    oc2 = onCleanup(@() rtwtestclosemodel(subsys));
    oc3 = onCleanup(@() rtwtestclosemodel(mdlRef));
    oc4 = onCleanup(@() rtwtestclosemodel('untitled'));

    success = Simulink.SubSystem.convertToModelReference([subsys, '/Atomic Subsystem'], mdlRef);    
    qeverify({success, true, '==', 'testdesc', 'Error during conversion to model reference'});

    mdlRefConf = getActiveConfigSet(mdlRef);
    qeverify({isa(mdlRefConf, 'Simulink.ConfigSetRef'),true,'==','testdesc','Model did not use config set reference'});
    
    subsysConf = getActiveConfigSet(subsys);
    qeverify({mdlRefConf.WSVarName , subsysConf.WSVarName , '==','testdesc','Compare if the WSVarName of parent and child are the same'});
    
%-------------------------------------------------------------------------    
%% This test point verifies if the requirement information of a given subsystem and that of its referenced model are the same or not.
function test = lvlTwo_CopyParameters(test , testparams)
    mdlName    = 'mCopyParam';    
    subsysName = [mdlName, '/SS1'];
    newSubsysName = ['untitled', '/SS1'];
    mdlRef     = 'mRefModel';
    tmpDir = qeWorkingDir(); %#ok

    open_system(mdlName);
    oc1 = onCleanup(@() rtwtestclosemodel(mdlName, true));
    oc2 = onCleanup(@() rtwtestclosemodel(mdlRef, true));
    
    % Case 1: The referenced model is created and opened in a new window
    req1 = rmi.getReqs(subsysName);
    Simulink.SubSystem.convertToModelReference(subsysName, [mdlRef, '1']);    
    req2 = rmi.getReqs(newSubsysName); 
    qeverify({isequal(req1, req2), true, '==', 'testdesc', ...
              'The requirement information of the original subsystem and that of a referenced model are different!'});
    
    % Case 2: 
    Simulink.SubSystem.convertToModelReference(subsysName, [mdlRef, '2'], 'ReplaceSubsystem', true);    
    req3 = rmi.getReqs(subsysName); 
    qeverify({isequal(req1, req3), true, '==', 'testdesc', ...
              'The requirement information of the original subsystem and that of a referenced model are different!'});

%% ------------------------------------------------------------------------
%% Positive test point for the conversion script
function test = lvlTwoConversionInputs(test, testparams)        
    obj = qeWorkingDir(); %#ok
    mdlName = 'mSubsystemConversionCheck';
    subsysNames  = {[mdlName '/Subsystem']; ...
                    [mdlName '/Subsystem1']; ...
                    [mdlName '/Subsystem2']; ...
                    [mdlName '/Subsystem3']};
    mdlRefNames  = {'mynewmodel1'; ...
                    'mynewmodel2'; ...
                    'mynewmodel3'; ...
                    'mynewmodel4'};
    
    % Load the model
    load_system(mdlName);
    c1 = onCleanup(@() rtwtestclosemodel(mdlName, true));
    
    subsysHandles = get_param(subsysNames, 'Handle');
    
    % Case 1:  subsys is a subsystem full name.    
    subsysIdx = 1;
    sobj = Simulink.ModelReference.ConversionParameters(subsysNames{subsysIdx}, mdlRefNames{subsysIdx});
    test = qeverify(test, ...
                    {subsysNames{subsysIdx}, sobj.SubsystemNames{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'}, ...
                    {subsysHandles{subsysIdx}, sobj.SubsystemHandles{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'});
    
    % Case 2:  subsys is a handle of a subsystem.    
    subsysIdx = 2;
    sobj = Simulink.ModelReference.ConversionParameters(subsysHandles{subsysIdx}, mdlRefNames{subsysIdx});
    test = qeverify(test, ...
                    {subsysNames{subsysIdx}, sobj.SubsystemNames{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'}, ...
                    {subsysHandles{subsysIdx}, sobj.SubsystemHandles{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'});    
    
    % Case 3:  subsys is a 1x1 cell of subsystem name.
    subsysIdx = 3;
    sobj = Simulink.ModelReference.ConversionParameters({subsysNames{subsysIdx}}, mdlRefNames{subsysIdx}); %#ok
    test = qeverify(test, ...
                    {subsysNames{subsysIdx}, sobj.SubsystemNames{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'}, ...
                    {subsysHandles{subsysIdx}, sobj.SubsystemHandles{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'});    
    
    % Case 4:  subsys is a 1x1 cell of subsystem handle.
    clear sobj;
    subsysIdx = 2;
    sobj = Simulink.ModelReference.ConversionParameters({subsysHandles{subsysIdx}}, mdlRefNames{subsysIdx}); %#ok
    test = qeverify(test, ...
                    {subsysNames{subsysIdx}, sobj.SubsystemNames{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'}, ...
                    {subsysHandles{subsysIdx}, sobj.SubsystemHandles{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'});    
        
    % Case 5:  mdlRef is a string.
    subsysIdx = 1;
    sobj = Simulink.ModelReference.ConversionParameters(subsysNames{subsysIdx}, mdlRefNames{subsysIdx});
    test = qeverify(test, ...
                    {mdlRefNames{subsysIdx}, sobj.ModelReferenceNames{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'});
        
    % Case 6:  mdlRef is a 1x1 cell of string.
    subsysIdx = 3;
    sobj = Simulink.ModelReference.ConversionParameters(subsysNames{subsysIdx}, {mdlRefNames{subsysIdx}}); %#ok
    test = qeverify(test, ...
                    {mdlRefNames{subsysIdx}, sobj.ModelReferenceNames{1}, '==', ...
                     'testdesc', 'Verify if the object is initialized correctly or not.'});        
        
    % Case 7:  Force is on
    subsysIdx = 1;
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'Force', true);
    test = qeverify(test, ...
                    {sobj.Force, 'on', '==', ...
                     'testdesc', 'Verify if the ''Force'' parameter is initialized correctly or not.'});
    
    % Case 8:  Force is off.
    subsysIdx = 2;
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'Force', false);
    test = qeverify(test, ...
                    {sobj.Force, 'off', '==', ...
                     'testdesc', 'Verify if the ''Force'' parameter is initialized correctly or not.'});
    
    % Case 9:  Inline replacement is off.
    subsysIdx = 3;
    val = false;
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'ReplaceSubsystem', val);
    test = qeverify(test, ...
                    {sobj.ReplaceSubsystem, val, '==', ...
                     'testdesc', 'Verify that the ''ReplaceSubsystem'' parameter is initialized correctly.'});
    
    % Case 10: Inline replacement is on.
    subsysIdx = 4;
    val = true;
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'ReplaceSubsystem', val);
    test = qeverify(test, ...
                    {sobj.ReplaceSubsystem, val, '==', ...
                     'testdesc', 'Verify that the ''ReplaceSubsystem'' parameter is initialized correctly.'});    
    
    % Case 11: Bus save format is 'Object'.
    subsysIdx = 4;
    val = 'Object';
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'BusSaveFormat', val);
    test = qeverify(test, ...
                    {sobj.BusSaveFormat, val, '==', ...
                     'testdesc', 'Verify that the ''BusSaveFormat'' parameter is initialized correctly.'});        
    
    % Case 12: Bus save format is 'Cell'
    subsysIdx = 2;
    val = 'Cell';
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'BusSaveFormat', val);
    test = qeverify(test, ...
                    {sobj.BusSaveFormat, val, '==', ...
                     'testdesc', 'Verify that the ''BusSaveFormat'' parameter is initialized correctly.'});            
    
    % Case 13: Build target is off.
    subsysIdx = 3;
    val = 'Sim';
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'BuildTarget', val);
    test = qeverify(test, ...
                    {sobj.BuildTarget, val, '==', ...
                     'testdesc', 'Verify that the ''BuildTarget'' parameter is initialized correctly.'});            
    
    % Case 14: Build target is on.
    subsysIdx = 1;
    val = 'RTW';
    sobj = Simulink.ModelReference.ConversionParameters(...
        subsysNames{subsysIdx}, mdlRefNames{subsysIdx}, ...
        'BuildTarget', val);
    test = qeverify(test, ...
                    {sobj.BuildTarget, val, '==', ...
                     'testdesc', 'Verify if the ''BuildTarget'' parameter is initialized correctly or not.'});                
    
    % Case 15: subsys is a cell array of subsystem names.
    sobj = Simulink.ModelReference.ConversionParameters(subsysNames);
    
    test = qeverify(test, ...
                    {sobj.SubsystemNames, subsysNames, '==', ...
                     'testdesc', 'Verify if the ''SubsystemNames'' are initialized correctly or not.'}, ...
                    {sobj.SubsystemHandles, subsysHandles, '==', ...
                     'testdesc', 'Verify if the ''SubsystemNames'' are initialized correctly or not.'});                            

    % Case 16: subsys is a cell array of subsystem handles.
    sobj = Simulink.ModelReference.ConversionParameters(subsysHandles);
    test = qeverify(test, ...
                    {sobj.SubsystemNames, subsysNames, '==', ...
                     'testdesc', 'Verify if the ''SubsystemNames'' are initialized correctly or not.'}, ...
                    {sobj.SubsystemHandles, subsysHandles, '==', ...
                     'testdesc', 'Verify if the ''SubsystemNames'' are initialized correctly or not.'});                            
    
    % Case 17: Check if subsystems are convertible.    
    sobj = Simulink.ModelReference.SubsystemConversionCheck(subsysNames);
    sobj.checkForError();
    
    sobj = Simulink.ModelReference.SubsystemConversionCheck(subsysHandles);
    sobj.checkForError();

    
%% ------------------------------------------------------------------------
%% These are negative test points for the conversion script 
function test = lvlTwoConversionInputsNeg(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    mdlName = 'mSubsystemConversionCheck';    
    subsysNames  = {[mdlName '/Subsystem']; ...
                    [mdlName '/Subsystem4']};
    mdlRefNames  = {'mynewmodel1'; ...
                    'mynewmodel4'};

    load_system(mdlName);
    c1 = onCleanup(@() rtwtestclosemodel(mdlName, true));
    c2 = onCleanup(@() rtwtestclosemodel(mdlRefNames{1}, true));
    c3 = onCleanup(@() rtwtestclosemodel(mdlRefNames{2}, true));
    
    subsysHandles = get_param(subsysNames, 'Handle');
    
    % Case 1: Invalid BuildTarget
    qeVerifyError('Simulink:modelReference:convertToModelReference_BuildTarget', ...
                  @() Simulink.ModelReference.SubsystemConversionCheck(subsysNames{1}, mdlRefNames{1}, 'BuildTarget', 'foo'));
    
    % Case 2: Invalid Bus save format -> We do not check it here.
    qeVerifyError('Simulink:modelReference:BusSaveFormat', ...
                  @() Simulink.ModelReference.SubsystemConversionCheck(subsysNames{1}, mdlRefNames{1}, 'BusSaveFormat', 'foo'));
    
    % Case 3: Invalid Force
    qeVerifyError('Simulink:modelReference:convertToModelReference_Force', ...
                  @() Simulink.ModelReference.SubsystemConversionCheck(subsysNames{1}, mdlRefNames{1}, 'Force', 'foo'));
    
    % Case 4: Invalid inline replacement
    qeVerifyError('Simulink:modelReference:convertToModelReference_ReplaceSubsystem', ...
                  @() Simulink.ModelReference.SubsystemConversionCheck(subsysNames{1}, mdlRefNames{1}, 'ReplaceSubsystem', 'foo'));
    
    % Case 5: Invalid parameter name
    qeVerifyError('Simulink:modelReference:convertToModelReference_InvalidInputArgument', ...
                  @() Simulink.ModelReference.SubsystemConversionCheck(subsysNames{1}, mdlRefNames{1}, 'BuildThisTarget', 'foo'));    
    
    % Case 6: Subsystems are not convertible
    obj = Simulink.ModelReference.SubsystemConversionCheck(subsysHandles);
    qeVerifyError('Simulink:modelReference:invalidSSTypeVirtual', @() obj.checkForError());
    
    % Case 7: Cannot convert more than one subsystems at a time
    qeVerifyError('Simulink:modelReference:convertToModelReference_InvalidNumOfSubsystems', ...
                  @() Simulink.SubSystem.convertToModelReference(subsysNames, mdlRefNames));
    
%% ------------------------------------------------------------------------
function test = lvlTwoCheckExceptionIdentifier(test, testparams)
  
    msgId  = 'Simulink:modelReference:convertToModelReference_InvalidNumNameValuePair';
    ex1 = MException(msgId, DAStudio.message(msgId));

    msgId = 'Simulink:modelReference:convertToModelReference_BuildTarget';
    ex2 = MException(msgId, DAStudio.message(msgId));

    msgId = 'Simulink:modelReference:convertToModelReference_InvalidNumNameValuePair';
    ex3 = MException(msgId, DAStudio.message(msgId));

    msgId = 'Simulink:modelReference:convertToModelReference_Force';
    ex4 = MException(msgId, DAStudio.message(msgId));

    msgId = 'Simulink:modelReference:convertToModelReference_ReplaceSubsystem';
    ex5 = MException(msgId, DAStudio.message(msgId));

    msgId = 'Simulink:modelReference:BusSaveFormat';
    ex6 = MException(msgId, DAStudio.message(msgId));

    msgId = 'Simulink:modelReference:convertToModelReference_InvalidInputArgument';
    ex7 = MException(msgId, DAStudio.message(msgId, ''));

    % Build an exception tree
    ex4 = addCause(ex4, ex6);
    ex3 = addCause(ex3, ex4);
    ex3 = addCause(ex3, ex5);

    ex2 = addCause(ex2, ex7);
    ex1 = addCause(ex1, ex2);
    ex1 = addCause(ex1, ex3);
    
    % Positive test
    msgId = 'Simulink:modelReference:BusSaveFormat';
    qeverify({Simulink.ModelReference.checkExceptionIdentifier(ex1, msgId), true, '=='});

    % Negative test
    msgId = 'Simulink:modelReference:convertToModelReference_inportInvalidDownStreamSampleTimeWarn';
    qeverify({Simulink.ModelReference.checkExceptionIdentifier(ex1, msgId), false, '=='});

%% ------------------------------------------------------------------------
function test = lvlTwoSubsystemTypeNeg(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    
    modelName  = 'mSubsystemTypeNeg';
    subsysName = [modelName, '/Gain'];
    
    open_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
        
    subsysHandle = get_param(subsysName, 'Handle');   
    ssType1 = Simulink.SubsystemType(subsysName);
    ssType2 = Simulink.SubsystemType(modelName);
    
    % Invalid number of arguments
    qeVerifyError('Simulink:Commands:TooManyInputArgs', @() slInternal('isSubsystem', subsysHandle, 'test'));
    qeVerifyError('Simulink:Commands:TooManyOutputArgs', @() invalidNumberOfOutputArguments('isSubsystem', subsysHandle));
    
    % Invalid subsystem
    qeverify({ssType1.isSubsystem(), false, '=='});
    qeverify({ssType2.isSubsystem(), false, '=='});


%% ------------------------------------------------------------------------    
function test = lvlTwo_g868873(test, testparams)    
    tmpDir = qeWorkingDir(); %#ok
    modelName = 'mg868873';
    subsysName = 'mg868873/Subsystem1';
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    % Check that subsystem type is 'iterator'
    this = Simulink.SubsystemType(subsysName);
    qeverify({this.getType, 'iterator', '=='});
    
    % Verify returned exception when converting this subsystem to a referecend model
    qeVerifyError('Simulink:modelReference:invalidSSTypeIterator', ...
                  @() Simulink.SubSystem.convertToModelReference(subsysName, 'new_mdlref'));
    
    
   
%% ------------------------------------------------------------------------
function test = lvlTwo_g872346(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    
    modelName  = 'mg872346';
    subsys = [modelName, '/model'];
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName, true));
        
    % Convert to referenced model with default option won't save bus objects to M file
    Simulink.SubSystem.convertToModelReference(subsys, 'refmodel1');
    qeverify({isempty(dir('refmodel1_bus.m')), true, '=='});
    
    % Clear all variables to make sure that there is no warning about existing bus objects
    evalin('base', 'clear;')           
    
    % Convert with specified BusSaveFormat will save bus object to a M file
    Simulink.SubSystem.convertToModelReference(subsys, 'refmodel2', 'BusSaveFormat', 'Object');
    qeverify({isempty(dir('refmodel2_bus.m')), false, '=='});

    
%% ------------------------------------------------------------------------
function test = lvlTwo_mModelRefFileNameWarning_g883081(test, testparams)
    tmpDir = qeWorkingDir(); %#ok    
    modelName  = 'mModelRefFileNameWarning_g883081';
    subsys = [modelName, '/Subsystem'];
    
    load_system(modelName);    
    c1 = onCleanup(@() rtwtestclosemodel(modelName, true));
    
    oldList = find_system('type', 'block_diagram');    
    subsysHandle = get_param(subsys, 'Handle');
    qeVerifyWarningFree(@() sl('slss2mdl_util', 'convertSubsystemToModelrefFromUI_l', subsysHandle));
    
    % Make sure that a model block is created in a new model 
    newList = find_system('type', 'block_diagram');
    createdModels = setdiff(newList, oldList);
    
    qeverify({get_param(createdModels{1}, 'Name'), 'Subsystem', '=='});
    qeverify({get_param([createdModels{2}, '/Subsystem'], 'Name'), 'Subsystem', '=='});
