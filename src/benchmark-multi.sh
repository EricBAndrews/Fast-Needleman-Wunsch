#!/bin/zsh

# benchmarking parameters
# thorough
let "nRuns = 5"
let "minSize = 2"
let "maxSize = 64"
let "sizeStep = 2"

# fast
# let "nRuns = 2"
# let "minSize = 4"
# let "maxSize = 16"
# let "sizeStep = 4"

# testing
# let "nRuns = 3"
# let "minSize = 2"
# let "maxSize = 6"
# let "sizeStep = 2"

# programs to benchmark:
# serial.e
# sentinel-otf-blocked.e (30 threads)
# mpi-vert.e (30 processes)
# hybrid-vert.e (3 processes x 10 threads)


# check multi.tsv doesn't exist
if [[ -f multi.tsv ]]
then
    echo WARNING! multi.tsv already exists. please rename or remove.
    exit
fi

echo benchmarking serial, omp, mpi, hybrid >> multi.tsv
echo -n "program\t" >> multi.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n "$i"gb"\t" >> multi.tsv
done

echo >> multi.tsv

# SERIAL

echo benchmarking serial.e
echo -n serial'\t' >> multi.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(serial/serial.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	# echo -n "$a " >> multi.tsv
	let "s = s + a"
    done
    echo -n "\t" >> multi.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> multi.tsv
done
echo >> multi.tsv
echo

# OMP

echo benchmarking sentinel-otf-blocked.e
echo -n omp'\t' >> multi.tsv

export OMP_NUM_THREADS=10
export GOMP_CPU_AFFINITY="0-29:1"
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(sentinel/sentinel-otf-blocked-mt.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	# echo -n "$a " >> multi.tsv
	let "s = s + a"
    done
    # echo -n "\t" >> multi.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> multi.tsv
done
echo >> multi.tsv
echo

unset GOMP_CPU_AFFINITY

# MPI10

echo benchmarking mpi-vert.e 10
echo -n mpi10'\t' >> multi.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun -np 10 mpi/mpi-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	let "s = s + a"
	# echo -n "$a " >> multi.tsv
    done
    # echo -n "\t" >> multi.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> multi.tsv
done
echo >> multi.tsv
echo

# MPI22

echo benchmarking mpi-vert.e 22
echo -n mpi22'\t' >> multi.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun -np 22 mpi/mpi-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	let "s = s + a"
	# echo -n "$a " >> multi.tsv
    done
    # echo -n "\t" >> multi.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> multi.tsv
done
echo >> multi.tsv
echo

# MPI30

echo benchmarking mpi-vert.e 30
echo -n mpi30'\t' >> multi.tsv

for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun -np 30 mpi/mpi-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	let "s = s + a"
	# echo -n "$a " >> multi.tsv
    done
    # echo -n "\t" >> multi.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> multi.tsv
done
echo >> multi.tsv
echo

# HYBRID

echo benchmarking hybrid-vert.e
echo -n hybrid'\t' >> multi.tsv

export OMP_NUM_THREADS=9
for ((i = minSize; i <= maxSize; i+=$sizeStep))
do
    echo -n .
    let "s = 0"
    for ((j = 0; j < nRuns; j++))
    do
	let "a = "$(mpirun -np 3 hybrid/hybrid-vert.e bdna-link/"$i"gb-1.bdna bdna-link/"$i"gb-2.bdna)""
	# echo -n "$a " >> multi.tsv
	let "s = s + a"
    done
    # echo -n "\t" >> multi.tsv
    let "s = s / $nRuns"
    echo -n $s"\t" >> multi.tsv
done
echo >> multi.tsv 
echo
