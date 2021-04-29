#!/bin/zsh

# thorough
# let "nRuns = 5"
# let "minSize = 2"
# let "maxSize = 16"
# let "sizeStep = 2"
# let "minThreads = 2"
# let "maxThreads = 8"
# let "threadStep = 2"

# fast(er)
let "nRuns = 3"
let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"
let "minThreads = 2"
let "maxThreads = 32"
let "threadStep = 2"

# testing
# let "nRuns = 1"
# let "minSize=2"
# let "maxSize = 8"
# let "sizeStep = 2"
# let "minThreads = 4"
# let "maxThreads = 8"
# let "threadStep = 4"

if [ $# -eq 0 ]
then
    echo WARNING! no filename supplied. using sentinel-mt.e
    prog="sentinel-mt.e"
else
    if [ -f $1 ]
    then
	prog=$1
    else
	echo WARNING! bad filename. please try again.
	exit
    fi
fi


# check threads.tsv doesn't exist
if [[ -f threads.tsv ]]
then
    echo WARNING! threads.tsv already exists. please rename or remove.
    exit
fi

# set up table
echo benchmarks of $prog >> threads.tsv 
echo -n threads"\t" >> threads.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n "$i"gb"\t" >> threads.tsv
done

echo >> threads.tsv

# benchmark serial

echo -n serial"\t" >> threads.tsv

echo benchmarking serial...
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo running on "$i"gb...
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = $(serial/serial.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
	let "s = s + a"
    done
    let "s = s / $nRuns"
    echo -n $s"\t" >> threads.tsv
done

echo >> threads.tsv 

# benchmark parallel

for ((n = minThreads; n <= maxThreads; n+=$threadStep))
do
    echo benchmarking $n threads...
    export OMP_NUM_THREADS=$n
    echo -n $n"\t" >> threads.tsv
    for ((i = minSize; i <= maxSize; i+=$sizeStep))
    do
	echo running on "$i"gb...
	let "s = 0"
	for ((j = 0; j < nRuns; j++))
	do
	    let "a = $(./$prog bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
	    let "s = s + a"
	done
	let "s = s / $nRuns"
	echo -n $s"\t" >> threads.tsv
    done
    echo >> threads.tsv 
done
