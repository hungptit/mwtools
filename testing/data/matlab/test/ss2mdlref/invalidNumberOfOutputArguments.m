function invalidNumberOfOutputArguments(fcnCallCmd, subsysHandle)

%   Copyright 2012 The MathWorks, Inc.

    [out1, out2] = slInternal(fcnCallCmd, subsysHandle); %#ok
end
