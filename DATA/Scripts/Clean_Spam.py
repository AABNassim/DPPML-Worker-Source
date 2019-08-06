""" This code allows to prepare the dataset spam for the training by multiplying all floats by a certain value
run python3 Clean_Spam.py
"""


import pandas as pd
import numpy as np
from sklearn import preprocessing

# read the dataset
df = pd.read_csv("data-brute.txt", header=None)

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
    if(df[c].dtype == 'int'):
        df[c]= df[c].astype(int)
        df[c]=df[c]*100
    if(df[c].dtype == 'float'):
        df[c]=round(df[c]*100)
        df[c]= df[c].astype(int)

name="../CLEAN/spam_"+str(len(df))+"_57.csv"
df.to_csv(name, sep=',', header=False, index=False)



