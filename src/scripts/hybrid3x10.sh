#!/bin/zsh

let "nRuns = 5"
let "minSize = 2"
let "maxSize = 64"
let "sizeStep = 2"

echo benchmarking hybrid-vert.e 3 x 10
echo -n hybrid3x10'\t' >> hybrid3x10.tsv

export OMP_NUM_THREADS=10
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun --map-by numa -np 3 hybrid/hybrid-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	# echo -n "$a " >> hybrid3x10.tsv
	let "s = s + a"
    done
    # echo -n "\t" >> hybrid3x10.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> hybrid3x10.tsv
done
echo >> hybrid3x10.tsv 
echo
