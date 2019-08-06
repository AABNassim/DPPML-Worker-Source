""" This code allows to prepare the dataset SIS for the training by multiplying all integers by a certain value
run python3 Clean_SIS.py

"""


import pandas as pd
import numpy as np
from sklearn import preprocessing


# read the dataset
df = pd.read_csv("data-brute.txt", header=None)

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
        df[c]=df[c]*100

name="../CLEAN/SIS_"+str(len(df))+"_15.csv"
df.to_csv(name, sep=',', header=False, index=False)



