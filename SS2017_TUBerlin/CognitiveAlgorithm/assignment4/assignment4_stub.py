import pylab as pl
import scipy as sp
from numpy.linalg import inv
from scipy.io import loadmat
import pdb

def load_myo_data(fname):
    ''' Loads EMG data from <fname>                      
    '''
    # load the data
    data = loadmat(fname)
    # extract data and hand positions
    X = data['training_data']
    #X = sp.log(X)
    Y = data['training_labels']
    #Split data into training and test data
    X_train = X[:, :5000]
    X_test = X[:, 5000:]
    Y_train = Y[:, :5000]
    Y_test = Y[:, 5000:]
    return X_train,Y_train,X_test, Y_test
	
def train_ols(X_train, Y_train, llambda = 0):
    ''' Trains ordinary least squares (ols) regression 
    Input:       X_train  -  DxN array of N data points with D features
                 Y        -  D2xN array of length N with D2 multiple labels
                 llabmda  -  Regularization parameter
    Output:      W        -  DxD2 array, linear mapping used to estimate labels 
                             with sp.dot(W.T, X)                      
    '''
    #your code here
    W = sp.dot(inv(sp.dot(X_train, X_train.T) + llambda * sp.identity(
        X_train.shape[0])), sp.dot(X_train, Y_train.T))
    return W
    
def apply_ols(W, X_test):
    ''' Applys ordinary least squares (ols) regression 
    Input:       X_test    -  DxN array of N data points with D features
                 W        -  DxD2 array, linear mapping used to estimate labels 
                             trained with train_ols                   
    Output:     Y_test    -  D2xN array
    '''
    Y_test = sp.dot(W.T, X_test)
    return Y_test
    
def predict_handposition():
    X_train,Y_train,X_test, Y_test = load_myo_data('myo_data.mat')
    # compute weight vector with linear regression
    W = train_ols(X_train, Y_train)
    # predict hand positions
    Y_hat_train = apply_ols(W, X_train)
    Y_hat_test = apply_ols(W, X_test) 
        
    pl.figure()
    pl.subplot(2,2,1)
    pl.plot(Y_train[0,:1000],Y_train[1,:1000],'.k',label = 'true')
    pl.plot(Y_hat_train[0,:1000],Y_hat_train[1,:1000],'.r', label = 'predicted')
    pl.title('Training Data')
    pl.xlabel('x position')
    pl.ylabel('y position')
    pl.legend(loc = 0)
    
    pl.subplot(2,2,2)
    pl.plot(Y_test[0,:1000],Y_test[1,:1000],'.k')
    pl.plot(Y_hat_test[0,:1000],Y_hat_test[1,:1000],'.r')
    pl.title('Test Data')
    pl.xlabel('x position')
    pl.ylabel('y position')
    
    pl.subplot(2,2,3)
    pl.plot(Y_train[1,:600], 'k', label = 'true')
    pl.plot(Y_hat_train[1,:600], 'r--', label = 'predicted')
    pl.xlabel('Time')
    pl.ylabel('y position')
    pl.legend(loc = 0)
    
    pl.subplot(2,2,4)
    pl.plot(Y_test[1,:600],'k')
    pl.plot(Y_hat_test[1,:600], 'r--')
    pl.xlabel('Time')
    pl.ylabel('y position')
    
def test_assignment4():
    ##Example without noise
    x_train = sp.array([[ 0,  0,  1 , 1],[ 0,  1,  0, 1]])
    y_train = sp.array([[0, 1, 1, 2]])
    w_est = train_ols(x_train, y_train) 
    w_est_ridge = train_ols(x_train, y_train, llambda = 1)
    assert(sp.all(w_est.T == [[1, 1]])) 
    assert(sp.all(w_est_ridge.T == [[.75, .75]]))
    y_est = apply_ols(w_est,x_train)
    assert(sp.all(y_train == y_est)) 
    print 'No-noise-case tests passed'
	
	##Example with noise
	#Data generation
    w_true = 4
    X_train = sp.arange(10)
    X_train = X_train[None,:]
    Y_train = w_true * X_train + sp.random.normal(0,2,X_train.shape)
    #Regression 
    w_est = train_ols(X_train, Y_train) 
    Y_est = apply_ols(w_est,X_train)
    #Plot result
    pl.figure()
    pl.plot(X_train.T, Y_train.T, '+', label = 'Train Data')
    pl.plot(X_train.T, Y_est.T, label = 'Estimated regression')
    pl.xlabel('x')
    pl.ylabel('y')
    pl.legend(loc = 'lower right')
    

# Task 6.

def apply_ols_ploy(x,y,m,l):
    # m: m-th order polynomial
    # l: lambda
    phy_x = sp.ones((m+1,x.shape[0]))
    
    for i in range(m):
        phy_x[i+1,:] = x**(i+1);
    
    w = train_ols(phy_x,y[None,:],l)
    
    return sp.sum(w*phy_x,axis=0)
    

def test_polynomial_regression():




    x = sp.linspace(0,10,num=10*10+1)
    y = sp.sin(x) + sp.random.normal(0, 0.5)
    
    #pick points from generated array
    x_p = x[sp.arange(11)*10]
    y_p = y[sp.arange(11)*10]


    pl.figure()
    
    #pl.subplot(1,2,1)
    pl.title('Polynomial regression for different degrees m')
    pl.plot(x_p,y_p,'b+', label ="Train data")
    pl.plot(x,apply_ols_ploy(x,y,1,0), 'g-.',label="m = 1")
    pl.plot(x,apply_ols_ploy(x,y,4,0), 'b-',label="m = 4")
    pl.plot(x,apply_ols_ploy(x,y,10,0), 'r--',label="m = 10")
    pl.xlabel('x')
    pl.ylabel('y')
    pl.legend(loc='lower left')
    pl.show();
    
    #pl.subplot(1,2,2)
    pl.figure()
    pl.title('Polynomial ridge regression (m = 9)')
    pl.plot(x_p,y_p,'b+', label ="Train data")
    pl.plot(x,apply_ols_ploy(x,y,9,0), 'r--',label="r = 0")
    pl.plot(x,apply_ols_ploy(x,y,9,1), 'b-',label="r = 1")
    pl.plot(x,apply_ols_ploy(x,y,9,10000), 'g-.',label="r = 10000")
    pl.xlabel('x')
    pl.ylabel('y')
    pl.legend(loc='lower right')
    
    pl.tight_layout()
    pl.show();

