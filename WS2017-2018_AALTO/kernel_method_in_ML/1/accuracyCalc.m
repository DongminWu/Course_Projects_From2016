function ret = accuracyCalc(X_train, X_test, y_train,y_test,S)
    num_S = size(S,2);
    num_test = size(y_test,1);
    ret = [];
    
    for i = 1:num_S
        correct = 0;
        predit = preditResult(X_train,X_test,y_train,S(:,i));
        for j = 1:num_test
            if predit(j) == y_test(j)
                correct += 1;
            end
        end
        ret = [ret; 1.0*correct/num_test];
    end
end