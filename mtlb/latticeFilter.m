function y = latticeFilter(x, ks)

% Get constants
N = length(x);
P = length(ks)+1;

% Initialize the errors
ferrors = zeros(P, 1);
berrors = zeros(P, 1);

% Run the lattice
y = zeros(N,1);
for t=1:N    
    % Propogate the errors
    ferrors(end) = x(t);
    for ii = P:-1:2
        ferrors(ii-1) = ferrors(ii) + ks(ii-1)*berrors(ii-1);
        berrors(ii) = -ks(ii-1)*ferrors(ii-1) + berrors(ii-1);
    end
    berrors(1) = ferrors(1);
    
    % Grab the output
    y(t) = ferrors(1);
end

end