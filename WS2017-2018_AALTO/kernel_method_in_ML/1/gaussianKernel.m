function ret = gaussianKernel(X, Z, S)

num_m = size(X,1);
num_z = size(Z,1);

myNorm = repmat(sum(X,2),1,num_z) - 2*X*Z' + repmat(sum(Z,2)',num_m,1);

ret = exp(- myNorm ./ (2 * (S^2) ));

end
