#!/bin/sh
lscpu > machine.info
USE_RW_ARRAY=1 make all
PREFIX='rw-array'
./run.sh ${PREFIX} 16 5 0 0
./run.sh ${PREFIX} 16 5 0 100
./run.sh ${PREFIX} 16 5 0 1000000
./run.sh ${PREFIX} 16 5000 0 0
./run.sh ${PREFIX} 16 5000 0 100
./run.sh ${PREFIX} 16 5000 0 1000000
