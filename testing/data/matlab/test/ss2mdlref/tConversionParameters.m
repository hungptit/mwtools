function [test] = tConversionParameters(point)
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
    modelName = 'mConversionParameters';
    subsys = 'mConversionParameters/Subsystem';
    modelReferenceName = 'new_model';
    load_system(modelName);    
    c1 = onCleanup(@() rtwtestclosemodel(modelName));
    this = runTest(subsys, modelReferenceName);
    
    % Test subsystem and model's name
    qeverify({this.SubsystemNames{1}, subsys});
    qeverify({this.ModelName, modelName});
    
    % Test default parameters
    qeverify({this.BusSaveFormat, ''});
    qeverify({this.ReplaceSubsystem, false});
    qeverify({this.BuildTarget, ''});
    qeverify({this.Force, 'off'});
end


function test = lvlTwo_BasicNegative(test, testparams)
    subsys = 'mConversionParameters/Subsystem';
    modelReferenceName = 'foo';
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_BuildTarget', ...
                  @() runTest('Test', 'foo', 'BuildTarget', 'foo'));    

    qeVerifyError('Simulink:modelReference:BusSaveFormat', ...
                  @() runTest(subsys, modelReferenceName, 'BusSaveFormat', 'foo'));
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_Force', ...
                  @() runTest(subsys, modelReferenceName, 'Force', 'foo'));
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_ReplaceSubsystem', ...
                  @() runTest(subsys, modelReferenceName, 'ReplaceSubsystem', 'foo'));
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_InvalidInputArgument', ...
                  @() runTest(subsys, modelReferenceName, 'BuildThisTarget', 'foo'));    
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_InvalidModelRef', ...
                  @() runTest(subsys, 'foo xxx!'));    
    
    qeVerifyError('Simulink:modelReference:convertToModelReference_ModelRefFileExisted', ...
                  @() runTest(subsys, 'mInvalidSubsys'));        
end
 

function this = runTest(varargin)
    this = Simulink.ModelReference.ConversionParameters(varargin{:});
end
