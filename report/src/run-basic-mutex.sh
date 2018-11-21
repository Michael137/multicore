#!/bin/sh
lscpu > machine.info
USE_MUTEX=1 make all
PREFIX='mutex'
SUFFIX=$1
./run.sh ${PREFIX} 16 5 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 0 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5 100 0 ${SUFFIX}
./run.sh ${PREFIX} 16 5000 1000000 0 ${SUFFIX}
