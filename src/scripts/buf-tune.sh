#!/bin/zsh

let "minBuf = 128"
let "maxBuf = 4096"
let "bufStep = 128"
let "nRuns = 5"

let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"

prog="mpi/mpi-horz-tune.e"

for ((i = $minSize; i <= $maxSize; i += $sizeStep))
do
    # check results.tsv doesn't exist
    if [[ -f buftune"$i".tsv ]]
    then
	echo WARNING! buftune"$i".tsv already exists. please rename or remove.
	exit
    fi
done

for ((i = $minSize; i <= $maxSize; i += $sizeStep))
do
    echo benchmarking "$i"gb

    echo "$i"gb >> buftune"$i".tsv
    echo -n bufsize"\t"  >> buftune"$i".tsv
    for ((B = $minBuf; B <= $maxBuf; B += $bufStep))
    do
	echo -n $B"\t"  >> buftune"$i".tsv
    done
    echo  >> buftune"$i".tsv
    echo -n time'\t' >> buftune"$i".tsv

    for ((B = $minBuf; B <= $maxBuf; B += $bufStep))
    do
	echo -n .
	let "sum = 0"
	for ((j = 0; j < $nRuns; ++j))
	do
	    let "a = $(mpirun -np 8 "$prog" bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna $B)"
	    let "sum = sum + a"
	done
	let "sum = sum / nRuns"
	echo -n $sum"\t" >> buftune"$i".tsv
    done
    echo >> buftune"$i".tsv
    echo
done
