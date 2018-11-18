#!/bin/sh
THREADS_NUM=$1
SHARED_SIZE=$2
SINGLE_CORE=$3
READ_WRITE=$4
OUT_FILE="fairness${READ_ONLY}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_CORE}.result"
echo "Writing results to file: ${OUT_FILE}"
echo 0 0 > $OUT_FILE
DEBUG=1 USE_TATAS=1 make all
./harness.out $THREADS_NUM ${SHARED_SIZE} ${SINGLE_CORE} | grep "Called from:" >> $OUT_FILE 2>&1
awk '{ print $3 }' ${OUT_FILE} | sort -n | uniq -c > "${OUT_FILE}.dist"
