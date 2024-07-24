import sys
import pandas as pd


"""
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
    df = df.sort_values(by=0)
    df = df.drop(df.columns[0], axis=1)
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
    eps = float(args[3])
    if eps <= 0:
        print("Invalid epsilon!")
        return -1, 0, 0
    
    path1 = args[4] if len(args) == 5 else args[5]
    path2 = args[5] if len(args) == 6 else args[6]
    df = parseDbs(path1, path2)

    return K, iter, eps, df


"""
Runs main logic of file

@rtype: int
@returns: 0 if program runs successfully else 1.

"""
def main():
    K, iter, eps, data = verifyData(sys.argv)
    if K == -1:
        return 1

    print(data.to_string(index=False, header=False))
    return 0






if __name__ == "__main__":
    main()