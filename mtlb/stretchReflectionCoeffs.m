function outks = stretchReflectionCoeffs(ks,n)
%
% stretchReflectionCoeffs creates a set of reflection coeffecients n
% samples long by copying each column the same number of times
%
% Inputs:  ks - the reflection coefficients to stretch, one set per col
%          n - the number of samples to strech it over
%
% Outputs: outks - a matrix where each column is the reflection coefficients
%               for one of each n time step
%

% Perform stretching
outks = zeros(size(ks,1), n);
for t=1:n
    outks(:,t) = ks(:, ceil(t/n * size(ks,2)));
end

end