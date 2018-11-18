#!/bin/sh
for i in `seq 1 $1`
do
	./run-rw-tatas.sh $i
	./run-tatas.sh $i
	./run-basic-mutex.sh $i
	./run-basic.sh $i
	./run-array-lock.sh $i
done
