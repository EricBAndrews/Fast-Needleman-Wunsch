import matplotlib.pyplot as plt

datadir = "04-06"
# parameters
# dataFileNameShort = "sentinel-nofill-mt"
# dataFileName = datadir + "/" + dataFileNameShort + ".tsv"
# title = dataFileNameShort + " Multithreaded Performance"

dataFileNameShort = "fill-techniques"
title = "Fill Techniques"
dataFileName = datadir + "/" + dataFileNameShort + ".tsv"

xLabel = "Table Size (GB)"
yLabel = "Time (ms)"

# load data
fin = open(dataFileName, 'r')
print("parsing results from " + fin.readline(), end='')
xVals = fin.readline().split()[1:]
xVals = [int(a[:-2]) for a in xVals]

yVals = {}
for line in fin:
    splitLine = line.split()
    yVals[splitLine[0]] = [int(a) for a in splitLine[1:]]

for v in yVals:
    plt.plot(xVals, yVals[v], label=v)

plt.legend()
plt.title(title)
plt.xlabel(xLabel)
plt.ylabel(yLabel)

plt.show()
