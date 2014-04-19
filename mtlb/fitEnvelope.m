function envelope = fitEnvelope(x)    
    % Leaky integrate the envelope
    envelope = zeros(1,length(x));
    alphaAttack = exp(-1/(44100*.001));
    alphaRelease = exp(-1/(44100*.1));
    for ii = 2:length(x)
        if abs(x(ii)) > envelope(ii-1)
            envelope(ii) = alphaAttack*envelope(ii-1) ...
                + (1-alphaAttack)*abs(x(ii));
        else
            envelope(ii) = alphaRelease*envelope(ii-1) ...
                + (1-alphaRelease)*abs(x(ii));
        end
    end
    
    envelope = (envelope);
end