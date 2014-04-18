function ks = interpolateReflectionCoeffs(k1,k2,n)
%
% interpolateReflectionCoeffs creates a linear interpolation between two
% sets of reflection coefficients over time
%
% Inputs:  k1 - the reflection coefficients at time t=1
%          k2 - the reflection coefficients at time t=n
%          n - how many steps of interpolation
%
% Outputs: ks - a matrix where each column is the reflection coefficients
%               for one of each n time step
%

ks = zeros(length(k1), n);

for ii=1:n
    a = (ii-1)/(n-1);
    ks(:,ii) = (1-a)*k1 + a*k2;
end

end