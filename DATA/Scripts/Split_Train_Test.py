""" This code splits the dataset given as a parameter into a train set and a test set according to a given percentage
run example : python3 Split_Train_Test.py ../Datasets/SIS/ ../Datasets/SIS/data-clean 0.2

"""
# Haven't tested this code on big dataset

import pandas as pd
import numpy as np
import sys
from sklearn.cross_validation import train_test_split


path=sys.argv[1]  # path of the dataset to be read
filename=sys.argv[2]  #dataset path without .csv
p=sys.argv[3]  # percentage of the test dataset

# read the dataset
df = pd.read_csv(filename+".csv", header=None)

X = df[df.columns[:-1]]
Y = df[df.columns[-1]]

# Split the dataset
X_train, X_test, y_train, y_test = train_test_split(X, Y, test_size=float(p), random_state=1)

#Merge the vectors and class labels in the case of the trainset
X_train['4']=y_train
#X_test['class']=y_test

#output the train, the test and the result datasets

X_train.to_csv(path+"training.data", sep=',', header=False, index=False)
X_test.to_csv(path+"testing.data", sep=',', header=False, index=False)
y_test.to_csv(path+"result.data", sep=',', header=False, index=False)


