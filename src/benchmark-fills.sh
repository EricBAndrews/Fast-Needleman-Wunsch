#!/bin/zsh

# benchmarking parameters
let "nRuns = 5"
let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"

# programs to benchmark
progs=(serial.e
       sentinel-mt.e
       sentinel-parfill-mt.e
       sentinel-otf-mt.e
       sentinel-nofill-mt.e)

# number of threads to run each program on
threads=(1 # serial
	 8 # sentinel
	 8 # parfill
	 4 # otf
	 4) # nofill

# names for table
tags=(serial
      naive
      parfill
      otf
      nofill)

let "numProgs = ${#progs[@]}"

if [ $numProgs != ${#tags[@]} ]
then
    echo WARNING! programs and tags do not match. please fix.
    exit
fi

if [ $numProgs != ${#threads} ]
then
    echo WARNING! programs and threads do not match. please fix.
    exit
fi

# check results.tsv doesn't exist
if [[ -f fill-techniques.tsv ]]
then
    echo WARNING! fill-techniques.tsv already exists. please rename or remove.
    exit
fi

echo benchmarking fill techniques >> fill-techniques.tsv
echo -n "program\t" >> fill-techniques.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n "$i"gb"\t" >> fill-techniques.tsv
done

echo >> fill-techniques.tsv

for ((i = 1; i <= $numProgs; i++))
do
    echo benchmarking ${progs[$i]} "("${threads[$i]} threads, tagged ${tags[$i]}")"
    echo -n ${tags[$i]}"\t" >> fill-techniques.tsv
    export OMP_NUM_THREADS=${threads[$i]}
    for ((j = minSize; j <= maxSize; j+=$sizeStep))
    do
	echo running on "$j"gb...
	let "s = 0"
	for ((k = 0; k < nRuns; k++))
	do
	    let "a = "$(./${progs[$i]} bdna-link/"$j"gb-1.bdna bdna-link/"$j"gb-2.bdna)""
	    let "s = s + a"
	done
	let "s = s / $nRuns"
	echo -n $s"\t" >> fill-techniques.tsv 
    done
    echo >> fill-techniques.tsv 
done
