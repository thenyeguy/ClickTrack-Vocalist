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

M = trainModel('../wav/raw/M.wav', poles);
N = trainModel('../wav/raw/N.wav', poles);

T = trainModel('../wav/raw/T.wav', poles);
P = trainModel('../wav/raw/P.wav', poles);
K = trainModel('../wav/raw/K.wav', poles);

disp 'Complete.'


%% Normalize gains
disp 'Normalizing gains...'
all = [A E I O U V Z M N T P K];
gains = [all.gain];
maxGain = max(gains);

A.gain = A.gain/maxGain;
E.gain = E.gain/maxGain;
I.gain = I.gain/maxGain;
O.gain = O.gain/maxGain;
U.gain = U.gain/maxGain;

V.gain = V.gain/maxGain;
Z.gain = Z.gain/maxGain;

M.gain = M.gain/maxGain;
N.gain = N.gain/maxGain;

T.gain = T.gain/maxGain;
P.gain = P.gain/maxGain;
K.gain = K.gain/maxGain;


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

saveModel(M);
saveModel(N);

saveModel(T);
saveModel(P);
saveModel(K);
disp 'Done!'