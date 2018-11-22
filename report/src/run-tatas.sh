#!/bin/sh
lscpu > machine.info
USE_TATAS=1 make all
PREFIX='tatas'
SUFFIX=$1
./run.sh ${PREFIX} 16 5 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5 0 100 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 0 1000000 ${SUFFIX}
