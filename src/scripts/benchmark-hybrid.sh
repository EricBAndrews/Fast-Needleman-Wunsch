#!/bin/zsh

# thorough
let "nRuns = 5"
let "minSize = 32"
let "maxSize = 32"
let "sizeStep = 2"
let "minThreads = 2"
let "maxThreads = 10"
let "threadStep = 1"
let "minProcs = 2"
let "maxProcs = 3"
let "procStep = 1"

# testing
# let "nRuns = 1"
# let "minSize=2"
# let "maxSize = 8"
# let "sizeStep = 2"
# let "minThreads = 2"
# let "maxThreads = 4"
# let "threadStep = 1"
# let "minProcs = 2"
# let "maxProcs = 3"
# let "procStep = 1"

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


# check procs-threads.tsv doesn't exist
if [[ -f procs-threads.tsv ]]
then
    echo WARNING! procs-threads.tsv already exists. please rename or remove.
    exit
fi

# set up table
echo benchmarks of $prog >> procs-threads.tsv 
echo -n procs"\t" >> procs-threads.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n "$i"gb"\t" >> procs-threads.tsv
done

echo >> procs-threads.tsv

# benchmark serial

echo -n serial"\t" >> procs-threads.tsv

echo benchmarking serial...
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = $(serial/serial.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
	let "s = s + a"
    done
    let "s = s / $nRuns"
    echo -n $s"\t" >> procs-threads.tsv
done
echo

echo >> procs-threads.tsv 

# benchmark parallel

for ((n = minProcs; n <= maxProcs; n+=$procStep))
do
    for ((m = minThreads; m <= maxThreads; m+=$threadStep))
    do
	export OMP_NUM_THREADS=$m
	echo benchmarking $n procs, $m threads...
	echo -n $n x $m"\t" >> procs-threads.tsv
	for ((i = minSize; i <= maxSize; i+=$sizeStep))
	do
	    let "s = 0"
	    for ((j = 0; j < nRuns; j++))
	    do
		echo -n .
		let "a = $(mpirun --map-by numa -np "$n" ./$prog bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
		let "s = s + a"
	    done
	    echo
	    let "s = s / $nRuns"
	    echo -n $s"\t" >> procs-threads.tsv
	done
	echo >> procs-threads.tsv
    done
done
