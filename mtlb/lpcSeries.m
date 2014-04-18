function coeffs = lpcSeries(x, binSize)
%
% lpcSeries constructs a series of reflection coefficients from bins of a
% certain length
%
% Inputs:  x - a vector representing our acoustic signal
%          binSize - a bin duration in ms
%
% Outputs: coeffs -a matrix of reflection coefficients. Each column
%                  represents the coeffs from one bin
%

coeffs = [];
stepSize = 44100/1000 * binSize;
for ii=1 : stepSize : (length(x) - stepSize)
    seg = x(ii : ii+stepSize);
    cor = xcorr(seg); cor = cor(floor(length(cor)/2) : end);
    [~,K] = trainLpcModel(cor, 50);
    coeffs = [coeffs K]; %#ok append
end
    
end