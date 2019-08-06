import pandas as pd
import sys
from sklearn.utils import shuffle
#python3 Shuffle.py ../Datasets/Adult/DO_1.data
filename=sys.argv[1]  #dataset path

# read the dataset
df = pd.read_csv(filename, header=None, engine='python', dtype='object')
df = shuffle(df);


#output the train, the test and the result datasets

df.to_csv(filename, sep=',', header=False, index=False)



