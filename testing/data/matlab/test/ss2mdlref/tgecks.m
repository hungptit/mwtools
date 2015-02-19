function [test] = tgecks(point)
% Test suite for automatic conversion tool for model reference
%
%
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.

% Copyright 2005-2012 The MathWorks, Inc.
  testharness; % Run the tests and report results
end 
  
%% ------------------------------------------------------------------------
function [test,testparams]=setup(test,testparams)
%SETUP Create variables, figures, pcode, etc., for use in all testpoints.
% Store these in fields of the testparams structure.
% Even if you do not intend to use testparams, create the
% variable anyway to avoid bogus warnings in function calls.
  testparams = qe_model_reference(testparams,'setup');
  testparams.convFeature = slfeature('ModelreferenceconversionTesting');
  slfeature('ModelreferenceconversionTesting',0);
end 
  
%% ------------------------------------------------------------------------
function test = cleanup(test,testparams)
%CLEANUP Close figure windows created by setup.
  testparams = qe_model_reference(testparams,'cleanup');
  slfeature('ModelreferenceconversionTesting', testparams.convFeature);
end 
  
%% -----------------------------------------------------------------------  
function [test,testparams] = unitsetup(test, testparams)
    testparams = slrtw_remove_drool(testparams,'setup');
end 


%% -----------------------------------------------------------------------
function test = unitcleanup(test, testparams)
    slrtw_remove_drool(testparams,'cleanup');
end 

%% ------------------------------------------------------------------------
function test = lvlTwo_zoom(test, testparams)
% Test for geck 390267 
    obj = qeWorkingDir(); %#ok
    
    model   = 'mZoom';
    subsys  = [model, '/Subsystem'];
    subsys2 = [subsys, '/Atomic Subsystem']; 
    subsys3 = [subsys2, '/Subsystem'];
    load_system(model);
    c1 = onCleanup(@() rtwtestclosemodel(model));

    % Get the original zoom factors
    modelZoom   = get_param(model,   'ZoomFactor');
    subsysZoom  = get_param(subsys,  'ZoomFactor');
    subsys2Zoom = get_param(subsys2, 'ZoomFactor');
    subsys3Zoom = get_param(subsys3, 'ZoomFactor');
    
    zooms = {modelZoom, subsysZoom, subsys2Zoom, subsys3Zoom};
    
    
    % Make sure all the zooms are different
    test = qeverify(test, {length(unique(zooms)), length(zooms), '==', 'testdesc', 'Initial ZoomFactors must all be unique'});
    
    % Make sure none of the zooms are '100'
    test = qeverify(test, {ismember('100', zooms), false, '==', 'testdesc', 'None of the initial ZoomFactors should be ''100'''});
        
    % End the test if the model is not setup correctly
    if(~isempty(test.failures))
        rtwtestclosemodel(model);
        return;
    end % if
                      
    % Do the conversion
    newModel = 'mynewmdl';
    c2 = onCleanup(@() rtwtestclosemodel(newModel));
    success  = Simulink.SubSystem.convertToModelReference(subsys2, newModel, 'ReplaceSubsystem', false);

    % Make sure the conversion succeeded
    test = qeverify(test, {success, true, '=='});

    % Get the zoom factor for the new models
    topModel       = 'untitled';
    c3 = onCleanup(@() rtwtestclosemodel(topModel));
    newMdlRefZoom  = get_param(newModel, 'ZoomFactor');
    newSubsys3Zoom = get_param(strrep(subsys3, subsys2, newModel), 'ZoomFactor');
    newTopMdlZoom  = get_param(topModel, 'ZoomFactor');
    
    % Test the new zoom factors
    test = qeverify(test, {newTopMdlZoom,  subsysZoom,  '=='}, ...
                          {newMdlRefZoom,  subsys2Zoom, '=='},...
                          {newSubsys3Zoom, subsys3Zoom, '=='});    
end



%% ------------------------------------------------------------------------
function test = lvlTwo_unconnected(test, testparams)
% Test for geck 433969
    obj = qeWorkingDir(); %#ok
    
    model   = 'mUnconnected';
    subsys  = [model, '/Subsystem'];
    load_system(model);

    % Do the conversion
    newModel = 'mynewmdl';
    success  = Simulink.SubSystem.convertToModelReference(subsys, newModel, 'ReplaceSubsystem', false);

    % Make sure the conversion succeeded
    test = qeverify(test, {success, true, '=='});
    
    % Close out the models
    rtwtestclosemodel(model);
    rtwtestclosemodel(newModel);
    rtwtestclosemodel('untitled');
end 



%% ------------------------------------------------------------------------
% If the subsystem has mask with a workspace, we don't modify any of the
% inports or outports of the system, we just copy them over.  Make sure
% that the conversion works without error.
function test = lvlTwoMasked(test, testparams)
    obj = qeWorkingDir(); %#ok
    sw = qeSuppressWarningID('Simulink:convertToModelReference:InvalidSubsystemUnderMask'); %#ok
    
    model   = 'mMaskedSubsystem';
    subsys  = [model, '/Subsystem'];
    load_system(model);

    % Do the conversion
    newModel = 'mynewmdl';
    success  = Simulink.SubSystem.convertToModelReference(subsys, newModel,...
                                                      'Force',true);

    % Make sure the conversion succeeded
    test = qeverify(test, {success, true, '=='});
    
    % Close out the models
    rtwtestclosemodel(model);
    rtwtestclosemodel(newModel);
    rtwtestclosemodel('untitled');
end



%% ------------------------------------------------------------------------
% Make sure we get the nag controller with appropriate messages.
% See g639730
function test = lvlTwo_nag_ctlr(test, testparams)
    obj = qeWorkingDir(); %#ok

    model  = 'mg639730';
    subsys = [model, '/OFP'];
    open_system(model);
    c1 = onCleanup(@() rtwtestclosemodel(model));
    
    
    set_param(subsys, 'Selected', 'on');
    studio = hgetUEStudio(model);
    dm = studio.getDigManager; 
    dm.executeMenuBarSchema('Simulink:ConvertSubsystemToReferencedModel');
    
    nags = slsfnagctlr('GetNags');
    
    % Make sure we see three nags
    qeverify({length(nags), 3, '==', 'testdesc', 'Incorrect number of nags seen'});
    
    qeverify({nags(1).msg.identifier, 'MATLAB:MException:MultipleErrors', '=='});
    qeverify({nags(2).msg.identifier, 'Simulink:Engine:PortDimsMismatch42', '=='});
    qeverify({nags(3).msg.identifier, 'Simulink:Engine:PortDimsMismatch41', '=='});
end


%% ------------------------------------------------------------------------
function test = lvlTwo_invalidValueForSubsystemsForErrorCheckingParameter(test, testparams)
    tmpDir = qeWorkingDir(); %#ok    
    model  = 'mg649109';
    subsys = [model, '/Variant Subsystem/Subsystem1'];
    load_system(model);
    c1 = onCleanup(@() rtwtestclosemodel(model, true));
    
    obj = Simulink.ModelReference.SubsystemConversionCheck({subsys});
    qeVerifyError('Simulink:modelReference:convertToModelReference_SubsystemInInactiveVariant', ...
        @() obj.checkForError);
end

%% ------------------------------------------------------------------------
function test = lvlTwo_checkSubsystemsForErrorCheckingParameter(test, testparams)
    obj = qeWorkingDir(); %#ok

    % Now make sure we only compile the model once if we pass in an array
    % of subsystems to check
    model = 'mMultipleSubsystems';
    c1 = onCleanup(@() rtwtestclosemodel(model));
    c2 = onCleanup(@() evalin('base','clear compCheck;'));
    
    subsystems = {[model '/Subsystem'], [model '/Subsystem1'], ...
        [model '/Subsystem2'], [model '/Subsystem3']};
    
    load_system(model);
    
    % We put a variable in the workspace to check how many times the
    % model is compiled.
    sobj = Simulink.ModelReference.SubsystemConversionCheck(subsystems);
    success = sobj.checkForError();
    compCheck = evalin('base','compCheck');
        
    qeverify({success, true, '==', 'testdesc',...
              'convertToModelReference did not succeed'}, ...
             {compCheck, 1, '==', 'testdesc', ...
              'We should only compile the model once during conversion'});    
end


%% ------------------------------------------------------------------------
% Check for success when a bus has a fixed point data type with quotes
% See g652562
function test = lvlTwo_fixedPtDTInBus(test, testparams)
    obj = qeWorkingDir(); %#ok

    newModel = 'mynewmdl';
    model  = 'mg652562';
    subsys = [model, '/atomicSubsys'];
    load_system(model);
    
    c1 = onCleanup(@() rtwtestclosemodel(model));
    c2 = onCleanup(@() rtwtestclosemodel(newModel));
    c3 = onCleanup(@() rtwtestclosemodel('untitled'));
    
    % We do not need to verify the returned status because if something happened the conversion
    % script will error out and never return anything.
    Simulink.SubSystem.convertToModelReference(subsys, newModel); 
end

%% ------------------------------------------------------------------------
% Check for error on inactive subsystem variant
% See g649109
function test = lvlTwo_inactiveSubsystemVariant(test, testparams)
    tmpDir = qeWorkingDir(); %#ok

    newModel = 'mynewmdl';
    model  = 'mg649109';
    subsys = [model, '/Variant Subsystem/Subsystem1'];
    load_system(model);
    c1 = onCleanup(@() rtwtestclosemodel(model, true));
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_SubsystemInInactiveVariant', ...
                  @() Simulink.SubSystem.convertToModelReference(subsys, newModel));
end

%% ------------------------------------------------------------------------
% Positive test points to check that we could convert and simulate masked subsystems with help dialog or unused masked variables 
% to referenced models. 
function test = lvlTwo_maskedSubsystem_g790093(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    modelName     = 'mMaskedSubsystem_g790093';
    subsysName1   = [modelName, '/Subsystem1/buffermaxBwClusters'];
    subsysName2   = [modelName, '/Subsystem2'];
    modelRefName1 = 'buffermaxBwClusters';
    modelRefName2 = 'refModelName';
    
    open_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName, true));
    
    % The first subsystem has a mask help dialog. Check that we could simulate the model after converting
    % this subsystem to a referenced model.
    Simulink.SubSystem.convertToModelReference(subsysName1, modelRefName1, 'ReplaceSubsystem', true);    
    sim(modelName);
    
    % The second subsystem has one unused mask variable. Check that we could simulate the model after 
    % converting this subsystem to a referenced model.
    Simulink.SubSystem.convertToModelReference(subsysName2, modelRefName2, 'ReplaceSubsystem', true, 'Force', true);
    sim(modelName);
end

%% ------------------------------------------------------------------------
% 
function test = lvlTwo_copyModelWorkspace(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    
    modelName = 'mModelWorkspaceCopy';
    subsysName = 'mModelWorkspaceCopy/SS';
    modelRefName1 = 'myNewModel1';
    modelRefName2 = 'myNewModel2';
    
    open_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName, true));
    
    new_system(modelRefName1);
    open_system(modelRefName1);
    c2 = onCleanup(@() rtwtestclosemodel(modelRefName1));
    
    % Copy all parameters
    this = Simulink.ModelReference.ModelWorkspaceUtils(modelName, modelRefName1);
    this.copy;    
    modelParams = get_param(modelRefName1, 'ModelWorkspace');
    qeverify({length(modelParams.data), 4, '=='});
    
    % Only copy used parameters
    Simulink.SubSystem.convertToModelReference(subsysName, modelRefName2, 'ReplaceSubsystem', true);
    modelParams = get_param(modelRefName2, 'ModelWorkspace');
    qeverify({length(modelParams.data), 2, '=='});
    
    % Make sure we could simulate the top model after converion
    sim(modelName);
end
