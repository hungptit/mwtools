function test = tExportedFunction(point)

%   Copyright 2013 The MathWorks, Inc.

    testharness;
end


function [test, testparams] = setup(test, testparams)
    testparams = qe_model_reference(testparams,'setup');
    testparams.convFeature = slfeature('ModelreferenceconversionTesting');
    testparams.RootFcnCallInportTopLevelBuild = slfeature('RootFcnCallInportTopLevelBuild', 1);
    slfeature('ModelreferenceconversionTesting',0);
end


function test = cleanup(test,testparams)
    testparams = qe_model_reference(testparams, 'cleanup');
    slfeature('ModelreferenceconversionTesting', testparams.convFeature);
    slfeature('RootFcnCallInportTopLevelBuild', testparams.RootFcnCallInportTopLevelBuild);
end


function test = lvlTwo_ExportedFunctionBasic(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    modelName = 'mExportedFcnBasic';
    subsys = [modelName, '/Subsystem'];
    modelRefName = 'test_model';
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName, true));
    
    Simulink.SubSystem.convertToModelReference(subsys, modelRefName, 'ExportedFunctionSubsystem', true, 'ReplaceSubsystem', true);
    sim(modelName);
end


function test = lvlTwo_ExportedFunctionBasicNeg(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    modelName = 'mExportedFcnBasic';
    subsys = [modelName, '/Subsystem'];
    modelRefName = 'test_model';
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    % Cannot convert an exported subsystem to a referenced model when the RootFcnCallInportTopLevelBuild feature is off
    slfeature('RootFcnCallInportTopLevelBuild', 0);
    qeVerifyError('Simulink:modelReference:convertToModelReference_InvalidInputArgument', ...
                  @() Simulink.SubSystem.convertToModelReference(subsys, modelRefName, 'ExportedFunctionSubsystem', true, 'ReplaceSubsystem', true));
    slfeature('RootFcnCallInportTopLevelBuild', 1);

    % Invalid value for ExportedFunctionSubsystem option
    qeVerifyError('Simulink:modelReference:convertToModelReference_ExportedFcn', ...
                  @() Simulink.SubSystem.convertToModelReference(subsys, modelRefName, 'ExportedFunctionSubsystem', 'tr', 'ReplaceSubsystem', true));
end


function test = lvlTwo_GUI(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    modelName = 'mExportedFcnBasicNeg';
    subsys = [modelName, '/Subsystem'];
    modelRefName = 'test_model';
    
    load_system(modelName);
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_invalidExportedFunctionSS', ...
                  @() Simulink.SubSystem.convertToModelReference(subsys, modelRefName, 'ExportedFunctionSubsystem', true));
    
    qeVerifyError('Simulink:modelReference:invalidSSTypeVirtual', ...
                  @() Simulink.SubSystem.convertToModelReference(subsys, modelRefName, 'Force', 'controlFromUI'));    
end
