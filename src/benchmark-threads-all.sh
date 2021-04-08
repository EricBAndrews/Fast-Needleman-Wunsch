#!/bin/zsh

# programs to benchmark
progs=(sentinel-mt.e
       sentinel-parfill-mt.e
       sentinel-otf-mt.e
       sentinel-nofill-mt.e
       sentinel-otf-blocked-mt.e)

# names for table
tags=(naive
      parfill
      otf
      nofill
      otf-blocked)

let "numProgs = ${#progs[@]}"

if [ $numProgs != ${#tags[@]} ]
then
    echo WARNING! programs and tags do not match. please fix.
    exit
fi

for ((i = 1; i <= $numProgs; i++))
do
    echo benchmarking ${progs[$i]}...
    ./benchmark-threads.sh ${progs[$i]}
    mv threads.tsv ${tags[$i]}.tsv
done

