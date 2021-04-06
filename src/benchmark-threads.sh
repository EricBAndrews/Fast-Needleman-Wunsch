#!/bin/zsh

let "nRuns = 5"
let "maxSize = 16"
let "maxThreads = 8"

if [ $# -eq 0 ]
then
    echo WARNING! no filename supplied. using sentinel-mt.e
    prog="sentinel-mt.e"
    # prog="sentinel-parfill-mt.e"
    # prog="sentinel-otf-mt.e"
    # prog="sentinel-nofill-mt.e"
else
    if [ -f $1 ]
    then
	prog=$1
    else
	echo WARNING! bad filename. please try again.
	exit
    fi
fi


# check results.tsv doesn't exist
if [[ -f results.tsv ]]
then
    echo WARNING! results.tsv already exists. please rename or remove.
    exit
fi

# set up table
echo benchmarks of $prog >> results.tsv 
echo -n threads"\t" >> results.tsv

for ((i = 1; i <= maxSize; i++))
do
    echo -n "$i"gb"\t" >> results.tsv
done

echo >> results.tsv

# benchmark serial

echo -n serial"\t" >> results.tsv

echo benchmarking serial...
for ((i = 1; i <= maxSize; i++))
do
    echo running on "$i"gb...
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = $(./serial.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
	let "s = s + a"
    done
    let "s = s / $nRuns"
    echo -n $s"\t" >> results.tsv
done

echo >> results.tsv 

# benchmark parallel

for ((n = 1; n <= maxThreads; ++n))
do
    echo benchmarking $n threads...
    export OMP_NUM_THREADS=$n
    echo -n $n"\t" >> results.tsv
    for ((i = 1; i <= maxSize; i++))
    do
	echo running on "$i"gb...
	let "s = 0"
	for ((j = 0; j < nRuns; j++))
	do
	    let "a = $(./$prog bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)"
	    echo $a
	    let "s = s + a"
	done
	let "s = s / $nRuns"
	echo -n $s"\t" >> results.tsv
    done
    echo >> results.tsv 
done
