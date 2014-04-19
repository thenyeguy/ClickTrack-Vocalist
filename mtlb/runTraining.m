%
% runTraining.m - a script to run a full training of our vocal models
%


%% Define constants
poles = 50; % number of poles to use in our model


%% Train LPC models
disp 'Training vowels...'
A = trainModel('../wav/raw/A.wav', poles);
E = trainModel('../wav/raw/E.wav', poles);
I = trainModel('../wav/raw/I.wav', poles);
O = trainModel('../wav/raw/O.wav', poles);
U = trainModel('../wav/raw/U.wav', poles);
disp 'Complete.'


%% Save models
disp 'Saving model data...'
saveModel(A);
saveModel(E);
saveModel(I);
saveModel(O);
saveModel(U);