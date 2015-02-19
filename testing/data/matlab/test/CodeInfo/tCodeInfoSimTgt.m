function [test] = tCodeInfoSimTgt(point)
% 
% Tests for using the CodeInfo to generate the simTgt.
%----------------------------------
% Do not modify this code block.
%----------------------------------
% Create an null variable 'point' if it doesn't already exist,
% to prevent bogus warnings in function calls.

% Copyright 2011-2012 The MathWorks, Inc.
  testharness;
end

% -----------------------------------------------------------------------  
function [test,testparams]=setup(test, testparams)
% SETUP setup features
    testparams = slrtw_remove_drool(testparams, 'setup');

    testparams.origfeature = slfeature('PILSimTargetUnification', 1);

end %setup


% -----------------------------------------------------------------------
function test = cleanup(test, testparams)
% CLEANUP cleanup
    slrtw_remove_drool(testparams, 'cleanup');
    
    slfeature('PILSimTargetUnification', testparams.origfeature);
end % cleanup


% -----------------------------------------------------------------------
% Continuous-states with variable step
function test = lvlTwo1(test, testparams)
    model = 'mContinuousTop';
    locCheckSimResults(model);
end

% -----------------------------------------------------------------------
% Completely constant Model block
function test = lvlTwo2(test, testparams)
    model = 'mConstSampTimeTop';
    locCheckSimResults(model);
end

% -----------------------------------------------------------------------
% Inherited plus constant output
function test = lvlTwo3(test, testparams)
    model = 'mInhWithConstTop';
    locCheckSimResults(model);
end

% -----------------------------------------------------------------------
% Global parameters and model arguments
function test = lvlTwo4(test, testparams)
    model = 'mParametersTop';
    locCheckSimResults(model);
end

% -----------------------------------------------------------------------
% Check mdlDisable for vardim scalar and matrix
function test = lvlTwo_mdlDisableVarDims(test, testparams)
    tmpDir = qeWorkingDir(); %#ok

    modelName = 'mDisableBusWithVarDim';
    slbuild(modelName, 'ModelReferenceSimTarget');
    
    modelName = 'mDisableBusWithVarDim';
    slbuild(modelName, 'ModelReferenceSimTarget');
    
    locCheckSimResults(modelName);
end

% -----------------------------------------------------------------------
% Check that we do not set the output ports inside the mdlOutputs functions if 
% PropagateVarSize is set to 'FromInputSize'
function test = lvlTwo_mdlOutputVarDims_opt(test, testparams)
    tmpDir = qeWorkingDir(); %#ok
    modelName1 = 'mVarBus';
    modelRefName = 'mVarBus_ref';
    modelName2 = 'mVarBus_baseline';
    
    load_system(modelRefName);
    c2 = onCleanup(@() rtwtestclosemodel(modelRefName));
    
    load_system(modelName1);
    c1 = onCleanup(@() rtwtestclosemodel(modelName1));
    
    load_system(modelName2);
    c2 = onCleanup(@() rtwtestclosemodel(modelName1));
    
    % Check generated code
    testparams.ofc = get_param(0, 'AcceleratorUseTrueIdentifier');
    set_param(0, 'AcceleratorUseTrueIdentifier', 'on');
    [startdir, testdir, test, hadErr] = rtwtestsetup(test); %#ok
    
    slbuild(modelRefName, 'ModelReferenceSimTarget');

    opts.targetType = 'ModelRefSim';
    opts.filetypes = {'mVarBus_ref_msf.c'};
    opts.include = {'\s*if \(tid != CONSTANT_TID\) {\n*', ...
                    '\s*mVarBus_ref\(ROOT_U0, Out1, ROOT_Y1, ROOT_Y2, OutVarDims2, ROOT_Y3, Out5,\n*', ...
                    '\s*ROOT_Y5, OutVarDims5, ROOT_Y6, ROOT_Y7, &\(OutVarDims7\), ROOT_Y8,\n*', ...
                    '\s*&\(OutVarDims8\), &\(dw->rtdw\)\);\n*', ...
                    '\s*}\n*'};
    test = rtwgencodecheck(test, modelRefName, [], opts);
    
    set_param(0, 'AcceleratorUseTrueIdentifier', testparams.ofc);

    % Verify results from accelerated model with that of normal mode.    
    y1 = sim(modelName1, 'ReturnWorkspaceOutputs', 'on');
    y2 = sim(modelName2, 'ReturnWorkspaceOutputs', 'on');
    validateSimulationResults(y1.get('yout'), y2.get('yout'));
end


function locCheckSimResults(model)
    obj = qeWorkingDir; %#ok
    load_system(model);
    
    r = sim(model, 'ReturnWorkspaceOutputs', 'on');
    
    yout = r.get('yout');
    qeverify({yout, zeros(size(yout)), '=='});
    
    rtwtestclosemodel(model);
end


function validateSimulationResults(y1, y2)
    numberOfDataset = length(y1.signals);
    for signalIdx = 1:numberOfDataset        
        qeverify({y1.signals(signalIdx).values, y2.signals(signalIdx).values});
    end
end
