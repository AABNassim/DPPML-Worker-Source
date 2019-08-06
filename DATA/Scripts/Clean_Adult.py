""" This code allows to prepare the dataset adult for the training by replacing
textual values with integers and by multiplying all integers by a certain value

run python3 Clean_Adult.py

"""


import pandas as pd
import numpy as np
import math

# create header for dataset

header = ['age','workclass','fnlwgt','education','education-num','marital-status','occupation','relationship',
    'race','sex','capital-gain','capital-loss','hours-per-week','native-country','class']

# read the dataset
df = pd.read_csv("data-brute.txt", header=None, names=header)

# dataset has '?' in it, convert these into NaN
df.replace(' ?', np.nan, inplace = True)

# drop the NaN
df = df.dropna(axis=0, how="any")

#remove the dot from the class attribute cause it considers it as a third attribute
for i in df.index:
    if df.at[i, 'class'].endswith('.'):
        df.at[i, 'class']=df.at[i, 'class'][:-1]

# Make all attributes integers

for c in df.columns:
    if(df[c].dtype == 'object'):
        df[c]=df[c].str.strip()
        df[c]= df[c].astype('category')
        df[c] = df[c].cat.codes

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
        df[c]= df[c].astype(int)

for i in df.index:
    df.at[i,'fnlwgt']= round(df.at[i,'fnlwgt']/10000)
    df.at[i,'capital-gain']=round(df.at[i,'capital-gain']/100)


name="../CLEAN/adult_"+str(len(df))+"_15.csv"
df.to_csv(name, sep=',', header=False, index=False)



