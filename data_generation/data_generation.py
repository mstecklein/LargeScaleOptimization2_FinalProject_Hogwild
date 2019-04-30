import numpy as np
import random

"""
Generates synthetic data which is sparse in each sample

-Arguments-
n: num samples
d: num features
Lasso: indicates if this is a Lasso problem - we want to make the coefficient vector sparse
Lasso_sparsity: proportion of coefficients that are nonzero (only used if Lasso = True)
data_sparsity: proportion of features that are nonzero for each sample
problem: regression OR classification
noisy: whether to add white Gaussian noise to the outputs
seed: random seed for reproducing results

-Returns-
b: the vector of labels/outputs
A: the data matrix
x: the true coefficient vector
"""
def gen_synth_sparse_data(n, d, Lasso = False, Lasso_sparsity = 0.1, data_sparsity = 0.1, problem = "regression", noisy = True, seed = 1000):

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

    if problem == "classification":
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
"""
def shuffle_rows(arr, seed=1000):
    np.random.seed(seed)
    random.seed(seed)

    x, y = arr.shape
    rows = np.indices((x, y))[0] #returns an (x,y) matrix whose elements represent the index of their corresponding rows
    cols = [np.random.permutation(y) for _ in range(x)] #returns an (x,y) matrix where each row is a random permutation of [0,...,y-1]

    return arr[rows, cols]

def main():
    n = 4
    d = 4

    seed = 2240

    lasso = True
    lasso_sparsity = 0.75
    sparsity = 0.25

    type = "regression"

    noisy = True

    b,A,x = gen_synth_sparse_data(n=n, d=d, Lasso=lasso, Lasso_sparsity=lasso_sparsity, data_sparsity=sparsity, problem=type, noisy=noisy, seed=seed)

    write_data(b=b, A=A, x=x, n=n, d=d, filename="Test.txt")

    print("A: ", A, "\n")
    print("x: ", x, "\n")
    print("b: ", b, "\n")


if __name__ == "__main__":
    main()








