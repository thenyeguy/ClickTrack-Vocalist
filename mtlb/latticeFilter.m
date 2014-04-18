function y = latticeFilter(x, ks)
%
% Filters a signal using an all pole lattice filter
%
% Inputs:  x - a vector representing our acoustic signal
%          ks - the reflection coefficients for an all pole lattice. If ks
%               is a vector, the lattice filter uses the same coeffs for
%               the entire time duration. Otherwise, it uses each column in
%               the ks matrix for one time step
%
% Outputs: y - the filtered output of the filter
%

if min(size(ks)) == 1
    ks = repmat(ks,1,length(x));
end

% Get constants
N = length(x);
P = size(ks,1)+1;

% Initialize the errors
ferrors = zeros(P, 1);
berrors = zeros(P, 1);

% Run the lattice
y = zeros(N,1);
for t=1:N    
    % Propogate the errors
    ferrors(end) = x(t);
    for ii = P:-1:2
        ferrors(ii-1) = ferrors(ii) + ks(ii-1,t)*berrors(ii-1);
        berrors(ii) = -ks(ii-1,t)*ferrors(ii-1) + berrors(ii-1);
    end
    berrors(1) = ferrors(1);
    
    % Grab the output
    y(t) = ferrors(1);
end

end