""" This code allows to prepare the dataset vote for the training by replacing
textual values with integers and by multiplying all integers by a certain value

run python3 Clean_Vote.py

"""


import pandas as pd
import numpy as np
from sklearn import preprocessing

# create header for dataset



# read the dataset
df = pd.read_csv("data-brute.txt", header=None)

# dataset has '?' in it, convert these into NaN
df.replace('?', np.nan, inplace = True)

# drop the NaN
df = df.dropna(axis=0, how="any")

# Make all attributes integers

for c in df.columns:
    if(df[c].dtype == 'object'):
        df[c]=df[c].str.strip()
        df[c]= df[c].astype('category')
        df[c] = df[c].cat.codes

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
        df[c]= df[c].astype(int)
        df[c]=df[c]*100


name="../CLEAN/vote_"+str(len(df))+"_15.csv"
df.to_csv(name, sep=',', header=False, index=False)



