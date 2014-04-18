function saveModel(model)
%
% Saves a set of coefficients in a format readable from the C++ instrument
%
% Inputs:  model - a model struct with name and ks fields
%
% Outputs: a file called PROJECTROOT/data/NAME.dat
%           NAME
%           NUMLPCFILTERCOEFFS
%           COEFF(0)
%              ...
%           COEFF(END)
%

% Open our file
file = fopen(['../data/' model.name '.dat'], 'w');

% Write out the header
fprintf(file, '%s\n', model.name);
fprintf(file, '%d\n', numel(model.ks));

% Write out our coeffs one by one
for k = model.ks
    fprintf(file, '%f\n', k);
end

% Close the file
fclose(file);

end