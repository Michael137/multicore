#!/bin/sh
lscpu > machine.info
make all
PREFIX="basic"
SUFFIX=$1
./run.sh ${PREFIX} 16 5 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5 1 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 1 0 ${SUFFIX}
