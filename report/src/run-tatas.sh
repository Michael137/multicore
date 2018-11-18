#!/bin/sh
lscpu > machine.info
USE_TATAS=1 make all
PREFIX='tatas'
./run.sh ${PREFIX} 16 5 0 0
./run.sh ${PREFIX} 16 5000 0 0
