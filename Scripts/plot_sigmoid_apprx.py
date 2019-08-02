from math import *
import matplotlib.pyplot as plt
import numpy as np

def sigmoid(x):
    return 1 / (1 + exp(-x))
def poly_sigmoid_deg3(x):
    #return 0.5 - 0.15012 * x + 0.0159230078125 * x * x * x
    return 0.5 + 1.20096 * x / 8 - 0.81562 * (x / 8) ** 3
def poly_sigmoid_deg5(x):
    return 0.5 + 1.53048 * x / 8 - 2.3533056 * (x / 8) ** 3 + 1.3511295 * (x / 8) ** 5

 
x = np.linspace(-8, 8, 160)
print(x)
sigmoids = [sigmoid(v) for v in x]
poly_sigmoids_deg3 = [poly_sigmoid_deg3(v) for v in x]
poly_sigmoids_deg5 = [poly_sigmoid_deg5(v) for v in x]


fig = plt.figure()
plt.plot(x, sigmoids, label='Sigmoid')
plt.plot(x, poly_sigmoids_deg3, label='Sigmoid Approx. Degree 3')
plt.plot(x, poly_sigmoids_deg5, label='Sigmoid Approx. Degree 5')

plt.xlabel('x')
plt.ylabel('y')

plt.title('Sigmoid vs Polynomial approx. of Sigmoid')

plt.legend()
plt.show()
