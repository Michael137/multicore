#!/bin/sh
LOCK=$1
THREADS_NUM=$2
SHARED_SIZE=$3
SINGLE_CORE=$4
READ_WRITE=$5
OUT_FILE="fairness${READ_ONLY}_${LOCK}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_CORE}.result"
echo "Writing results to file: ${OUT_FILE}"
echo 0 0 > $OUT_FILE
eval "DEBUG=1 USE_${LOCK}=1 make all"
./harness.out $THREADS_NUM ${SHARED_SIZE} ${SINGLE_CORE} | grep "Called from:" >> $OUT_FILE 2>&1
awk '{ print $3 }' ${OUT_FILE} | sort -n | uniq -c > "${OUT_FILE}.dist"
