#!/bin/sh
lscpu > machine.info
USE_MUTEX=1 make all
PREFIX='mutex'
./run.sh ${PREFIX} 16 5 0 0
./run.sh ${PREFIX} 16 5000 0 0
