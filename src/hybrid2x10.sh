#!/bin/zsh

let "nRuns = 5"
let "minSize = 2"
let "maxSize = 64"
let "sizeStep = 2"

echo benchmarking hybrid-vert.e 2 x 10
echo -n hybrid2x10'\t' >> hybrid2.tsv

export OMP_NUM_THREADS=10
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun --map-by numa -np 2 hybrid/hybrid-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	# echo -n "$a " >> hybrid2.tsv
	let "s = s + a"
    done
    # echo -n "\t" >> hybrid2.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> hybrid2.tsv
done
echo >> hybrid2.tsv 
echo
