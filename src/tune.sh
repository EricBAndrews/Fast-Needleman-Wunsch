#!/bin/zsh

let "minN = 16"
let "maxN = 256"
let "minM = 16"
let "maxM = 256"
let "blockStep = 16"

let "minSize = 2"
let "maxSize = 16"
let "sizeStep = 2"


export OMP_NUM_THREADS=4

for ((i = $minSize; i <= $maxSize; i += $sizeStep))
do
    echo benchmarking "$i"gb
    # check results.tsv doesn't exist
    if [[ -f blocktune"$i".tsv ]]
    then
	echo WARNING! blocktune"$i".tsv already exists. please rename or remove.
	exit
    fi

    echo "$i"gb >> blocktune"$i".tsv
    echo -n dims'\t'  >> blocktune"$i".tsv
    for ((M = $minM; M <= $maxM; M += $blockStep))
    do
	echo -n $M'\t'  >> blocktune"$i".tsv
    done
    echo  >> blocktune"$i".tsv

    for ((N = $minN; N <= $maxN; N += $blockStep))
    do
	echo -n $N'\t'  >> blocktune"$i".tsv
	for ((M = $minM; M <= $maxM; M += $blockStep))
	do
	    echo -n .
	    # take best of two runs to deal with outlier runs
	    let "run1 = $(./sentinel-ob-tune.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna $N $M)"
	    let "run2 = $(./sentinel-ob-tune.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna $N $M)"
	    if [[ $run1 < $run2 ]]
	    then
	       	echo -n $run1'\t'  >> blocktune"$i".tsv
	    else
		echo -n $run2'\t'  >> blocktune"$i".tsv
	    fi
	done
	echo >> blocktune"$i".tsv
	echo
    done
    echo
done


# echo benchmarking 4gb...
# ./sentinel-ob-tune.e bdna-link/4gb-1.bdna bdna-link/4gb-2.bdna > 4.tsv

# echo benchmarking 8gb...
# ./sentinel-ob-tune.e bdna-link/8gb-1.bdna bdna-link/8gb-2.bdna > 8.tsv

# echo benchmarking 12gb...
# ./sentinel-ob-tune.e bdna-link/12gb-1.bdna bdna-link/12gb-2.bdna > 12.tsv

# echo benchmarking 16gb...
# ./sentinel-ob-tune.e bdna-link/16gb-1.bdna bdna-link/16gb-2.bdna > 16.tsv
