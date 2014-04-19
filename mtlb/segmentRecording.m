function xs = segmentRecording(x, threshold)
%
% Given a recording and a threshold, this segments that recording into each
% continuous segment that exceeds the threshold
%

% Detect rising and falling edges of the thresholds
chunks = fitEnvelope(x/max(abs(x))) > threshold;
edges  = diff(chunks);

starts = find(edges == 1);
ends   = find(edges == -1);

% Extract each segment
xs = {};
for ii=1:length(starts)
    % Ignore short segments - under one second
    if ends(ii) - starts(ii) < 44100
        continue;
    end
    
    % Strip the beginning and end, and normalize
    seg = x((starts(ii)+10000) : (ends(ii)-10000));
    xs = [xs {seg / max(abs(seg))}]; %#ok append
end

end