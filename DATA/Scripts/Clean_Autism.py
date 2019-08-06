""" This code allows to prepare the dataset autism for the training by replacing
textual values with integers and by multiplying all integers by a certain value

run python3 Clean_Autism.py

"""


import pandas as pd
import numpy as np
from sklearn import preprocessing

# create header for dataset

header = ['A1_Score','A2_Score','A3_Score','A4_Score','A5_Score','A6_Score','A7_Score','A8_Score',
    'A9_Score','A10_Score','age','gender','ethnicity','jundice','austim','contry_of_res','used_app_before','result','age_desc','relation','class']

# read the dataset
df = pd.read_csv("data-brute.txt", header=None, names=header)

# dataset has '?' in it, convert these into NaN
df.replace('?', '0', inplace = True)



# Make all attributes integers

for c in df.columns:
    if(df[c].dtype == 'object'):
        df[c]=df[c].str.strip()
        df[c]= df[c].astype('category')
        df[c] = df[c].cat.codes

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
        df[c]= df[c].astype(int)


name="../CLEAN/autism_"+str(len(df))+"_21.csv"
df.to_csv(name, sep=',', header=False, index=False)



