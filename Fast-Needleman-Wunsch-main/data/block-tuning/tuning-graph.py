import matplotlib.pyplot as plt
import numpy as np
import argparse

parser = argparse.ArgumentParser(
    description='graph from tsv files'
)

parser.add_argument('-f',
                    type=argparse.FileType('r'),
                    help='data file',
                    required=True)
parser.add_argument('-t',
                    type=str,
                    help='title')
parser.add_argument('-x',
                    type=str,
                    help='x label')
parser.add_argument('-y',
                    type=str,
                    help='y label')
parser.add_argument('-z',
                    type=str,
                    help='z label')
parser.add_argument('--speedup',
                    type=bool,
                    help='graph relative to serial')

args = parser.parse_args() 

# parameters
title = args.t
xLabel = args.x
yLabel = args.y

# load data
fin = args.f
print('parsing results from ' + fin.readline(), end='')
xVals = fin.readline().split()[1:]
xVals = [int(a) for a in xVals]

zMat = []

yVals = []
for line in fin:
    yVals.append(int(line.split()[0]))
    zMat.append([int(a) for a in line.split()[1:]])

def f(x, y):
    return zMat[xVals.index(x)][yVals.index(y)]


X, Y = np.meshgrid(xVals, yVals)
Z = np.array(zMat)
maxTime = max([max(a) for a in zMat])

fig = plt.figure()
ax = plt.axes(projection='3d')
ax.plot_surface(X, Y, Z, cmap='viridis', linewidth=.2)
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
ax.set_zlim([0, maxTime])

plt.title(title)
plt.xlabel(xLabel)
plt.ylabel(yLabel)

plt.show()
