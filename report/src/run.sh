#!/bin/sh
PREFIX=$1
THREADS_NUM=$2
SHARED_SIZE=$3
SINGLE_CORE=$4
READ_WRITE=$5
OUT_FILE_BASE="${PREFIX}_harness${READ_WRITE}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_CORE}"
for j in `seq 1 10`
do
	OUT_FILE="${OUT_FILE_BASE}_${j}.result"
	echo "Writing results to file: ${OUT_FILE}"
	truncate -s 0 ${OUT_FILE}
	for i in `seq 1 $THREADS_NUM`
	do
		(time -f "$i %e" ./harness.out $i ${SHARED_SIZE} ${SINGLE_CORE} ${READ_WRITE} | grep -Ev 'S') >> ${OUT_FILE} 2>&1
		sed -i '/terminated by/c\' ${OUT_FILE}
	done
done

./collect-times.sh ${OUT_FILE_BASE}
