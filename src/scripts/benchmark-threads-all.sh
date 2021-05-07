#!/bin/zsh

# programs to benchmark
progs=(sentinel/sentinel-parfill-mt.e
       sentinel/sentinel-otf-mt.e
       sentinel/sentinel-otf-blocked-mt.e)

# names for table
tags=(parfill
      otf
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

