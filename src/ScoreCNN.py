#This module takes a csv file and finds strategy percentages based on wieghted inputs, this will be used as the score for the neural net
import numpy as np
import sys, csv

#this function takes a game state of 5 variables and based on the wieghts gives the likelyhood of the 4 main strats
def classify(bases, gas, army, struct, time):
    #equations change drastically when a base is built, this easiest way is to split into 4 base catagories since anyhting after 4 is flat out economic
    if(bases < 2):
	#min maxes to control how much a variable can affect judgement and the devides control importance of the variable
        timing = 30 + min(gas/15, 10) + min(army/200, 10) + min((time-60)/15, 10) - min((time-60)/15, 30)
        all_in = 10 + min(army/50, 50) - min(struct/100, 10) + min((time-60)/15, 20) + min(max((time-120)/10, 0), 20)
        economy = 0
        cheese = 100 - timing - all_in

    elif(bases == 2):
        timing = 40 + min(gas/30, 20) + min(army/200, 10) + min((time-120)/20, 10) + max(min((4 - (time-360)/30), 0), -15)
        all_in = 40 + min(gas/50, 10) + min(army/200, 30) - min(struct/150, 15) + min((time-360)/30, 20)
        economy = min(time/60, 20) + max(struct/150, 15) - min(army/100, 40)
        cheese = 100 - timing - all_in - economy

    elif(bases == 3):
        timing = 40 + min(gas/60, 10) + min(army/200, 10) + max(-struct/150, -25) - max((time-360)/30, 0)
        all_in = 20 + min(army/250, 15) + max(-struct/150, -15) - max((time-360)/15, 0)
        economy = 40 + max(struct/75, 40) + max((time-360)/10, 0)
        cheese = 100 - timing - all_in - economy

    elif(bases >= 4):
        timing = 10 + min(gas/60, 10) + min(army/200, 30) + max(-struct/150, -25) - min((time-420)/60, 15)
        all_in = 0
        economy = 90 - min(gas/60, 10) - min(army/500, 30) - max(-struct/150, -25) + min((time-420)/60, 100)
        cheese = 100 - timing - all_in - economy

    #This just evens out the percentages if they dont add to 100%, only needed if the variables weights aren't balenced out, probably a todo
    err_t = (timing+all_in+economy)/100
    if(cheese < 0):
        timing = timing + (timing)/err_t*(cheese/100)
        all_in = all_in + (all_in)/err_t*(cheese/100)
        economy = economy + (economy)/err_t*(cheese/100)
        cheese = 0.0
    return cheese, timing, all_in, economy

def get_input(filename):

   with open( filename ) as fin:
        reader = csv.reader( fin )  # read the file with the csv reader
        data = list( reader )       # convert the csv reader object (a generator) to a list (of 
        data.pop(0)
        In_arr = np.empty([len(data), 5])
        count = 0
        for i in data:
            if(int(i[0]) < 2):
                In_arr[count][0]=1
                In_arr[count][1]=0
                In_arr[count][2]=0
            if(int(i[0]) == 2):
                In_arr[count][0]=0
                In_arr[count][1]=1
                In_arr[count][2]=0
            if(int(i[0]) > 2):
                In_arr[count][0]=0
                In_arr[count][1]=0
                In_arr[count][2]=1
            In_arr[count][3]=0
            In_arr[count][4]=0
            count += 1
        return In_arr

    
def get_output(filename):
    # open CSV file (automatically closed at end of with stmt)
    with open( filename ) as fin:
        reader = csv.reader( fin )  # read the file with the csv reader
        data = list( reader )       # convert the csv reader object (a generator) to a list (of lists)
        data.pop(0)
    Out_arr = np.empty([len(data), 4])
    count = 0
    for i in data:
        ch, tm, ai, ec = classify(float(i[0]), float(i[1]), float(i[2]), float(i[4]), float(i[5]))
        ch = float(round(ch,2))
        tm = float(round(tm,2))
        ai = float(round(ai,2)) 
        ec = float(round(ec,2))  
        if(ch > tm and ch > ai and ch > ec):
            Out_arr[count][0] = 1
            Out_arr[count][1] = 0
            Out_arr[count][2] = 0
            Out_arr[count][3] = 0     
        elif(tm > ch and tm > ai and tm > ec):
            Out_arr[count][0] = 0
            Out_arr[count][1] = 1
            Out_arr[count][2] = 0
            Out_arr[count][3] = 0     
        elif(ai > tm and ai > ch and ai > ec):
            Out_arr[count][0] = 0
            Out_arr[count][1] = 0
            Out_arr[count][2] = 1
            Out_arr[count][3] = 0    
        elif(ec > tm and ec > ai and ec > ch):
            Out_arr[count][0] = 0
            Out_arr[count][1] = 0
            Out_arr[count][2] = 0
            Out_arr[count][3] = 1     
        count += 1

    return Out_arr






















