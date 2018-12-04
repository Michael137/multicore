#!/bin/sh
lscpu > machine.info
USE_RW_TATAS=1 make all
PREFIX='rw-tatas'
./benchmark.sh ${PREFIX} 16 5 0 0
./benchmark.sh ${PREFIX} 16 5 0 100
./benchmark.sh ${PREFIX} 16 5 0 50000
./benchmark.sh ${PREFIX} 16 5000 0 0
./benchmark.sh ${PREFIX} 16 5000 0 100
./benchmark.sh ${PREFIX} 16 5000 0 50000
