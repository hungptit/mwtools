function [test] = tFunctionInterfaceWriter(point)
    testharness;
end


function [test,testparams]=setup(test, testparams)
    testparams = slrtw_remove_drool(testparams, 'setup');
    testparams.origfeature = slfeature('PILSimTargetUnification', 1);
end


function test = cleanup(test, testparams)
    slrtw_remove_drool(testparams, 'cleanup');
    slfeature('PILSimTargetUnification', testparams.origfeature);
end


function test = lvlTwo_getActualArguments(test, testparams)
    load('codeInfo.mat');
    actualArgs = rtw.modelreference.FunctionInterfaceUtils.getActualArguments(codeInfo.OutputFunctions);
    variableNames = {'In1'; 'Out1'; 'In2'; 'Out2'; 'In3'; 'Out3'};
    qeverify({get(actualArgs, 'GraphicalName'), variableNames, '=='});
    evalin('base', 'clear codeInfo;');
end
