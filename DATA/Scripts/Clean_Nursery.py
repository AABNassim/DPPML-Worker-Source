"""

 run python3 Clean_Nursery.py

 """


import pandas as pd



# read the dataset
df = pd.read_csv("data-brute.txt", header=None, sep=',' , low_memory=False)


# Make all attributes integers

for c in df.columns:
    if(df[c].dtype == 'object'):
        df[c]=df[c].str.strip()
        df[c]= df[c].astype('category')
        df[c] = df[c].cat.codes

#Multiply all values by 100 to get a precision of 10^2
for c in df.columns:
    df[c]= df[c].astype(int)


name="../CLEAN/nursery_"+str(len(df))+"_8.csv"
df.to_csv(name, sep=',', header=False, index=False)



