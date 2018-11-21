#!/bin/sh
PREFIX=$1
THREADS_NUM=$2
SHARED_SIZE=$3
SINGLE_CORE=$4
READ_WRITE=$5
SUFFIX=$6
OUT_FILE="${PREFIX}_harness${READ_ONLY}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_CORE}_run${SUFFIX}.result"
echo "Writing results to file: ${OUT_FILE}"
echo 0 0 > $OUT_FILE
for i in `seq 1 $THREADS_NUM`
do
	(time -f "$i %e" ./harness.out $i ${SHARED_SIZE} ${SINGLE_CORE} ${READ_WRITE} | grep -Ev 'S') >> $OUT_FILE 2>&1
done
