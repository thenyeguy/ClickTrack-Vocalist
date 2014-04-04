function pulses = generatePulseTrain(freqs, durations)
%
% Generates a series of pulse trains at the given frequencys with the given
% durations, sampled at 44100Hz
%
% Inputs:  freqs - a vector containing frequencies of each note in Hz
%          durations - a vector containing the duration for each frequency,
%                      in seconds
%
% Outputs: pulses - a pulse train meeting the above specification
%

% Convert the units to samples
periods = round(44100./freqs);
durations = durations * 44100;

pulses = [];
for ii=1:numel(freqs)
    pulse = zeros(durations(ii),1);
    pulse(mod(0 : (durations(ii)-1), periods(ii)) == 0) = 1;
    
    pulses = [pulses; pulse]; %#ok append
end

end