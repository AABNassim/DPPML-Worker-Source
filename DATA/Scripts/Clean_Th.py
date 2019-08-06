""" This code allows to prepare the dataset bank for the training by replacing
textual values with integers and by multiplying all integers by a certain value

 run python3 Clean_Bank.py

 """


import pandas as pd
import numpy as np
from sklearn import preprocessing


# read the dataset
df = pd.read_csv("data-brute.txt", header=None, sep=',' , low_memory=False)

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:

    if(df[c].dtype == 'int'):
        df[c]= df[c].astype(int)
        df[c]=df[c]*10000
    if(df[c].dtype == 'float'):
        df[c]=round(df[c]*10000)
        df[c]= df[c].astype(int)


name="../CLEAN/thy_"+str(len(df))+"_17.csv"
df.to_csv(name, sep=',', header=False, index=False)



