#!/bin/bash
if [ $EUID != 0 ]; then
	echo "perf stat requires root access"
    exit
fi

PREFIX=$1
THREADS_NUM=$2
SHARED_SIZE=$3
SINGLE_CORE=$4
READ_WRITE=$5
OUT_FILE="${PREFIX}_harness${READ_WRITE}_${THREADS_NUM}_${SHARED_SIZE}_cores${SINGLE_CORE}.result"

echo "Writing results to file: ${OUT_FILE}"

results=()
truncate -s 0 ${OUT_FILE}.report
for i in `seq 1 $THREADS_NUM`
do

	# Run perf stat 25 times
	# TODO: add "-x \;" option?
	# perf stat --pre 'sleep 2' -r 10 -d -d \
	results+=($( \
		perf stat -r 15 -d -d \
			-e cache-references,cache-misses,faults,context-switches,cpu-clock,offcore_requests.all_data_rd \
			./harness.out $i ${SHARED_SIZE} ${SINGLE_CORE} ${READ_WRITE} 2>&1 \
			| grep -v "Spawning" \
			| tee -a ${OUT_FILE}.report \
			| grep "seconds time elapsed" | awk '{print $1" "$7}')
	)

	sleep 5
done

truncate -s 0 ${OUT_FILE}
for ((j=0; j < ${#results[@]} - 1; j+=2));
do
	# gnuplot expects '#' as comments
	echo "${results[j]} # ${results[j+1]}" >> ${OUT_FILE}
done

gawk -i inplace '{printf("%d %s\n", NR, $0)}' ${OUT_FILE}
mv ${OUT_FILE} $(echo ${OUT_FILE} | tr '_' '-')
