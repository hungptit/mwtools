function [test] = tSubsystemConversion(point)

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
    modelName = 'mSubsystemConversion';
    subsys = [modelName, '/Subsystem'];
    modelReferenceName = 'new_model';
    load_system(modelName);    
    c1 = onCleanup(@() rtwtestclosemodel(modelName, true));
    
    % Convert a subsystem to a model reference using runCheck method
    this = Simulink.ModelReference.SubsystemConversion(subsys, modelReferenceName);
    
    % Check model settings
    this.runCheck('com.mathworks.Simulink.ModelReferenceAdvisor.ModelConfigurations');
    
    % Check subsystem before compilation
    checkIds = { ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.SubsystemType', ...
    'com.mathworks.Simulink.ModelReferenceAdvisor.LinkToADirtyLibrary', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.MaskedSubsystem', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.StateflowSubsystem'};
    runCheck(this, checkIds);
    
    % Compile model
    this.runCheck('com.mathworks.Simulink.ModelReferenceAdvisor.CompileModel');

    % Check subsystem after compilation
    checkIds = { ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.InactiveVariant', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.Inports', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.SampleTimes', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.DSMBlocks', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.FunctionCall', ...
        'com.mathworks.Simulink.ModelReferenceAdvisor.VariableDimensionPorts'};
    runCheck(this, checkIds);
    
    % Complete conversion
    this.runCheck('com.mathworks.Simulink.ModelReferenceAdvisor.CompleteConversion');
end


function runCheck(this, checkIds)
    numberOfChecks = length(checkIds);
    for idx = 1:numberOfChecks
        this.runCheck(checkIds{idx});
    end
end
