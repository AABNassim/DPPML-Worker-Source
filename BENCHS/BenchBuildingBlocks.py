import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


df = pd.read_csv('test.csv', delimiter='\t')
print(df['ExecTime'])

fig = plt.figure()
df.plot()