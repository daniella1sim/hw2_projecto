import sys
import pandas as pd
import numpy as np
from kmeans_module import kmeans

""""
Verifies argument is an int
@type arg: String
@param arg: A string of input from user to verify
@rtype: Touple(int, int)
@returns: First int return 1 if arg is not an integer and 1 if it is, second returns the arg in integer form.
"""
def isInt(arg):
    try:
        return 0, int(arg)
    except:
        try:
            float_arg = float(arg)
            int_arg = int(arg[0: arg.find(".")])
            if (float_arg) == int_arg:
                return 0, int_arg
            else:
                return 1, 0
        except:
            return 1, 0


"""
Parses two csv files and merges them on the first column.
@type path1: String
@param path1: A string of input from user of a txt or csv file to parse
@type path2: String
@param path2: A string of input from user of a txt or csv file to parse
@rtype: Dataframe
@returns: Retruns a dataframe of the two csv files merged on the first column.
"""
def parseDbs(path1, path2):
    df1 = pd.read_csv(path1, delimiter=',', header=None)
    df2 = pd.read_csv(path2, delimiter=',', header=None)
    df = pd.merge(df1, df2, left_on=0, right_on=0, how='inner')
    df.sort_values(by=0, inplace=True)
    return df


"""
Verifies argument validity: correct number of arguments, correct input for K, iter, file.
Parses file from txt file to a 2d matrix of coordinates.
@type args: List[String]
@param args: List of arguments from user input: K - number of clusters, iter - number of iterations, optional, eps - error margin, file1 - csv or txt, file2 - csv or txt
@rtype: Tuple[int, int, float, DataFrame]
@returns: Tuple[0] = k, List[1] = iter, List[2] = eps,List[3] = data OR [-1,0,0] if error.
"""
def verifyData(args):
    if len(args) not in [5, 6]:
        print("An error has occurred!")
        return -1, 0, 0
    err_K, K = isInt(args[1])
    if len(args) == 5:
        iter = 200
    else:
        err_iter, iter = isInt(args[2])
        if iter >= 1000 or iter <= 0 or err_iter:
            print("Invalid maximum iteration!")
            return -1, 0, 0
    eps = float(args[2]) if len(args) == 5 else float(args[3])
    if eps <= 0:
        print("Invalid epsilon!")
        return -1, 0, 0
    
    path1 = args[3] if len(args) == 5 else args[4]
    path2 = args[4] if len(args) == 5 else args[5]
    df = parseDbs(path1, path2)

    return K, iter, eps, df


'''
Calculates the distance between two points
@type row: List
@param row: A list of floats representing a row in the dataframe
@type random_row: List
@param random_row: A list of floats representing a random row in the dataframe
@rtype: float
@returns: The distance between the two row
'''
def calculate_distance(row, random_row):
    row = np.array(row)
    random_row = np.array(random_row)    
    return np.linalg.norm(row - random_row)


"""
Runs the kmeans++ algorithm to find the initial centroids of the clusters
@type data: DataFrame
@param data: A dataframe of the two input files merged on the first column
@type K: int
@param K: The number of clusters to find
@rtype: List[List[float]]
@returns: A list of lists of floats representing the centroids of the clusters
"""
def kmeanspp(data, K):
    centroidList = []
    indexList = []
    df = data.copy()
    np.random.seed(1234)
    random_index = np.random.choice(df.index)
    indexList.append(int(df.loc[random_index, 0]))
    random_line = df.loc[random_index]  
    random_line_list = random_line.tolist()
    centroidList.append(random_line_list)
    df.drop(random_index, inplace = True)
    
    N  = df.shape[0]
    df["Distance"] = [0.0 for i in range(N)]
    
    for i in range(K - 1):
        for index in df.index:
            min_distance = float('inf')
            for centroid in centroidList:
                row = df.loc[index, df.columns[1:-1]].to_list()
                distance = calculate_distance(row, centroid[1:])
                if distance < min_distance:
                    min_distance = distance
            df.loc[index ,"Distance"] = min_distance
        probability = df["Distance"]
        probability = probability / probability.sum()
        random_index = np.random.choice(df.index, p=probability)
        random_line = df.loc[random_index]
        indexList.append(int(df.loc[random_index, 0]))
        centroidList.append(random_line.tolist()[:-1])
        df.drop(random_index, inplace = True)

    indexStr = ",".join([str(int(i)) for i in indexList])
    print(indexStr)
    df.drop(df.columns[0], axis=1, inplace=True)  
    return indexList


"""
Runs main logic of file

@rtype: int
@returns: 0 if program runs successfully else 1.

"""
def main():
    K, iter, eps, data = verifyData(sys.argv)
    if K == -1:
        return 1

    centroidList= kmeanspp(data, K)
    data.pop(0)
    data = data.values.tolist()
    #centroids = kmeans(iter, eps, data, centroidList)
    #print("ahahah")
    #for centroid in centroids:
    #    target = ",".join([str(round(i, 4)) for i in centroid])
    #    print(target)

    return 0



if __name__ == "__main__":
    main()
