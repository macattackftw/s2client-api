import ScoreCNN
import numpy as np
import sys, csv

def nonlin(x, deriv=False):
    if(deriv==True):
        return x*(1-x)

    return 1/(1+np.exp(-x))

np.random.seed(1)

#set weight valus, random for now
syn0 = 2*np.random.random((5,4))-1
syn1 = 2*np.random.random((4,1))-1

for i in xrange(100000):

    #feed through the layers
    l0 = ScoreCNN.get_input(sys.argv[1], 60)
    l1 = nonlin(np.dot(l0,syn0))
    l2 = nonlin(np.dot(l1,syn1))
 
    #find the difference between calculated and desired
    l2_error = ScoreCNN.get_output(sys.argv[1],60)  - l2
    if(i%10000) == 0:
        print "Error=" + str(np.mean(np.abs(l2_error)))
        print l2
        print l2_error

    
    #find direction of miss
    l2_delta = l2_error*nonlin(l2, deriv=True)

    #value of l1 error contribution
    l1_error = l2_delta.dot(syn1.T)


    #find direction that l1 missed
    l1_delta = l1_error * nonlin(l1, deriv=True)

    syn1 +=l1.T.dot(l2_delta)
    syn0 +=l0.T.dot(l1_delta)
    
