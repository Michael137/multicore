#!/bin/sh
FILENAME=$1

COMBINED_FILE="${FILENAME}.combined"

paste -d" " ${FILENAME}_*.result | awk 'BEGIN {FS= " " ; OFS=FS}
     { for (i=2;i<=NF;i+=2) {printf("%s%s", $i,OFS)}
       printf("\n","")
     }' > ${COMBINED_FILE}

gawk -i inplace '{ for(i=1; i<=NF;i++) j+=$i; printf("%0.2f\n", j / NF); j=0 }' ${COMBINED_FILE}

gawk -i inplace '{printf "%d %s\n", NR, $0}' ${COMBINED_FILE}
