""" This code allows to prepare the dataset bank for the training by replacing
textual values with integers and by multiplying all integers by a certain value

 run python3 Clean_Bank.py

 """


import pandas as pd



# read the dataset
df = pd.read_csv("data-brute.txt", header=None, sep=';' , low_memory=False)

# replace -1 by 0 (no pdays)
df.replace('-1', '0', inplace = True)

# Make all attributes integers

for c in df.columns:
    if(df[c].dtype == 'object'):
        df[c]=df[c].str.strip()
        df[c]= df[c].astype('category')
        df[c] = df[c].cat.codes

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
    df[c]= df[c].astype(int)


df[5] = df[5].apply(lambda x: round(x/100))

df[11] = df[11].apply(lambda x: round(x/100))

name="../CLEAN/bank_"+str(len(df))+"_17.csv"
df.to_csv(name, sep=',', header=False, index=False)



