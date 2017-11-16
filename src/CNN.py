import ScoreCNN
import numpy as np
import sys, csv
import os

#a sigmoid function to find probability
def nonlin(x, deriv=False):


    if(deriv==True):
        return x*(1-x)
    #this statment puts a value between 0 and 1
    return 1/(1+np.exp(-x))

def ans(inputs, in_weights, out_weights):
    #first set up the input array dimensions to multiply by the wieghts
    input_t = np.array(inputs, ndmin=2)

    #then set up the hidden layer values
    hidden_layer = np.dot(input_t, in_weights)
    layer_out = nonlin(hidden_layer)
    #print the output guess for debug
    print layer_out
 
    #finally boil it down into the output values and use the sigmoid for %chance
    hidden_out  = np.dot(layer_out, out_weights)   
    output_vals = nonlin(hidden_out)
   
    return nonlin(output_vals)


def train_step(inputs, out_weights, in_weights, ans):
    #learning rat to adjust how quickly data will change the hidden layer
    learning_rate = .2
    #the start is the same as finding the ans
    #first set up the input array dimensions to multiply by the wieghts
    input_t = np.array(inputs, ndmin=2)

    #then set up the hidden layer values
    hidden_layer = np.dot(input_t, in_weights)
    layer_out = nonlin(hidden_layer)
 
    #finally boil it down into the output values and use the sigmoid for %chance
    hidden_out  = np.dot(layer_out, out_weights)   
    output_vals = nonlin(hidden_out)

    output_err = ans - output_vals
    layer_err = np.dot(out_weights.T, output_err)

    #update out weights accodring to the previous weight and the partial derivative with respect to error
    out_weights += learning_rate * np.dot((output_err * output_vals * (1.0 - output_vals)), np.transpose(hidden_out))
    in_weights += learning_rate * np.dot(np.transpose(input_t), (layer_err * hidden_out * (1.0 - hidden_out)))

    pass


#set weight valus, wieght set to try and implicate base importance
np.random.seed(1)
input_weights = np.random.random_sample((5, 4))
output_weights = np.random.random_sample((4, 1))
test_files = os.listdir(sys.argv[1])

#run the training loop, uses the end game state of every game to get a hard training answer
for i in test_files:
    ans_in = ScoreCNN.get_output("test_case/"+i, -1)
    data_in = ScoreCNN.get_input("test_case/"+i, -1)
    train_step(data_in, output_weights, input_weights, ans_in)

#test the wieghts against predictions
#testing for a cheese prediction
print ("Cheese-timing-all_in-economy")
data_in = np.array([1, 0, 0, 0, 0])
print ans(data_in, input_weights, output_weights)
#testing for an all in prediction
data_in = np.array([0, 1, 0, 0, 0])
print ans(data_in, input_weights, output_weights)
#testing for a timing prediction
data_in = np.array([0, 1, 0, 0, 0])
print ans(data_in, input_weights, output_weights)
#testing for economy
data_in = np.array([0, 0, 1, 0, 0])
print ans(data_in, input_weights, output_weights)








