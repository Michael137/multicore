#!/bin/sh
LOCK=$1
THREADS_NUM=$2
SHARED_SIZE=$3
SINGLE_CORE=$4
READ_WRITE=$5
OUT_FILE="fairness${READ_WRITE}_${LOCK}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_CORE}.result"
echo "Writing results to file: ${OUT_FILE}"
echo 0 0 > $OUT_FILE
eval "DEBUG=1 USE_${LOCK}=1 make all"
./harness.out $THREADS_NUM ${SHARED_SIZE} ${SINGLE_CORE} ${READ_WRITE} > $OUT_FILE 2>&1

DIST_FILE=${OUT_FILE}.dist
grep "Called from:" ${OUT_FILE} | awk '{ print $3 }' | sort -n | uniq -c > "${DIST_FILE}"
SUM=$(awk '{print $1}' ${DIST_FILE} | paste -sd+ | bc)
WRITES=$(grep "unlocking write" ${OUT_FILE} | wc -l)
PCT=$(echo "scale=4;100 * ${WRITES}/${SUM}" | bc -l )
echo "Writes: ${WRITES}/${SUM} (${PCT}%)" >> ${DIST_FILE}

cat ${DIST_FILE}
