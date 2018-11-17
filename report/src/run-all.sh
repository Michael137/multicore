#!/bin/sh
lscpu > machine.info
if [ ! -d harness.out ];then
	make all
fi
./run.sh 16 5 0 0
./run.sh 16 5 1 0
./run.sh 16 5000 0 0
./run.sh 16 5000 1 0
