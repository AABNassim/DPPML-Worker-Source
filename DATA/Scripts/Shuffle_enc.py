import pandas as pd
import sys
from sklearn.utils import shuffle
#python3 Shuffle.py ../Datasets/Adult/DO_1.data
filename=sys.argv[1]  #dataset path
line = next(open(filename))


# read the dataset
df = pd.read_csv(filename, skiprows=1, engine='python', header=None, dtype='object')
#print(line)
#print(df)
df = shuffle(df);
#print(df)

#output the train, the test and the result datasets
with open(filename, 'w') as f:
    f.write(line)

with open(filename, 'a') as file:
   df.to_csv(file, sep=',', header=False, index=False)



