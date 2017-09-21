function ret = preditResult(X_train,X_test,labelX,S)
%myFun - Description
%
% Syntax: ret = myFun(input)
%
% Long description

num_X_train = size(X_train,1);
num_X_test = size(X_test,1);

X_train_pos = [];
X_train_neg = [];
X_test_pos = [];
X_test_neg = [];

for index = 1:num_X_train
    if labelX(index) >= 0
        X_train_pos = [X_train_pos;X_train(index,:)];
    else
        X_train_neg = [X_train_neg;X_train(index,:)];
    end
end



num_X_train_pos = size(X_train_pos,1);
num_X_train_neg = size(X_train_neg,1);



K_neg = gaussianKernel(X_train_neg,X_train_neg,S);
K_pos = gaussianKernel(X_train_pos,X_train_pos,S);

b = (1/(2*size(K_neg,1)^2))*sum(sum(K_neg,2)) - 1/(2*size(K_pos,1)^2)*sum(sum(K_pos,2));

ret_pos = 1/num_X_train_pos*(sum(gaussianKernel(X_train_pos,X_test,S)));
ret_neg = -1/num_X_train_neg*(sum(gaussianKernel(X_train_neg,X_test,S)));

hypothesis = ret_pos+ret_neg+b;

ret = [];
for i = 1:size(hypothesis,2)
    
    if hypothesis(:,i) >0
        ret = [ret;1];
    else
        ret = [ret;-1];
    end
end
    
end