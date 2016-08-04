#!/bin/bash


EXE=$1

name=${EXE#../}
name=${name%%/*}
logfile="$name.txt"


test -f $logfile && exit

n=512
while (( n <= 3072 )); do
	$EXE --size $n | tee -a $logfile
	(( n += 128 ))
done

