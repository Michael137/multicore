#!/bin/sh

# lscpu > machine.info
# rm -f basic.out
# gcc -O2 -pthread basic.c -o basic.out
# THREADS_NUM=$1
# DELAY=$2
# SLEEP=$3
# for j in `seq 1 6`
# do
# 	OUT_FILE="basic_${THREADS_NUM}_${DELAY}_sleep${SLEEP}_${j}.result"
# 	echo 0 0 > ${OUT_FILE}
# 	for i in `seq 1 ${THREADS_NUM}`
# 	do
# 		(time -f "${i} %e" ./basic.out ${i} ${DELAY}) >> $OUT_FILE 2>&1
# 	done
# 	sleep ${SLEEP}
# done

lscpu > machine.info
#gcc -O2 -pthread basic.c -o harness.out
#PREFIX='basic'
#./benchmark.sh ${PREFIX} 16 5 0 0
#./benchmark.sh ${PREFIX} 16 5000 0 0

make all
PREFIX='single-core'
./benchmark.sh ${PREFIX} 16 5 1 0
./benchmark.sh ${PREFIX} 16 5000 1 0
