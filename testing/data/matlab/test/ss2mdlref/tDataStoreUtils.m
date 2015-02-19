function [test] = tDataStoreUtils(point)

%   Copyright 2013 The MathWorks, Inc.

testharness;
end


function [test,testparams] = setup(test,testparams)
    testparams = qe_model_reference(testparams,'setup');
    testparams.convFeature = slfeature('ModelreferenceconversionTesting');
    slfeature('ModelreferenceconversionTesting',0);
end


function test = cleanup(test,testparams)
    testparams = qe_model_reference(testparams,'cleanup');
    slfeature('ModelreferenceconversionTesting', testparams.convFeature);
end


function test = lvlTwo_Basic(test, testparams)
    tmpDir = qeWorkingDir(); %#ok    
    modelName = 'mDataStoreBlocks';
    subsys1 = [modelName '/Subsystem'];
    subsys2 = [modelName '/Subsystem1'];
    subsys3 = [modelName '/Subsystem2'];
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
        
    % Positive tests
    sim(modelName); % We should be able to simulate the model
    runPositiveTest(subsys1);

    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys2, true, expectedErrorIds);
           
    % Negative tests
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys2, false, expectedErrorIds);      
    
    % Test a masked subsystem
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys3, false, expectedErrorIds);
end


function test = lvlTwo_GlobalParam(test, testparams)
    modelName = 'mdatastore_glb';
    subsys = [modelName, '/SS'];
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    runPositiveTest(subsys);

    evalin('base', 'clear A;');
end


function test = lvlTwo_LibraryLink(test, testparams)
    modelName = 'mDataStoreBlocksInsideLibraryLink';
    subsys1 = [modelName, '/Subsystem1'];
    subsys2 = [modelName, '/Subsystem2'];
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    set_param(bdroot, 'SimulationCommand', 'Update');
    
    runPositiveTest(subsys1);
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys2, false, expectedErrorIds);
end


function test = lvlTwo_CommentedBlock(test, testparams)
    modelName = 'mDataStoreBlocksCommented';
    subsys = [modelName, '/Subsystem'];
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    sim(modelName); % We should be able to simulate the model
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys, false, expectedErrorIds);
end


function test = lvlTwo_Variants(test, testparams)
    modelName = 'mDataStoreBlocksVariants';
    subsys1 = [modelName, '/Controller1'];
    subsys2 = [modelName, '/Controller2'];
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    sim(modelName); % We should be able to simulate the model
    
    runPositiveTest(subsys1);   
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys2, false, expectedErrorIds);
end


function test = lvlTwo_Stateflow(test, testparams)
    modelName = 'mStateflowDSMBasic';
    subsys1 = [modelName, '/Subsystem1'];

    tmpDir = qeWorkingDir; %#ok
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    set_param(bdroot, 'SimulationCommand', 'Update'); % We should be able to update diagram
       
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys1, false, expectedErrorIds);
end


function test = lvlTwo_StateflowNameMapping(test, testparams)
    modelName = 'mStateflowDSMBasicNameMapping';
    subsys1 = [modelName, '/Subsystem'];

    tmpDir = qeWorkingDir; %#ok
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    set_param(bdroot, 'SimulationCommand', 'Update'); % We should be able to update diagram
    
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys1, false, expectedErrorIds);
end


function test = lvlTwo_MatlabFunction(test, testparams)
    modelName = 'mMatlabFunctionDSM';
    subsys1 = [modelName, '/Subsystem'];

    tmpDir = qeWorkingDir; %#ok
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    set_param(bdroot, 'SimulationCommand', 'Update'); % We should be able to update diagram
    
    expectedErrorIds = {'Simulink:modelReference:convertToModelReference_InvalidSubsystemLocalDataStoreCrossSys', ...
                        'Simulink:modelReference:convertToModelReference_LocalDataStoreCrossSystemBoundary'};
    runNegativeTest(subsys1, false, expectedErrorIds);
end


function runPositiveTest(subsys)
    this = Simulink.ModelReference.DataStoreUtils(subsys, false);
    this.check;
end


function runNegativeTest(subsys, forceToConvert, expectedErrorIds)
    this = Simulink.ModelReference.DataStoreUtils(subsys, forceToConvert);
    if forceToConvert
        % Check for warning only
        qeVerifyWarning(expectedErrorIds, @()this.check);
    else
        % Check for error message
        qeSLVerifyError(expectedErrorIds, @()this.check);
    end
end
