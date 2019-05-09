import numpy as np
import random
import sys

"""
Generates synthetic data which is sparse in each sample

-Arguments-
n: num samples
d: num features
Lasso: indicates if this is a Lasso problem - we want to make the coefficient vector sparse
Lasso_sparsity: proportion of coefficients that are nonzero (only used if Lasso = True)
data_sparsity: proportion of features that are nonzero for each sample
problem: regression (r) OR classification (c)
noisy: whether to add white Gaussian noise to the outputs
seed: random seed for reproducing results

-Returns-
b: the vector of labels/outputs
A: the data matrix
x: the true coefficient vector
"""
def gen_synth_sparse_data(n, d, Lasso = False, Lasso_sparsity = 0.1, data_sparsity = 0.1, problem = "regression", noisy = False, seed = 1000):

    num_nonzero = int(d*data_sparsity)
    num_zero = d - num_nonzero

    #Seed for reproducibility
    np.random.seed(seed)
    random.seed(seed)

    #Check if sparsity parameter correct
    if data_sparsity > 1:
        print("Must have sparsity_prop < 1! Try again!")
        exit(0)

    #Generate the sparse data matrix
    A_dense = np.random.normal(0, 1, (n, d))

    mask_arr = np.array([0]*num_zero + [1]*num_nonzero) #create the {0,1} mask matrix to project A onto the appropriate sparsity
    stacked = np.tile(mask_arr, (n, 1))
    mask = shuffle_rows(stacked, seed)

    A = np.multiply(A_dense, mask)

    #Generate the coefficient vector with possible sparsity for Lasso problems
    x = np.random.normal(0, 1, d)

    if Lasso:
        x[random.sample(range(d), int((1-Lasso_sparsity) * d))] = 0 #replace all but Lasso_sparsity fraction of entries with 0

    #Ensure that there are no degenerate (e.g. -0) values
    A[np.abs(A) < 1e-5] = 0
    x[np.abs(x) < 1e-5] = 0

    #Generate the output vector with possible AWGN and binary quantization if the problem is classification
    b = np.dot(A, x)

    if noisy:
        b += np.random.normal(0, 1, n)

    if problem == "c":
        b = np.sign(b)

    return [b, A, x]


"""
Write data to files to use in parallel algorithms

-Arguments-
b: the outputs/labels
A: the data matrix
x: the true coefficient vector
n: the number of samples/rows of A/length of b
d: the number of features/columns of A/length of x
filename: the name of the output text file
"""
def write_data(b, A, x, n, d, filename = "test_data"):
    #Want to save the arrays in the following format: x on first row with pad at end, A under it on LHS, b under on RHS (vertically)

    #Create a shell matrix and fill it with the appropriate quantities
    out = np.zeros((n+1, d+1))
    out[0, 0:d] = x
    out[1:n+1, d] = b
    out[1:n+1, 0:d] = A

    metadata = str(n) + " " + str(d)

    np.savetxt(fname=filename, X=out, fmt="%-1.8f", header=metadata, comments='') #print with left justification, >= 1 character, floating point, 8 fractional digits


"""
Shuffles the elements in each row of a matrix independently

-Arguments-
arr: the array to shuffle
seed: random seed for reproducibility
"""
def shuffle_rows(arr, seed=1000):
    np.random.seed(seed)
    random.seed(seed)

    x, y = arr.shape
    rows = np.indices((x, y))[0] #returns an (x,y) matrix whose elements represent the index of their corresponding rows
    cols = [np.random.permutation(y) for _ in range(x)] #returns an (x,y) matrix where each row is a random permutation of [0,...,y-1]

    return arr[rows, cols]


def main(argv):

    if(not validate_input(argv)):
        print("Enter: <#samples> <#features> <sparsity> <problem type> <filename> <noisy?> <seed>")
        return

    n = int(argv[0])
    d = int(argv[1])

    sparsity = float(argv[2])

    problem_type = argv[3].lower()

    filename = argv[4]

    if(len(argv) >= 6):
        if(argv[5] == "t" or argv[5] == "true"):
            noisy = True
        else:
            noisy = False
    else:
        noisy = False
        seed = 1000

    if(len(argv) == 7):
        seed = int(argv[6])
    else:
        seed = 1000


    b,A,x = gen_synth_sparse_data(n=n, d=d, data_sparsity=sparsity, problem=problem_type, noisy=noisy, seed=seed)

    write_data(b=b, A=A, x=x, n=n, d=d, filename=filename)

    #print("A: ", A, "\n")
    #print("x: ", x, "\n")
    #print("b: ", b, "\n")


def validate_input(s):
    if (len(s) < 5 or len(s) > 7):
        return False

    #check n, d, and sparsity
    try:
        int(s[0])
        int(s[1])
        float(s[2])
    except ValueError:
        return False

    if (float(s[2]) < 0 or float(s[2]) > 1):
        return False

    #check problem type
    acceptable = ["r", "c"]

    if (s[3].lower() not in acceptable):
        return False

    #check filename
    if(not s[4].lower().endswith('.txt')):
        return False

    #If there is a noise parameter check it for accuracy
    if (len(s) >= 6):
        acceptable = ["t", "f", "true", "false"]

        if(s[5].lower() not in acceptable):
            return False

    #If there is a seed parameter check it for accuracy
    if (len(s) == 7):
        try:
            int(s[6])
        except ValueError:
            return False

    return True


if __name__ == "__main__":
    main(sys.argv[1:])