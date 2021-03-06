#!/bin/zsh

if ! [[ -f "$1" ]]
then
    echo "$1 not found; attempting to compile..."
    make "$1"
fi

if ! [[ -f "$1" ]]
then
    echo "ERROR: could not compile $1. exiting."
else
    ./"$1" ../bdna/mid1.bdna ../bdna/mid2.bdna
fi
