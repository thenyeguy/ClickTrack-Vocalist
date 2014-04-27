function model = trainModel(file, P)
%
% trainModel takes in the path to a wav file, and returns an lpc model for
% that file
%
% Inputs:  file - a path to a wav file
%          P - the number of poles to use. Defaults to 50
%
% Outputs: model - a struct as follows:
%              model.name - the filename
%              model.wav - the raw waveform
%              model.alphas - the denominator polynomial of an IIR filter
%              model.ks - the reflection coeffs of an all pole lattice
%

% Set default P value
if nargin < 2
    P = 50;
end

% Store the name
[~,model.name,~] = fileparts(file);

% Read in the file and compute segmends and compute LPC params
model.wav = wavread(file);
segs = segmentRecording(model.wav,0.1);

% Compute LPC for whole model
x = vertcat(segs{:});
[model.alphas model.ks model.gain] = computeLpc(x(:), P);

end