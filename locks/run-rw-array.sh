#!/bin/sh
lscpu > machine.info
USE_RW_ARRAY=1 make all
PREFIX='rw-array'
./benchmark.sh ${PREFIX} 16 5 0 0
./benchmark.sh ${PREFIX} 16 5 0 100
./benchmark.sh ${PREFIX} 16 5 0 50000
./benchmark.sh ${PREFIX} 16 5000 0 0
./benchmark.sh ${PREFIX} 16 5000 0 100
./benchmark.sh ${PREFIX} 16 5000 0 50000
