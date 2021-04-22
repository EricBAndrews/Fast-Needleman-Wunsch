import matplotlib.pyplot as plt
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
parser.add_argument('--speedup',
                    type=bool,
                    help='graph relative to serial')

args = parser.parse_args() 

# parameters
title = args.t
# dataFileName = datadir + '/' + dataFileNameShort + '.tsv'
xLabel = args.x
yLabel = args.y

# load data
fin = args.f
print('parsing results from ' + fin.readline(), end='')
xVals = fin.readline().split()[1:]
xVals = [int(a) for a in xVals]

yVals = {}
for line in fin:
    splitLine = line.split()
    yVals[splitLine[0]] = [int(a) for a in splitLine[1:]]

# adjust to do speedup
if (args.speedup):
    for v in yVals:
        if (v != 'serial'):
            for i in range(len(yVals[v])):
                yVals[v][i] = yVals['serial'][i] / yVals[v][i]
    yVals['serial'] = [1 for i in yVals['serial']]

for v in yVals:
    plt.plot(xVals, yVals[v], label=v)

plt.legend()
plt.title(title)
plt.xlabel(xLabel)
plt.ylabel(yLabel)

plt.show()
