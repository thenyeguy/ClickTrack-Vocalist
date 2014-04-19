function ks = interpolateReflectionCoeffs(k1,k2,ni,n1,n2)
%
% interpolateReflectionCoeffs creates a linear interpolation between two
% sets of reflection coefficients over time
%
% Inputs:  k1 - the reflection coefficients at time t=1
%          k2 - the reflection coefficients at time t=n
%          ni - how many steps of interpolation between k1 and k2
%          n1 - how many steps of just k1. Defaults to 0
%          n2 - how many steps of just k2. Defaults to 0
%
% Outputs: ks - a matrix where each column is the reflection coefficients
%               for one of each n time step
%

% Set defaults
if nargin < 4
    n1 = 0;
end
if nargin < 5
    n2 = 0;
end

% Perform interpolation
ks = zeros(length(k1), ni);
for ii=1:ni
    a = (ii-1)/(ni-1);
    ks(:,ii) = (1-a)*k1 + a*k2;
end

% Stick in left and right constant regions
ks = [repmat(k1,1,n1) ks repmat(k2,1,n2)];

end