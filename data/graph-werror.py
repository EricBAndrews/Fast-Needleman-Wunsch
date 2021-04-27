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
plotTitle = args.t
# dataFileName = datadir + '/' + dataFileNameShort + '.tsv'
xLabel = args.x
yLabel = args.y

# load data
fin = args.f
print('parsing results from ' + fin.readline(), end='')
xVals = fin.readline().split()[1:]
xVals = [int(a[:-2]) for a in xVals]

yVals = {}
# yErrLarge = {}
# yErrSmall = {}
yErr = {}
for line in fin:
    title = line.split('\t')[0]
    splitLine = [[int(b) for b in a.split()] for a in line.split('\t')[1:-1]]
    # print(splitLine)
    vals = []
    err = [[], []]
    for a in splitLine:
        avg = sum(a) / len(a)
        vals.append(avg)
        err[0].append(abs(min(a) - avg))
        err[1].append(max(a) - avg)
        print('avg:', avg, 'neg', min(a) - avg, 'pos', max(a) - avg)
    yVals[title] = vals
    yErr[title] = err
    # yVals[title] = [sum(a) / len(a) for a in splitLine]
    # yErr[title] = [[max(a) for a in splitLine], [min(a) for a in splitLine]]
    # yErrLarge[title] = [max(a) for a in splitLine]
    # yErrSmall[title] = [min(a) for a in splitLine]

print(yErr)

# adjust to do speedup
if (args.speedup):
    for v in yVals:
        if (v != 'serial'):
            for i in range(len(yVals[v])):
                yVals[v][i] = yVals['serial'][i] / yVals[v][i]
    yVals['serial'] = [1 for i in yVals['serial']]

    for v in yVals:
        plt.plot(xVals, yVals[v], label=v)

else:
    for v in yVals:
        plt.errorbar(xVals, yVals[v],
                     yerr = yErr[title],
                     capsize = 3,
                     label = v)

plt.legend()
plt.title(plotTitle)
plt.xlabel(xLabel)
plt.ylabel(yLabel)

plt.show()
