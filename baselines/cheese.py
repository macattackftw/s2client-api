import sys

def main():
    files = ('/home/kyle/Downloads/StarCraftII/Replays/PvZ/43038 3.16.1 560180215.000000_dtw.csv',
             '/home/kyle/Downloads/StarCraftII/Replays/PvZ/24941 3.16.1 560180215.000000_dtw.csv',
             '/home/kyle/Downloads/StarCraftII/Replays/PvZ/10326 3.16.1 560180215.000000_dtw.csv',
             '/home/kyle/Downloads/StarCraftII/Replays/PvZ/4827 3.16.1 560180197.000000_dtw.csv',
             '/home/kyle/Downloads/StarCraftII/Replays/PvZ/26386 3.16.1 560180215.000000_dtw.csv')
             
    k_features = 0
    with open(files[0], "r") as file:
        k_features = len(file.readline().split(','))

    data = []
    # Read each csv file and add the list of list of strings to data
    for csv in files:
        with open(csv, "r") as file:
            # ignore headers
            lines = file.read().splitlines()[1:]
            new_lst = []
            for line in lines:
                new_lst.append([float(_) for _ in line.split(',')])
            data.append(new_lst)

    min = sys.maxsize
    for d in data:
        if len(d) < min:
            min = len(d)

    newFile = [[0.0]*k_features for _ in range(min)]
    for row in range(min):
        count = 0
        for file in range(len(files)):
            if (row < len(data[file])):
                for col in range(k_features):
                    newFile[row][col] = newFile[row][col] + data[file][row][col]
                count = count + 1
        for col in range(k_features):
            newFile[row][col] = newFile[row][col] / count
        count = 0
    with open("cheese_baseline.csv", "w") as file:
        file.write("Seconds,ArmyVal,MinRate,GasRate,StructVal,UpgMin,UpgGas,StructPerim,ArmyDist\n")
        for i in range(min):
            for j in range(k_features):
                file.write(str(newFile[i][j]) + ",")
            file.write("\n")























if __name__ == '__main__':
    main()
