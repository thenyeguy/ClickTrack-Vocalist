function coeffs = lpcSeries(x)

coeffs = [];
stepSize = 44100/1000 * 20;
for ii=1 : stepSize : (length(x) - stepSize)
    seg = x(ii : ii+stepSize);
    cor = xcorr(seg); cor = cor(floor(length(cor)/2) : end);
    [~,~,K] = levinson(cor, 50);
    coeffs = [coeffs K]; %3ok append
end
    
end