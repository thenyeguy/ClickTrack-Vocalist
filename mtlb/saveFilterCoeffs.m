function saveFilterCoeffs(name, reflectionCoeffs)
%
% Saves a set of coefficients in a format readable from the C++ instrument
%
% Inputs:  name - the name of the letter we are modeling
%          lpcCoeffs - a vector of lpc reflection coefficients
%
% Outputs: a file called PROJECTROOT/data/NAME.dat
%           NAME
%           NUMLPCFILTERCOEFFS
%           COEFF(0)
%              ...
%           COEFF(END)
%

% Open our file
file = fopen(['../data/' name '.dat'], 'w');

% Write out the header
fprintf(file, '%s\n', name);
fprintf(file, '%d\n', numel(reflectionCoeffs));

% Write out our coeffs one by one
for c = reflectionCoeffs
    fprintf(file, '%f\n', c);
end

% Close the file
fclose(file);

end