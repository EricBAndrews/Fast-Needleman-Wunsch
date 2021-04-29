#!/bin/zsh

# thorough
# let "nRuns = 5"
# let "minSize = 2"
# let "maxSize = 16"
# let "sizeStep = 2"
# let "minProcs = 2"
# let "maxProcs = 8"
# let "procStep = 2"

# fast(er)
let "nRuns = 3"
let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"
let "minProcs = 2"
let "maxProcs = 30"
let "procStep = 2"

# testing
# let "nRuns = 1"
# let "minSize=2"
# let "maxSize = 8"
# let "sizeStep = 2"
# let "minProcs = 4"
# let "maxProcs = 8"
# let "procStep = 4"

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


# check procs.tsv doesn't exist
if [[ -f procs.tsv ]]
then
    echo WARNING! procs.tsv already exists. please rename or remove.
    exit
fi

# set up table
echo benchmarks of $prog >> procs.tsv 
echo -n procs"\t" >> procs.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n "$i"gb"\t" >> procs.tsv
done

echo >> procs.tsv

# benchmark serial

echo -n serial"\t" >> procs.tsv

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
    echo -n $s"\t" >> procs.tsv
done

echo >> procs.tsv 

# benchmark parallel

for ((n = minProcs; n <= maxProcs; n+=$procStep))
do
    echo benchmarking $n procs...
    echo -n $n"\t" >> procs.tsv
    for ((i = minSize; i <= maxSize; i+=$sizeStep))
    do
	echo running on "$i"gb...
	let "s = 0"
	for ((j = 0; j < nRuns; j++))
	do
	    let "a = $(mpirun -np "$n" ./$prog bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
	    let "s = s + a"
	done
	let "s = s / $nRuns"
	echo -n $s"\t" >> procs.tsv
    done
    echo >> procs.tsv 
done
