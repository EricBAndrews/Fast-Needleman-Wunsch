#!/bin/zsh

# benchmarking parameters
let "nRuns = 5"
let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"
let "threads = 4"

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


# check sizes.tsv doesn't exist
if [[ -f sizes.tsv ]]
then
    echo WARNING! sizes.tsv already exists. please rename or remove.
    exit
fi

echo benchmarking sizes on $prog >> sizes.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n "$i"gb"\t" >> sizes.tsv
done

echo >> sizes.tsv

export OMP_NUM_THREADS=$threads
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo running on "$i"gb...
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(./"$prog" bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	let "s = s + a"
    done
    let "s = s / $nRuns"
    echo -n $s"\t" >> sizes.tsv 
done
echo >> sizes.tsv 

