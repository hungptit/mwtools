function [test] = tsubsys2model3(point)
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
function test = lvlTwo5(test, testparams)
% Test for fixed point data type for non-bus signals
  
  mdl    = 'msigspec_bus';
  subsys = 'msigspec_bus/Subsystem2/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bsigspec_bus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo6(test, testparams)
% Test for fixed point data type for bus signals
  
  mdl    = 'msigspec_bus';
  subsys = 'msigspec_bus/Subsystem2';
  mdlRef = 'myNewModel';
  baseFileName = 'bsigspec_bus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef);

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo7(test, testparams)
% Test for single instanced model
  
  mdl    = 'msingleinst';
  subsys = 'msingleinst/Subsystem2';
  mdlRef = 'myNewModel';
  baseFileName = 'bsigspec_bus';

  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef, ...
                      'BuildTarget', 'Sim');

%endfunction

%------------------------------------------------------------------------------
function test = lvlTwo8(test, testparams)
% Test for fixed point data type in the bus objects
  mdl    = 'msigspec_bus';
  subsys = 'msigspec_bus/Subsystem2';
  mdlRef = 'myNewModel';
  

  lasterr('');
  needCleanup = 0;

  oldListOfOpenModels = find_system('flat');
  
  try    
    [startdir, testdir, test, hadErr] = rtwtestsetup(test);
    if hadErr, i_jump; end;
    needCleanup = 1;
    
    load_system(mdl);

    set_param(mdl,'SignalResolutionControl','UseLocalSettings');
     
    [s, mdlRefH] = Simulink.SubSystem.convertToModelReference(subsys, mdlRef, ...
                                    'ReplaceSubsystem', true,   ...
                                    'BusSaveFormat', 'Cell');
    
    blkType = get_param(mdlRefH,'BlockType');
    
    test = qeverify(test, {s, true}, {blkType, 'ModelReference'});
    
    % Clear the base
    test_cleanup(testparams);
    
    % Load bus object using the created file
    which myNewModel_bus;
    cellOut = myNewModel_bus;
    
    dtypeExists = evalin('base', 'exist(''slnum_sfix16_En10'')');

    test = qeverify(test, {length(cellOut), 1}, {dtypeExists, 1});
    
  catch
    disp(['*>Test failed due to error:', lasterr]);
    test = qeverify(test, {1,0,'==','testdesc',['Failed '...
                        'when converting a model to using model reference']});
  end
  
  newListOfOpenModels = find_system('flat');
  modelsToClose = setdiff(newListOfOpenModels, oldListOfOpenModels);
  
  for mdlIdx = 1:length(modelsToClose)
    rtwtestclosemodel(modelsToClose{mdlIdx});
  end
  
  if needCleanup
    rtwtestcleanup(startdir, testdir);
  end
%endfunction


%------------------------------------------------------------------------------
function test = lvlTwo9(test, testparams)
% Test for artificial alg loop and combine output and update (See 197267)
% with an ERT model
  
  mdl    = 'martalg1';
  subsys = 'martalg1/Subsystem';

  [mdlRefCmbOutputUpdate, mdlRefMinAlgLoop] = AlgLoopCombOut(mdl, subsys);
  test = qeverify(test, {mdlRefCmbOutputUpdate, 'off'}, ...
                    {mdlRefMinAlgLoop, 'on'});
 %endfunction
                
%------------------------------------------------------------------------------

function test = lvlTwo10(test, testparams)
% Test for artificial alg loop and combine output and update
  
  mdl    = 'martalg1';
  subsys = 'martalg1/Subsystem';
  mdlRef = 'myNewModel';
  baseFileName = 'bartalg1';
  test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef, ...
                      'BuildTarget', 'Sim');
  
%endfunction

%------------------------------------------------------------------------------

function test = lvlTwo11(test, testparams)
% Test for artificial alg loop and combine output and update (See 197267)
% with a GRT model
    
  mdl    = 'martalg2';
  subsys = 'martalg2/Subsystem';
  
  [mdlRefCmbOutputUpdate, mdlRefMinAlgLoop] = AlgLoopCombOut(mdl, subsys);
  test = qeverify(test, {mdlRefCmbOutputUpdate, 'on'}, ...
                    {mdlRefMinAlgLoop, 'on'});
    
 %endfunction   
    
%------------------------------------------------------------------------------
function [mdlRefCmbOutputUpdate, mdlRefMinAlgLoop] = AlgLoopCombOut(mdl, subsys)
  %Helper function which converts subsystem to referenced model and returns
  %the values of CombineOutputUpdateFcns and
  %ModelReferenceMinAlgLoopOccurences

  tmpDir = qeWorkingDir();%#ok
  mdlRef = 'myNewModel';
  
  load_system(mdl);
  oc1 = onCleanup(@() rtwtestclosemodel(mdl));
  oc2 = onCleanup(@() rtwtestclosemodel(mdlRef));
  set_param(mdl,'SignalResolutionControl','UseLocalSettings');
   
  set_param(mdl,'CombineOutputUpdateFcns', 'on')
  set_param(subsys, 'MinAlgLoopOccurrences', 'on');
  set_param(mdl, 'ModelReferenceMinAlgLoopOccurrences', 'off');
  
  load_system(mdl);
    
  [s, mdlRefH] = Simulink.SubSystem.convertToModelReference(subsys, mdlRef);
    
  blkType = get_param(mdlRefH, 'BlockType');
    
  qeverify({s, true}, {blkType, 'ModelReference'});
    
  load_system(mdlRef);

    
  mdlRefCmbOutputUpdate = get_param(mdlRef,'CombineOutputUpdateFcns');
  mdlRefMinAlgLoop = get_param(mdlRef, 'ModelReferenceMinAlgLoopOccurrences');

 %end function
  
%------------------------------------------------------------------------------


% **************************************************
% *                                                *
% *      This test is full.  Please do not add     *
% *        additional testpoints to the file.      *
% *                                                *
% **************************************************

