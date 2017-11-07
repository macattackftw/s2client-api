import ScoreCNN
import numpy as np
import sys, csv

#a sigmoid function to find probability
def nonlin(x, deriv=False):
    if(deriv==True):
        return x*(1-x)

    return 1/(1+np.exp(-x))



#set weight valus, wieght set to try and implicate base importance
np.random.seed(1)
cheese_w = np.array([[5],[0], [0],[.01],[.001]])
all_in_w = np.array([[5],[0], [0],[.01],[.001]])
timing_w = np.array([[5],[0], [0],[.01],[.001]])
eco_w = np.array([[5],[0], [0],[.01],[.001]])
ans = ScoreCNN.get_output(sys.argv[1],60)
data_in = ScoreCNN.get_input(sys.argv[1], 60)
print ans
print data_in
for i in xrange(100000):

    #set base layer equal to the input array

    #Find an initial guess for each strat
    cheese_l = nonlin(np.dot(data_in, cheese_w))
    all_in_l = nonlin(np.dot(data_in, all_in_w))
    timing_l = nonlin(np.dot(data_in, timing_w))
    eco_l = nonlin(np.dot(data_in, eco_w))
    #find error for each guess
    cheese_err = ans[0] - cheese_l
    all_in_err = ans[1] - all_in_l
    timing_err = ans[2] - timing_l
    eco_err = ans[3] - eco_l
    #find which direction each guess was off
    cheese_d = cheese_err*nonlin(cheese_l, deriv=True)
    all_in_d = all_in_err*nonlin(all_in_l, deriv=True)
    timing_d = timing_err*nonlin(timing_l, deriv=True)
    eco_d = eco_err*nonlin(eco_l, deriv=True)
    #now we modify the wieght values to close in on the answer
    cheese_w +=cheese_l.T.dot(cheese_d)
    all_in_w +=all_in_l.T.dot(cheese_d)
    timing_w +=timing_l.T.dot(cheese_d)
    eco_w +=eco_l.T.dot(cheese_d)

    #combine into one array and find overall error
    guess = np.concatenate((cheese_l, all_in_l, timing_l, eco_l), axis = 0)
    total_err = ans - guess 
    if(i%10000) == 0:
        print "Error=" + str(np.mean(np.abs(total_err)))
        print cheese_l
        print cheese_w
        print cheese_err
        print cheese_d





