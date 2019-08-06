"""

 run python3 Clean_Nursery.py

 """


import pandas as pd



# read the dataset
df = pd.read_csv("brute.data", header=None, sep=',' , low_memory=False)


# Make all attributes integers

for c in df.columns:
    if(df[c].dtype == 'object'):
        df[c]=df[c].str.strip()
        df[c]= df[c].astype('category')
        df[c] = df[c].cat.codes

    if(df[c].dtype == 'float'):
        df[c]=round(df[c]*10)
        df[c]= df[c].astype(int)

    df[c]= df[c].astype(int)


name="../CLEAN/iris_"+str(len(df))+"_8.csv"
df.to_csv(name, sep=',', header=False, index=False)



