#!/bin/sh
lscpu > machine.info
USE_ANDERSON=1 make all
PREFIX='anderson'
./run.sh ${PREFIX} 16 5 0 0
./run.sh ${PREFIX} 16 5 0 100
./run.sh ${PREFIX} 16 5 0 1000000
./run.sh ${PREFIX} 16 5000 0 0
./run.sh ${PREFIX} 16 5000 0 100
./run.sh ${PREFIX} 16 5000 0 1000000
