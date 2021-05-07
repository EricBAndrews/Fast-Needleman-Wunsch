#!/bin/zsh

let "nRuns = 5"
let "minSize = 2"
let "maxSize = 64"
let "sizeStep = 2"

# MPI10

echo benchmarking mpi-vert.e 27
echo -n mpi27'\t' >> mpi27.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun -np 27 mpi/mpi-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	let "s = s + a"
    done
    let "s = s / $nRuns"
    echo -n $s"\t" >> mpi27.tsv
done
echo >> mpi27.tsv
echo
