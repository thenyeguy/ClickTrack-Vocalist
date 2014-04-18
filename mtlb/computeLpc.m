function [alphas, ks] = computeLpc(x, P)
%
% computeLpc calculates coefficients for an all pole model of the signal
%
% Inputs:  x - a vector representing our acoustic signal
%          P - the number of poles to calculate
%
% Outputs: alphas - a denominator polynomial for our filter response
%          ks - the reflection coefficients for an all pole lattice
%

if nargin < 2
    P = length(x);
end

% Get the autocorrelation and shift it so that cor(1) is the zero lag
phi = ifftshift(xcorr(x, P));

% Initialize the containers
errors = zeros(P,1);
errors(1) = phi(1);

ks = zeros(P,1);
ks(1) = phi(2)/errors(1);

alphas = zeros(P, P);
alphas(1,1) = ks(1);

for ii = 2:P
    %Update the error
    errors(ii) = (1-ks(ii-1)^2) * errors(ii-1);
    
    % Update the reflection coefficient
    ks(ii) = (phi(ii+1) - alphas(1:ii, ii-1)'*phi(ii:-1:1)) / ...
        errors(ii);
    
    % Compute the new alphas
    alphas(1:(ii-1), ii) = alphas(1:(ii-1), ii-1) - ...
        ks(ii)*alphas((ii-1):-1:1, ii-1);
    alphas(ii,ii) = ks(ii);
end

% Extract just the last alphas, and convert to the filter denominator form
alphas = [1; -1*alphas(:,end)];

end