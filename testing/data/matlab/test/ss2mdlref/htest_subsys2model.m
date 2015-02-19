
% Copyright 2004-2012 The MathWorks, Inc.
% $Revision.3.2.1 $
%
function test = htest_subsys2model(test, baseFileName, mdl, subsys, mdlRef, varargin)

  needCleanup = 0;
  w = warning;
  warning on;
  
  origListOfOpenModels = find_system('flat');
  
  try    
    [startdir, testdir, test, hadErr] = rtwtestsetup(test);
    if hadErr, i_jump; end;
    needCleanup = 1;
    
    load_and_setup_system_l(mdl, subsys);

    if evalin('base','exist(''rtwsimGenerateBaseLine'',''var'')')
      genBaseline = evalin('base','rtwsimGenerateBaseLine==1');
    else 
      genBaseline = false;
    end 

    if ~genBaseline
      
      %% Now verify that the original model gives the expected answers
      modes{1}.SimOnly=1;
      % Skip state checks because they won't work without a significant effort
      % since the block names will be different after conversion
      modes{1}.saveStates = 0;
      modes{1}.checkFinalState = 0;
      modes{1}.Accelerator = 0;
      
      % Call rtwsim for the original model
      % Note we need to do this before doing the conversion because we'll
      % be pointing to the converted libraries after conversion and so
      % won't really have the original model any more
      test = rtwsim(test, mdl, ...
                    modes,{'baseFileName',baseFileName,...
                          'StayInCurrDir',1});

      
      
      % Convert the model and verify that it was successful
      load_and_setup_system_l(mdl, subsys);
      [success, mdlBlkH] = Simulink.SubSystem.convertToModelReference(subsys, mdlRef, ...
                                                        'ReplaceSubsystem', true, ...
                                                        varargin{:});
      
      simulationMode = get_param(mdlBlkH, 'SimulationMode');
      
      test = qeverify(test,{success,true, '==','testdesc',...
                          'The model should be able to successfully convert'}, ...
                      {strcmpi(simulationMode, 'normal'), true, '==', 'testdesc', ...
                       'The newly created Model block should be in normal mode'});
      
      newMdl = [mdl, '_new'];
      save_system(mdl, newMdl);
      
      actMdlRefs = find_mdlrefs(newMdl);
      index = strcmp(mdlRef, actMdlRefs);
      
      test = qeverify(test,{sum(index),1, '==','testdesc',...
                  ['did not find one Model block referencing: ', mdlRef]});
    
      solverType = get_param(newMdl,'SolverType');
      
      if isequal(solverType, 'Fixed-step')
        %% Now verify that the converted model gives the same answers
        % Call rtwsim for the converted model
        % For now only use ert and accelerator for testing
        modes = rtwsimstate('modelref');
        modes = rtwsimstate( modes, 'DELETE', ...
                             {'RTWSystemTargetFile', 'grt.tlc' });
        modes = rtwsimstate( modes, 'DELETE', ...
                             {'RTWSystemTargetFile', 'rsim.tlc' });
        for idx = length(modes)
          modes{idx}.saveStates = 0;
          modes{idx}.checkFinalState = 0;
        end
      end

      morePrms = {'baseFileName',baseFileName, 'StayInCurrDir',1};
      if strcmp(get_param(newMdl,'GenCodeOnly'),'on')
        morePrms{end+1} = 'generateCodeOnly';
        morePrms{end+1} = 1;
      end
      test=rtwsim(test, newMdl,modes,morePrms);
    else
      % generate the baseline
      modes = rtwsimstate('modelref');
      test=rtwsim(test, mdl,...
                  modes,{'StayInCurrDir',1});
    end
    
  catch
    disp(['*>Test failed due to error:', lasterr]);
    test = qeverify(test, {1,0,'==','testdesc',['Failed '...
                        'when converting a model to using model reference']});
  end

  newListOfOpenModels = find_system('flat');
  
  modelsToClose = '';
  
  for newMdlIdx = 1:length(newListOfOpenModels)
    found = false;
    for origMdlIdx = 1:length(origListOfOpenModels)
      if strcmp(newListOfOpenModels{newMdlIdx},origListOfOpenModels{origMdlIdx})
        found = true;
      end
    end
    if ~found
      modelsToClose{end+1} = newListOfOpenModels{newMdlIdx};
    end
  end
  
  for mdlIdx = 1:length(modelsToClose)
    rtwtestclosemodel(modelsToClose{mdlIdx});
  end

  warning(w);    
  if needCleanup
    rtwtestcleanup(startdir, testdir);
  end

% This is a helper function to convert a subsystem to model reference.
% This allows this m-file to be called for virtual subsystems and
% linked blocks. 
%   (See test/toolbox/simulink/execcontext/texeccontext.m
%    Test point: lvlTwo_g197447_1 and  lvlTwo_g197447_2)
%
function load_and_setup_system_l(mdl, subsys)
  load_system(mdl);
  set_param(mdl, 'StrictBusMsg', 'ErrorLevel1');
  set_param(mdl, 'SignalResolutionControl', 'UseLocalSettings');
  set_param(mdl, 'InlineParams', 'on');
  if ~isempty(subsys)
    % Force to instantiate library links
    find_system(mdl,...
                'FollowLinks','on',...
                'LookUnderMasks','all');
    break_links_l(subsys);
    set_param(subsys, 'TreatAsAtomicUnit','on');
  end
  
%endfunction

function break_links_l(subsys)
  parent = get_param(subsys,'Parent');
  if ~isempty(parent)
    break_links_l(parent);
    set_param(subsys,'LinkStatus','none');
  end
%endfunction


  
