#!/bin/zsh

let "minBuf = 128"
let "maxBuf = 512" # 4096
let "bufStep = 128"

let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"

prog="mpi/mpi-horz-tune.e"

for ((i = $minSize; i <= $maxSize; i += $sizeStep))
do
    echo benchmarking "$i"gb
    # check results.tsv doesn't exist
    if [[ -f buftune"$i".tsv ]]
    then
	echo WARNING! buftune"$i".tsv already exists. please rename or remove.
	exit
    fi

    echo "$i"gb >> buftune"$i".tsv
    echo -n bufsize'\t'  >> buftune"$i".tsv
    for ((B = $minBuf; B <= $maxBuf; B += $bufStep))
    do
	echo -n $B'\t'  >> buftune"$i".tsv
    done
    echo  >> buftune"$i".tsv
    echo -n time'\t' >> buftune"$i".tsv

    for ((B = $minBuf; B <= $maxBuf; B += $bufStep))
    do
	echo -n .
	# take best of two runs to deal with outlier runs
	let "run1 = $(mpirun -np 4 "$prog" bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna $B)"
	let "run2 = $(mpirun -np 4 "$prog" bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna $B)"
	if [[ $run1 < $run2 ]]
	then
	    echo -n $run1'\t'  >> buftune"$i".tsv
	else
	    echo -n $run2'\t'  >> buftune"$i".tsv
	fi
    done
    echo >> buftune"$i".tsv
    echo
done
