function [test] = tsubsys2model7(point)
% Test suite for Simulink.SubSystem.convertToModelReference
%

% Copyright 2003-2012 The MathWorks, Inc.
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
extra = setdiff(testparams.wsVars_after, testparams.wsVars_before);
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
function test = lvlTwo_g164815_g282480(test, testparams)
% Test for fixed step size (geck g164815 and g282480)
tmpDir = qeWorkingDir();%#ok
model = 'mtiming';
subsys = 'mtiming/parameters';
newmodel = 'my_new_mdl';

load_system(model);
oc1 = onCleanup(@() rtwtestclosemodel(model));
oc2 = onCleanup(@() rtwtestclosemodel(newmodel));

set_param(model,'SignalResolutionControl','UseLocalSettings');

[s, mdlRefH] = Simulink.SubSystem.convertToModelReference(subsys, newmodel, ...
    'ReplaceSubsystem', true);

blkType = get_param(mdlRefH, 'BlockType');

test = qeverify(test, {s, true}, {blkType, 'ModelReference'});

load_system(newmodel);

fixedstep_mdl    = get_param(model, 'FixedStep');
fixedstep_refmdl = get_param(newmodel, 'FixedStep');

test = qeverify(test, {fixedstep_refmdl, fixedstep_mdl}, ...
    {fixedstep_refmdl, '0.1'});

  
%endfunction


%------------------------------------------------------------------------------
function test = lvlTwo_g164815_2(test, testparams)
% Test for fixed step size (geck g164815)
  mdl = 'mtiming';
  subsys = 'mtiming/parameters';
  mdlRef = 'myNewModel';
  baseFileName = 'btiming';
  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
  

%------------------------------------------------------------------------------
function test = lvlTwo_global_data_store(test, testparams)
% Test for global Data Store memory blocks

  mdl = 'mdatastore_glb';
  subsys = 'mdatastore_glb/SS';
  mdlRef = 'myNewModel';
  baseFileName = 'bdatastore_glb';
  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
  
  
%------------------------------------------------------------------------------
function test = lvlTwo_g217910(test, testparams)
% Test for geck 217910

  mdl = 'mclosed_loop_sim';
  subsys = 'mclosed_loop_sim/Throttle Angle Controller';
  mdlRef = 'myNewModel';
  baseFileName = 'bclosed_loop_sim';
  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
  
%------------------------------------------------------------------------------
function test = lvlTwo_194068(test, testparams)
% Test for geck 194068. 
% Test to make sure we turn off OptimizeModelRefInitCode setting if it 
% is on and the subsystem is in another subsystem that resets its states.

  mdl = 'mreset';
  subsys = 'mreset/SS1/SS2/SS3';
  mdlRef = 'myNewModel';
  baseFileName = 'breset';
  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);
  
%------------------------------------------------------------------------------
function test = lvlTwo_structure_bus(test, testparams)
% Test that bus virtuality is preserved after conversion (255421)
% In reference to geck 482613
   sw1 = qeSuppressWarningID('Simulink:utility:slUtilityCompBusCannotUseSignalNameForBusName');

   model = 'mstruct_bus1';
   subModel = 'SS_mdl';
   subModel1 = 'SS1_mdl';
   
   tmpDir = qeWorkingDir; %#ok
   
   load_system(model);
   c1 = onCleanup(@() rtwtestclosemodel(model, true));
   
   set_param(model,'SignalResolutionControl','UseLocalSettings');
     
   ssBlk = [model, '/SS'];
   Simulink.SubSystem.convertToModelReference(ssBlk, subModel, 'ReplaceSubsystem', true);
   
   ssBlk1 = [model, '/SS1'];
   Simulink.SubSystem.convertToModelReference(ssBlk1, subModel1, 'ReplaceSubsystem', true);
     
   % Check bus at the I/O of subModel
   sub = get_sub_model_bus_l(subModel);
   test = qeverify(test, ...
                   {sub.inUseBus, 'on'}, {sub.inOutputAsStruct, 'off'}, ...
                   {sub.outUseBus, 'on'}, {sub.outOutputAsStruct, 'off'});
     
   % Check bus at the I/O of subModel1
   sub1 = get_sub_model_bus_l(subModel1);
   test = qeverify(test, ...
                   {sub1.inUseBus, 'on'}, {sub1.inOutputAsStruct, 'on'}, ...
                   {sub1.outUseBus, 'on'}, {sub1.outOutputAsStruct, 'on'});

   % Make sure we can simulate the model without a compilation error
   mdlInfo =  get_bus_block_info_l(model);
   
   test = qeverify(test, ...
                   {mdlInfo.BC, 0},  {mdlInfo.BS, 0}, ...
                   {mdlInfo.BC1, 1}, {mdlInfo.BS1, 1});
        
%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo_replace_subsys(test, testparams)
% Test that subsystem output labels and position are preserved when 
% ReplaceSubsystem is false

   model    = 'msiglabel_out';
   subModel = 'mymdl';
   needCleanup = 0;
   try    
     [startdir, testdir, test, hadErr] = rtwtestsetup(test);
     if hadErr, i_jump; end;
     needCleanup = 1;
     
     load_system(model);
     
     set_param(model,'SignalResolutionControl','UseLocalSettings');
    
     ssBlk = [model, '/Unit3'];
     
     pos = get_param (ssBlk, 'Position');
     
     [success, mdlRefBlk] = Simulink.SubSystem.convertToModelReference(ssBlk, subModel, ...
                                           'ReplaceSubsystem', false);
     
     mdlRefBlkPHs = get_param(mdlRefBlk, 'PortHandles');
     
     name = get_param(mdlRefBlkPHs.Outport(1), 'Name');
     
     actPos = get_param(mdlRefBlk,'Position');
     ssW = pos(3) - pos(1);
     ssH = pos(4) - pos(2);
     
     mdlRefW = actPos(3) - actPos(1);
     mdlRefH = actPos(4) - actPos(2);
     
     test = qeverify(test, {success, true}, {name, 'c'}, ...
                     {mdlRefW, ssW}, {mdlRefH, ssH});
     
   catch
     disp(['*>Test failed due to error:', lasterr]);
     test = qeverify(test, {1,0,'==','testdesc',['Failed '...
                         'when converting a model to use model reference']});
   end
   rtwtestclosemodel(model);
   if needCleanup
     rtwtestcleanup(startdir, testdir);
   end
   
%endfunction


%------------------------------------------------------------------------------
function test = lvlTwo_alias_dtype(test, testparams)

% This model is used for right click s-function code generation
% See test/toolbox/simulink/engine/tslbus_rtw.m

   model    = 'mslbus_28';
   subModel = 'mynewmdl';
   needCleanup = 0;
   try    
     [startdir, testdir, test, hadErr] = rtwtestsetup(test);
     if hadErr, i_jump; end;
     needCleanup = 1;
     
     load_system(model);
     
     ssBlk = [model, '/mslbus28_ss'];
     
     [success, mdlRefBlk] = Simulink.SubSystem.convertToModelReference(ssBlk, subModel, ...
                                           'BuildTarget', 'Sim', ...
                                           'ReplaceSubsystem', true);
     test = qeverify(test, {success, true});
     
     mslbus_28([],[],[],'compile');
     mdlrefPorts = get_param(mdlRefBlk,'PortHandles');
     cinfo = sl('slport_get_compiled_info', mdlrefPorts.Outport);
     mslbus_28([],[],[],'term');
     
     test = qeverify(test, {cinfo.DataType, 'salade_t'}, ...
                     {cinfo.AliasThruDataType, 'sfix32_En10'});
     
     
     open_system('mynewmdl');
     mynewmdl([],[],[],'compile');
     in1 = [subModel,'/pommes'];
     in1Ports = get_param(in1,'PortHandles');
     in1Info = sl('slport_get_compiled_info', in1Ports.Outport);
     
     in2 = [subModel,'/frites'];
     in2Ports  = get_param(in2,'PortHandles');
     in2Info = sl('slport_get_compiled_info', in2Ports.Outport);
     
     out1 = [subModel,'/salade'];
     out1Ports = get_param(out1,'PortHandles');
     out1Info = sl('slport_get_compiled_info', out1Ports.Inport);
     mynewmdl([],[],[],'term');
     
     test = qeverify(test, ...
                     {in1Info.DataType, 'pommes_t'}, ...
                     {in1Info.AliasThruDataType, 'sfix16_En4'}, ...
                     {in2Info.DataType, 'frites_t'}, ...
                     {in2Info.AliasThruDataType, 'sfix16_Sp01'}, ...
                     {out1Info.DataType, 'salade_t'}, ...
                     {out1Info.AliasThruDataType, 'sfix32_En10'});
     
   catch
     disp(['*>Test failed due to error:', lasterr]);
     test = qeverify(test, {1,0,'==','testdesc',['Failed '...
                         'when converting a model to use model reference']});
   end
   rtwtestclosemodel('mynewmdl');
   rtwtestclosemodel(model);
   if needCleanup
     rtwtestcleanup(startdir, testdir);
   end
   
%endfunction

%------------------------------------------------------------------------------
function test= lvlTwo_rtw_storage_class(test, testparams)
% Test storage class and fixedpoint

  evalin('base', 'mySigA=Simulink.Signal;');
  evalin('base', 'mySigA.RTWInfo.StorageClass = ''Exported'';');
  evalin('base', 'mySigB=Simulink.Signal;');
  evalin('base', 'mySigB.RTWInfo.StorageClass = ''ImportedExtern'';');    
  evalin('base', 'mySigC=Simulink.Signal;');
  evalin('base', 'mySigC.RTWInfo.StorageClass = ''Exported'';');
  
  model = 'mslbus_05';
  load_system(model);
  
  needCleanup = 0;
  try    
      [startdir, testdir, test, hadErr] = rtwtestsetup(test);
      if hadErr, i_jump; end;
      needCleanup = 1;
      lasterr('');
      ssBlk =  'mslbus_05/Subsystem';
      Simulink.SubSystem.convertToModelReference(ssBlk, 'my_new_model', 'ReplaceSubsystem', true, ...
                     'BuildTarget', 'Sim');
      
      slbuild(model);
      
      opts.targetType = 'ModelRefRTWERT';
      opts.include    = {'mySigA','mySigC'};
      % Signal resolution control is on.
      % mySigB is considered as a label.
      opts.exclude    = {'mySigB'};
      opts.filetypes  = {'my_new_model.c'};
      test = rtwgencodecheck(test, 'my_new_model', [], opts);
      
  catch
      disp(['*>Test failed due to error:', lasterr]);
      test = qeverify(test, {1,0,'==','testdesc',['Failed '...
                          'when converting a model to use model reference']});
  end
  rtwtestclosemodel(model);
  rtwtestclosemodel('my_new_model');
  if needCleanup
      rtwtestcleanup(startdir, testdir);
  end 
  evalin('base', 'clear mySigA mySigB mySigC;');
  
%endfunction lvlOneA5

%------------------------------------------------------------------------------
function test = lvlTwo_color(test, testparams)
% Make sure Foreground and background colors are properly set
%In reference to geck 482613

    sw1= qeSuppressWarningID('Simulink:utility:slUtilityCompBusCannotUseSignalNameForBusName'); %#ok
    
    tmpDir = qeWorkingDir(); %#ok
    
    model  = 'mModelRefBlockColor_g482613';
    mdlRef = 'my_new_model';
    ssBlk  = [model, '/Bus Counter'];
    open_system(model);    
    c1 = onCleanup(@() rtwtestclosemodel(model, true));
    
    set_param(model, 'SampleTimeColors', 'on');
    sim(model);
    
    % It seems that when the model is compiled, HiliteAncestors will be reset
    set_param(ssBlk, 'HiliteAncestors','on');
    set_param(model,'SignalResolutionControl','UseLocalSettings');    
    
    [~, mdlBlkH] = Simulink.SubSystem.convertToModelReference(ssBlk, mdlRef, 'ReplaceSubsystem', true);

    % Verify model reference block's foreground and background colors after conversion
    qeverify(test, ...
             {get_param(mdlBlkH, 'ForeGroundColor'), 'black','=='}, ...
             {get_param(mdlBlkH, 'BackGroundColor'), 'white','=='});
        
    % Turn the sample time color off then verify model reference block's foreground and background colors
    set_param(model, 'SampleTimeColors', 'off');    
    qeverify(test, ...
             {get_param(mdlBlkH, 'ForeGroundColor'), 'red','=='}, ...
             {get_param(mdlBlkH, 'BackGroundColor'), 'white','=='});


%endfunction


%------------------------------------------------------------------------------
function test = lvlTwo_301825(test, testparams)
% Negative test SubsystemHasLinkToADirtyLibrary
tmpDir = qeWorkingDir();%#ok

model = 'm301825';
newmodel = 'my_new_model';
lib   = 'mlib_301825';

expErrId = [msgIdPref_l, 'SubsystemHasLinkToADirtyLibrary'];

qe_copy(fullfile(tmpDir.origDir,[model, '.mdl']),...
    fullfile(tmpDir.dir,[model, '.mdl']));

qe_copy(fullfile(tmpDir.origDir,[lib, '.mdl']),...
    fullfile(tmpDir.dir,[lib, '.mdl']));

load_system(model);
load_system(lib);

oc1 = onCleanup(@() rtwtestclosemodel(model));
oc2 = onCleanup(@() rtwtestclosemodel(newmodel));
oc3 = onCleanup(@() rtwtestclosemodel(lib));

% modify the library
set_param(lib, 'Lock','off');
set_param('mlib_301825/cfg_ss', 'MemberBlocks', 'fule_rate');

delete_block('m301825/fule');
add_block('mlib_301825/fule_rate','m301825/fule');
test = qeVerifyError(expErrId, @() Simulink.SubSystem.convertToModelReference('m301825/fule', newmodel));

% make sure saving the library fixes the problem
save_system(lib);
Simulink.SubSystem.convertToModelReference('m301825/fule', newmodel);
  
%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo_g624376(test, testparams)
% Make sure we can convert a subsystem in a model that has used add_param.
% This was found by a physmod block.
  
  model = 'maddparam';
  
  obj = qeWorkingDir; %#ok
  load_system(model);
  
  paramVal1 = get_param(model, 'NewParam1');
  paramVal2 = get_param(model, 'NewParam2');
  
  qeverify({paramVal1, 'value1', '==', 'testdesc', 'Added parameter not correct'}, ...
           {paramVal2, 'value2', '==', 'testdesc', 'Added parameter not correct'})
  
  Simulink.SubSystem.convertToModelReference('maddparam/Subsystem','tmpMdl');
  
  newParamVal1 = get_param('tmpMdl', 'NewParam1');
  newParamVal2 = get_param('tmpMdl', 'NewParam2');
  
  qeverify({newParamVal1, 'value1', '==', 'testdesc', 'Added parameter not correct'}, ...
           {newParamVal2, 'value2', '==', 'testdesc', 'Added parameter not correct'})
  
  
  rtwtestclosemodel('tmpMdl');
  rtwtestclosemodel('untitled');
  rtwtestclosemodel(model);
  
%endfunction

%------------------------------------------------------------------------------
function test = lvlTwoVariantNotSupported(test, testparams)
  tmpDir = qeWorkingDir(); %#ok
  
  model = 'mvariantsubsys';
  load_system(model)
  c1 = onCleanup(@() rtwtestclosemodel(model));
  
  ssBlk = [model '/Variant'];

  expId = 'Simulink:modelReference:invalidSSTypeVariant';
  
  qeVerifyError(expId, @() Simulink.SubSystem.convertToModelReference(ssBlk, 'mynewmodel'));
  
%endfunction


function ret = get_bus_block_info_l(model)
  cmd = [model, '([],[],[],''compile'')'];
  evalc(cmd);
  
  BC  = 'mstruct_bus1/BC';
  BC1 = 'mstruct_bus1/BC1';
  BS  = 'mstruct_bus1/BS';
  BS1 = 'mstruct_bus1/BS1';
  
  BCPHs  = get_param(BC, 'PortHandles');
  BC1PHs = get_param(BC1, 'PortHandles');
  BSPHs  = get_param(BS, 'PortHandles');
  BS1PHs = get_param(BS1, 'PortHandles');
  
  ret.BC  = get_param(BCPHs.Outport, 'IsCompiledStructureBus');
  ret.BC1 = get_param(BC1PHs.Outport, 'IsCompiledStructureBus');
  ret.BS  = get_param(BSPHs.Inport, 'IsCompiledStructureBus');
  ret.BS1 = get_param(BS1PHs.Inport, 'IsCompiledStructureBus');
     
     
  cmd = [model, '([],[],[],''term'')'];
  evalc(cmd);

  
% Function: ===================================================================
% This is a helper function for lvlTwo_structure_bus.
%
function ret = get_sub_model_bus_l(subModel)
  
load_system(subModel);
cmd = [subModel, '([],[],[],''compile'')'];
evalc(cmd);

inBlk  = [subModel, '/In1'];
outBlk = [subModel, '/Out1'];


ret.inUseBus = get_param(inBlk,'UseBusObject');
ret.inOutputAsStruct = get_param(inBlk,'BusOutputAsStruct');

ret.outUseBus = get_param(outBlk,'UseBusObject');
ret.outOutputAsStruct = get_param(outBlk,'BusOutputAsStruct');
     
cmd = [subModel, '([],[],[],''term'')'];
evalc(cmd);


%------------------------------------------------------------------------------
% Helper function for testing Force option

% **************************************************
% *                                                *
% *      This test is full.  Please do not add     *
% *        additional testpoints to the file.      *
% *                                                *
% **************************************************

% LocalWords:  mtiming btiming mdatastore glb bdatastore mclosed bclosed mreset
% LocalWords:  breset virtuality mstruct msiglabel mymdl tslbus mslbus mynewmdl
% LocalWords:  slport salade pommes frites fixedpoint RTWERT lvl mdlref
% LocalWords:  Foregroundcolor ADirty mlib fule tmp physmod maddparam
% LocalWords:  mvariantsubsys mynewmodel
