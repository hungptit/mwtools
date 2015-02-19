function [test] = tsubsys2model_ui(point)
% Test suite for automatic conversion tool for model reference
%

% Copyright 2003-2011 The MathWorks, Inc.
%
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.
testharness;                     % Run the tests and report results
  
end

function [test,testparams]=setup(test,testparams)
%SETUP Create variables, figures, pcode, etc., for use in all testpoints.
% Store these in fields of the testparams structure.
% Even if you do not intend to use testparams, create the
% variable anyway to avoid bogus warnings in function calls.
    
  testparams = qe_model_reference(testparams,'setup');
  testparams.convFeature = slfeature('ModelreferenceconversionTesting');
  slfeature('ModelreferenceconversionTesting',0);

end

function test = cleanup(test,testparams)
%CLEANUP Close figure windows created by setup.
  testparams = qe_model_reference(testparams,'cleanup');
  slfeature('ModelreferenceconversionTesting', testparams.convFeature);
end


%------------------------------------------------------------------------------
function test = lvlTwo_menu_item(test, testparams)
% Test for create model reference menu item.
% The test converts a subsystem to a model by right clicking on the context
% menu and it ensures that the model was created.

  modelName = 'mmdlref_conversion_menu_item';
  open_system(modelName)
  
  obj = qeWorkingDir; %#ok

  subSystemName = 'SubSystemTestCounter';
  set_param([modelName '/' subSystemName], 'Selected', 'on');
  
  
  block = [modelName '/' subSystemName];
  pos = get_param(block, 'Position');
  center = pos(1:2) + (pos(3:4) - pos(1:2))/2;  
  editorTester = hgetEditorTester(modelName);
  editorTester.executeContextMenuSchemaAtScenePos('Simulink:ConvertSubsystemToReferencedModel', ...
                                                  center(1), center(2));

  % Make sure the model was created by checking if the exist function returns 4
  expVal = 4; 
  test = qeverify(test,...
                  {exist(subSystemName, 'file'), expVal, '==','testdesc',...
                   ['The conversion form menu item failed to create the model:' subSystemName] });     
  rtwtestclosemodel(subSystemName);
  close_system(modelName,0)
end

%------------------------------------------------------------------------------
function test = lvlTwo_menu_item_edit(test, testparams)
% Test for create model reference menu item.
% The test converts a subsystem to a model by clinking on the edit menu item
%  and it ensures that the model was created.

  obj = qeWorkingDir; %#ok
  modelName = 'mmdlref_conversion_menu_item';
  open_system(modelName)
  
  subModelName  = 'SubSystemTestCounter_Edit';
  subSystemName = 'SubSystemTestCounter';
  blockHandle =    get_param([modelName '/' subSystemName], 'Handle');
  set_param(blockHandle, 'Name',  subModelName);
  

  set_param(blockHandle, 'Selected', 'on');
  % Obtain the menu item
  studio = hgetUEStudio(modelName);
  dm = studio.getDigManager; 
  dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
  
  expVal = 4; % Make sure the model was created by checking if the exist function returns 4
  test = qeverify(test,...
                  {exist(subModelName, 'file'), expVal, '==','testdesc',...
                   ['The conversion form menu item failed to create the model:' subSystemName] });     
  
  rtwtestclosemodel(subModelName);
  rtwtestclosemodel(modelName);

end

 %------------------------------------------------------------------------------
function test = lvlTwo_menu_item_force_ui(test, testparams)

  %In reference to geck 482613
  sw1= qeSuppressWarningID('Simulink:convertToModelReference:InlineParameter'); %#ok


% Test for create model reference menu item.
% The test converts a subsystem to a model by right clicking on the context
% menu and it ensures that the model was created even though a error occurred
% during the conversion. This done by forcing the conversion and checking for 
% warnings. 
%    The test mimics the behavior: 
%      1. Right-click on the subsystem   
%      2. Select "Convert to Model Reference"
%      3.  Question Dialog comes up
%          "Error occurred during conversion" with buttons: "Continue" and "Stop"
%      4. Continue button is selected
%      5. The conversion continues and we check for warning messages
    
  % Assign the Value for the question dialog: Force
  evalin('base', 'MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting = ''Continue conversion''; ');
  cmd = 'evalin(''base'', ''clear MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting'');';
  c = onCleanup(@() eval(cmd));
  
  obj = qeWorkingDir; %#ok
  modelName = 'mmdlref_conversion_menu_item';
  open_system(modelName)

  set_param(modelName,'InlineParams','off')
  subSystemName = 'SubSystemTestCounter';
  subModelName  = 'SubSystemTestCounter1';
  blockHandle =    get_param([modelName '/' subSystemName], 'Handle');
  set_param(blockHandle, 'Name',  subModelName);
  set_param(blockHandle, 'Selected', 'on');

  
  studio = hgetUEStudio(modelName);
  dm = studio.getDigManager; 
  dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
  
  % Make sure the model was created by checking if the exist function returns 4
  expVal = 4; 
  actWarn = lastwarn;
  test = qeverify(test,...
                  {exist(subModelName, 'file'), expVal, '==','testdesc',...
                   ['The conversion from the menu item failed to create the model:' subSystemName] });     
  
  if(qe_isenglish)
      expWarnStr = '''Inline Parameters'' option in the optimization';
      test = qeverify(test,...
                      {actWarn ,expWarnStr , '==','ignore','fullstring'});     
  else
      test = qeverify(test, {isempty(lastwarn), false});
  end
  
  rtwtestclosemodel(subModelName);
  rtwtestclosemodel(modelName);
 
end

%------------------------------------------------------------------------------
function test = lvlTwo_menu_item_stop_ui(test, testparams)
% Test for create model reference menu item.
% The test converts a subsystem to a model by right clicking on the context
% menu and it ensures that the model was created even though a error occurred
% during the conversion. This done by forcing the conversion and checking for 
% warnings. 
%    The test mimics the behavior: 
%      1. Right-click on the subsystem   
%      2. Select "Convert to Model Reference"
%      3.  Question Dialog comes up
%          "Error occurred during conversion" with buttons: "Force" and "Stop"
%      4. Stop button is selected
%      5. The conversion will stop and we will check for error messages

  % Assign the Value for the question dialog: Stop
  evalin('base', 'MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting = ''Stop''; ')
  cmd = 'evalin(''base'', ''clear MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting;'');';
  c = onCleanup(@() eval(cmd));
  
  
  modelName = 'mmdlref_conversion_menu_item';
  open_system(modelName);
  c1 = onCleanup(@() rtwtestclosemodel(modelName));
  
  obj = qeWorkingDir; %#ok
  
  set_param(modelName,'InlineParams','off')
  subSystemName = 'SubSystemTestCounter';

  set_param([modelName '/' subSystemName], 'Selected', 'on');
  
  studio = hgetUEStudio(modelName);
  dm = studio.getDigManager;
  dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
  
  ErrorId = 'Simulink:modelReference:convertToModelReference_InlineParamsOff';
  actError = lasterror; %#ok<LERR>
  test = qeverify(test, {actError.identifier, ErrorId});   
end
 
%------------------------------------------------------------------------------
function test = lvlTwo_menu_item_neg(test, testparams)
% Test for create model reference menu item.
% Negative tests to ensure that we error out in case the name of the 
% subsystem/model is not a valid model name

    %In reference to geck 482613
    sw1= qeSuppressWarningID('Simulink:modelReference:slss2mdlInvalidModelName'); %#ok


    % Disable the Error dialoglvlTwo_menu_item_neg_hard_errors
    evalin('base', 'MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting = ''Stop''; ')
    cmd = ' evalin(''base'', ''clear MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting;'');';
    c = onCleanup(@() eval(cmd));
    
    longName = 'f';    
    for k = 1:50
        longName = [longName num2str(k)]; %#ok<AGROW>
    end
    
    InvalidNames = {'Space in the name',...
                    longName};
    obj = qeWorkingDir; %#ok

    modelName = 'mmdlref_conversion_menu_item';
    open_system(modelName)
    subSystemName = 'SubSystemTestCounter';
    blockHandle =    get_param([modelName '/' subSystemName], 'Handle');
    for i = 1 : length(InvalidNames)
        set_param(blockHandle, 'Name', InvalidNames{i});
        set_param(blockHandle, 'Selected', 'on');
        studio = hgetUEStudio(modelName);
        dm = studio.getDigManager;
        dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
        
        actWarn = lastwarn;
        if(qe_isenglish)
            expWarnStr = 'The name of the Subsystem is not a valid model name';
            test = qeverify(test,...
                            {actWarn ,expWarnStr , '==','ignore','fullstring'});     
        else
            test = qeverify(test, {isempty(lastwarn), false});
        end
    end
    close_system(modelName,0)

end
 
 %------------------------------------------------------------------------------
function test = lvlTwo_menu_item_neg_hard_errors(test, testparams)
% Test for create model reference menu item.
% Negative tests to ensure that we error out in case there 
% is an hard error during the conversion.
%    The test mimics the behavior: 
%      1. Right-click on the subsystem   
%      2. Select "Convert to Model Reference"
%      3.  Question Dialog comes up
%          "Error occurred during conversion" with buttons: "Continue" and "Stop"
%      4. Continue button is selected
%      5. An error occurred and we check for this error

  function locCleanup()
      evalin('base', 'clear MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting');
      evalin('base', 'clear ans')
      evalin('base', 'clear mixedtsbus')
      evalin('base', 'clear sametsbus')
  end


  %In reference to geck 482613
  sw1= qeSuppressWarningID('Simulink:modelReference:convertToModelReference_InvalidUpStreamSampleTimeWarn'); %#ok

  
  % Assign the Value for the question dialog: Force
  evalin('base', 'MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting = ''Continue conversion''; ');
  c = onCleanup(@() locCleanup);

  modelName = 'mmixedts';
  open_system(modelName)
  subModelName  = 'SubSystemTestHardError';
  obj = qeWorkingDir; %#ok
  
  set_param(modelName,'SignalResolutionControl','UseLocalSettings')
  subSystemName = 'SS';
  
  blockHandle =    get_param([modelName '/' subSystemName], 'Handle');
  set_param(blockHandle, 'Name',  subModelName);
  set_param(blockHandle, 'Selected', 'on');
  
  studio = hgetUEStudio(modelName);
  dm = studio.getDigManager;
  dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
  
  
  ErrorId = 'Simulink:modelReference:convertToModelReference_BusWithMixedSampleTimes';
  actError = lasterror; %#ok<LERR>
  test = qeverify(test,...
                  {actError.identifier, ErrorId}); 

  close_system(modelName,0)
end
 
 
function test = lvlTwo_menu_item_neg_hard_errors1(test, testparams)
% Test for create model reference menu item.
% Negative tests to ensure that we error out for permission denied error messages 

    if ~isunix
        disp('tmuxreplace: lvlTwo_neg6 is only running on unix platforms');
        return;
    end

    modelName = 'mmdlref_conversion_menu_item';
    open_system(modelName);

    % Assign the Value for the question dialog: Force
    evalin('base', 'MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting = ''Continue conversion''; ');
    cmd = 'evalin(''base'', ''clear MathWorks_ConvertSSToMdlRef_MenuItemResponseValue_ForTesting'');';
    c = onCleanup(@() eval(cmd));
    
    rodir=tempname;
    mkdir(rodir);
    fileattrib(rodir,'-w','u');
    disp([rodir, ' is created.']);
    curdir = pwd;
    cd(rodir)
    
    subSystemName = 'SubSystemTestCounter';
    set_param([modelName '/' subSystemName], 'Selected', 'on');
    studio = hgetUEStudio(modelName);
    dm = studio.getDigManager;
    dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
       
    isin = qeverifyslmessageid('Simulink:LoadSave:FileWriteError', 'error','only');
    test = qeverify(test, { isin, true });
    cd(curdir)
    qe_deletedir(rodir)
    disp([rodir, ' has been removed']);
   
    close_system(modelName,0)
end


function test = lvlTwo_menu_item_convert_mdlref_neg(test, testparams)
% Test for 'Convert to Model Block' menu item
% Negative tests to ensure that for the cases below, the menu item
% is disabled:
%       1. Stateflow chart
%       2. Truth table
%       3. EML block
% The test mimics the behaviour:
%      1. For each subsystem in model
%      2. Invoke its context menu  
%      3. Get the menu item "Convert to Model Block"
%      4. If the item is enabled error out
    model = 'msubsys_noconvert'; 
    
    test = locCheckForMenuItem(test, model, false);
end


function test = lvlTwo_menu_item_convert_mdlref_pos(test, testparams)
% Test for 'Convert to Model Block' menu item
% Positive tests to ensure that for any type of standard subsystem, the
% menu item is enabled.
    model = 'msubsys_convert'; 
    
    test = locCheckForMenuItem(test, model, true);
end

function test = locCheckForMenuItem(test, model, expEnabledStatus)
    open_system(model);
    set_param(model, 'StrictBusMsg', 'ErrorLevel1');
    set_param(model,'SignalResolutionControl','UseLocalSettings');
    
    subsysBlks = find_system(model,'SearchDepth',1,'BlockType','SubSystem');
    numSubSys  = length(subsysBlks);
    
    for idx = 1:numSubSys
        ssBlk = subsysBlks{idx};
        ssBlkName = get_param(ssBlk,'Name');
        
        % set the expState to empty, and hold the 'expEnabledStatus in a
        % temporary so that it is reset after qeverify.
        expState = '';
        tmpState = expEnabledStatus;
        
        %if block is a variant subsystem,
        %'ConvertSubsystemToReferenceModel' will be disabled
        if (strcmp(get_param(ssBlk,'BlockType'),'SubSystem'))
            if (strcmp(get_param(ssBlk,'Variant'),'on'))
                expState = 'Disabled';
            end
        end
                            
        pos = get_param(ssBlk, 'Position');
        center = pos(1:2) + (pos(3:4) - pos(1:2))/2;  
        editorTester = hgetEditorTester(model);
        cMenuItem = editorTester.getContextMenuSchemaAtScenePos('Simulink:ConvertSubsystemToReferencedModel', center(1), center(2));
        
        if ~isempty(cMenuItem)
            if (strcmp(expState,''))
                if(expEnabledStatus && strcmp(expState,''))
                    expState = 'Enabled';
                else
                    expState = 'Hidden';
                end
            end
            
            test = qeverify(test, {cMenuItem.state, expState, '==', 'testdesc', ...
                                ['Block',...
                                ' ''', ssBlkName, ''' should not allow conversion to model block.']});
        end
        
        
        % revert the expEnabledStatus
        expEnabledStatus = tmpState;
    end
    
    close_system(model, 0);
end
