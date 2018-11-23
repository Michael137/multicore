#!/bin/sh
for i in `seq 1 100`
do
	time -f "%e" ./harness.out 6 5 0 0 | grep -vE 'Spawn' 2>&1
	sleep 1
done
