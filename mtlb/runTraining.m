%
% runTraining.m - a script to run a full training of our vocal models
%


%% Define constants
poles = 100; % number of poles to use in our model


%% Train LPC models
disp 'Training vowels...'
A = trainModel('../wav/raw/A.wav', poles);
E = trainModel('../wav/raw/E.wav', poles);
I = trainModel('../wav/raw/I.wav', poles);
O = trainModel('../wav/raw/O.wav', poles);
U = trainModel('../wav/raw/U.wav', poles);

disp 'Training consonants...';
V = trainModel('../wav/raw/V.wav', poles);
Z = trainModel('../wav/raw/Z.wav', poles);
disp 'Complete.'


%% Normalize gains
disp 'Normalizing gains...'
all = [A E I O U V Z];
gains = [all.gain];
maxGain = max(gains);

A.gain = A.gain/maxGain;
E.gain = E.gain/maxGain;
I.gain = I.gain/maxGain;
O.gain = O.gain/maxGain;
U.gain = U.gain/maxGain;

V.gain = V.gain/maxGain;
Z.gain = Z.gain/maxGain;

clear all gains maxGain;


%% Save models
disp 'Saving model data...'
saveModel(A);
saveModel(E);
saveModel(I);
saveModel(O);
saveModel(U);

saveModel(V);
saveModel(Z);