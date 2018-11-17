#!/bin/sh
THREADS_NUM=$1
SHARED_SIZE=$2
SINGLE_MULTI=$3
RO_RW=$4
OUT_FILE="harness${RO_RW}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_MULTI}.result"
echo "Writing results to file: ${OUT_FILE}"
echo 0 0 > $OUT_FILE
for i in `seq 1 $THREADS_NUM`
do
	(time -f "$i %e" ./harness.out $i ${SHARED_SIZE} ${SINGLE_MULTI} | grep -Ev 'S') >> $OUT_FILE 2>&1
done
