import scipy as sp
import scipy.io as io
import pdb
import pylab as pl

def load_usps_data(fname, digit=3):
    ''' Loads USPS (United State Postal Service) data from <fname> 
    Definition:  X, Y = load_usps_data(fname, digit = 3)
    Input:       fname   - string
                 digit   - optional, integer between 0 and 9, default is 3
    Output:      X       -  DxN array with N images with D pixels
                 Y       -  1D array of length N of class labels
                                 1 - where picture contains the <digit>
                                -1 - otherwise                           
    '''
    # load the data
    data = io.loadmat(fname)
    # extract images and labels
    X = data['data_patterns']
    Y = data['data_labels']
    Y = Y[digit,:]
    return X, Y
    
def train_perceptron(X,Y,iterations=200,eta=.1):
    ''' Trains a linear perceptron
    Definition:  w, b, acc  = train_perceptron(X,Y,iterations=200,eta=.1)
    Input:       X       -  DxN array of N data points with D features
                 Y       -  1D array of length N of class labels {-1, 1}
                 iter    -  optional, number of iterations, default 200
                 eta     -  optional, learning rate, default 0.1
    Output:      w       -  1D array of length D, weight vector 
                 b       -  bias term for linear classification                          
                 acc     -  1D array of length iter, contains classification accuracies 
                            after each iteration  
                            Accuracy = #correctly classified points / N 
    '''
    acc = sp.zeros((iterations))
    #include the bias term by adding a row of ones to X 
    X = sp.concatenate((sp.ones((1,X.shape[1])), X))
    #initialize weight vector
    weights = sp.ones((X.shape[0]))/X.shape[0]    
    for it in sp.arange(iterations):
        # indices of misclassified data
        wrong = (sp.sign(weights.dot(X)) != Y).nonzero()[0]
        
        # compute accuracy acc[it]
        # ... your code here
        N = X.shape[1]
        correct = X.shape[1] - wrong.shape[0]
        acc[it] = float(correct)/N

        if wrong.shape[0] > 0:
            
            # pick a random misclassified data point
            # ... your code here
            random_index = sp.random.randint(0, wrong.shape[0])
            random_point = X[:,wrong[random_index]]
            
            #update weight vector (use variable learning rate (eta/(1.+it)) )
            # ... your code here
            weights += (eta/(1.+it))*random_point*Y[wrong[random_index]]

            if it % 20 == 0:
                print "Iteration %d:"%it + "Accuracy %0.2f" %acc[it]
    b = -weights[0] 
    w = weights[1:]
    #return weight vector, bias and accuracies
    return w,b,acc

def train_ncc(X,Y):
    ''' Trains a prototype/nearest centroid classifier
    Definition:  w, b   = train_ncc(X,Y)
    Input:       X       -  DxN array of N data points with D features
                 Y       -  1D array of length N of class labels {-1, 1}
    Output:      w       -  1D array of length D, weight vector  
                 b       -  bias term for linear classification                          
    '''
    # ... your code here

    Yn = sp.copy(Y)
    # center of class '+1'
    # add 1 to the elements in Y and divide by 2: creates label 1 for class 1 and 0 for class -1
    Yn += 1
    Yn = Yn*0.5

    # number of elements in class 1
    class_1 = sp.sum(Yn)
    # multiply data points (everything will label 0 will have value 0)
    w_1 = sp.dot(Yn,X.T)/class_1

    # center of class '-1'
    # convert labels in Y: 0 for 1, 1 for -1
    Yn -= 1
    Yn = Yn*(-1.0)
    # number of elements in class -1
    class_minus1 = sp.sum(Yn)
    w_minus1 = sp.dot(Yn,X.T)/class_minus1
    
    w = (w_1 - w_minus1).T
    bias = (sp.dot(w_1.T, w_1) - sp.dot(w_minus1.T, w_minus1))*0.5
    return w, bias
    

def plot_histogram(X, Y, w, b):
    ''' Plots a histogram of classifier outputs (w^T X) for each class with pl.hist 
    The title of the histogram is the accuracy of the classification
    Accuracy = #correctly classified points / N 
    
    Definition:     plot_histogram(X, Y, w, b)
    Input:          X       -  DxN array of N data points with D features
                    Y       -  1D array of length N of class labels
                    w       -  1D array of length D, weight vector 
                    b       -  bias term for linear classification   
    
    '''
    # ... your code here
    x = sp.dot(w.T, X)
    x_b = x - b
    
    # compute accuracy
    computedclasses = sp.sign(x_b)
    correct = sp.dot(computedclasses,Y.T)
    # TODO: no idea why I have to divide by 2 :(
    correct += (Y.shape[0]-correct)/2   

    acc = correct/Y.shape[0]*100.0
    acc = int(round(acc))

    # plot accuracy as title
    pl.title("Accuracy:" + str(acc))

    # plot result for both classes
    # TODO: CHANGE GITHUB-CODE: w.dot(X[:,(Y == 1)]) ...
    pl.hist(w.dot(X[:,(Y == 1)]), label='target')
    pl.hist(w.dot(X[:,(Y == -1)]), label='non-target')
    
    pl.legend(loc='upper right')
   
    pl.xlabel("w^T X")
    
def compare_classifiers(digit = 3):
    ''' Loads usps.mat data, trains the perceptron and the Nearest centroid classifiers, 
    and plots their weight vector and classifier output
    Definition: compare_classifiers(digit = 3)
    '''
    X,Y = load_usps_data('usps.mat',digit)
    w_ncc,b_ncc = train_ncc(X,Y)
    w_per,b_per,_ = train_perceptron(X,Y)
    
    pl.figure()
    pl.subplot(2,2,1)
    plot_img(w_ncc)
    pl.title('NCC')
    pl.subplot(2,2,3)
    plot_histogram(X, Y, w_ncc, b_ncc)
    
    pl.subplot(2,2,2)
    plot_img(w_per)
    pl.title('Perceptron')
    pl.subplot(2,2,4)
    plot_histogram(X, Y, w_per, b_per)
    pl.show()
    
def analyse_accuracies_perceptron(digit = 3):
    ''' Loads usps.mat data and plots digit recognition accuracy in the linear perceptron
    Definition: analyse_perceptron(digit = 3)
    '''
    X,Y = load_usps_data('usps.mat',digit)
    w_per,b_per,acc = train_perceptron(X,Y)
    
    pl.figure()
    pl.plot(sp.arange(len(acc)),acc)
    pl.title('Digit recognition accuracy')      
    pl.xlabel('Iterations')
    pl.ylabel('Accuracy')
    pl.show()
   
def plot_img(a):
    ''' Plots one image 
    Definition: plot_img(a) 
    Input:      a - 1D array that contains an image 
    '''   
    # cast float to int
    bla = int(sp.sqrt(a.shape[0]))
    a2 = sp.reshape(a,(bla, bla))
    pl.imshow(a2, cmap='gray') 
    pl.colorbar()
    pl.setp(pl.gca(), xticks=[], yticks=[])
            
def plot_imgs(X, Y):   
    ''' Plots 3 images from each of the two classes 
    Definition:         plot_imgs(X,Y)
    Input:       X       -  DxN array of N pictures with D pixel
                 Y       -  1D array of length N of class labels {1, -1}                  
    '''
    pl.figure()
    for i in sp.arange(3):
        classpos = (Y == 1).nonzero()[0]
        m = classpos[sp.random.random_integers(0, classpos.shape[0]-1)]
        pl.subplot(2,3,1+i)
        plot_img(X[:, m])
    for i in sp.arange(3):
        classneg = (Y != 1).nonzero()[0]
        m = classneg[sp.random.random_integers(0, classneg.shape[0]-1)]
        pl.subplot(2,3,4+i)
        plot_img(X[:, m])
    pl.show()

X, Y = load_usps_data('usps.mat', 3)
plot_imgs(X,Y)
train_perceptron(X,Y,200,0.1)
analyse_accuracies_perceptron(digit = 3)
train_ncc(X,Y)
compare_classifiers(digit = 3)
