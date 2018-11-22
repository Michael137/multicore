#!/bin/sh
lscpu > machine.info
USE_FLAG_RW=1 make all
PREFIX='array-based'
SUFFIX=$1
./run.sh ${PREFIX} 16 5 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5 0 100 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 0 1000000 ${SUFFIX}
