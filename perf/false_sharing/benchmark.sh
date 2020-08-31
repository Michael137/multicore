#!/bin/bash

echo "With false-sharing"
for i in `seq 8`
do
	g++ -DTH_NUM=${i} -DDT_SIZE=8 false_sharing.cpp -std=c++2a -O0 && ./a.out
done

echo "Without false-sharing"
for i in `seq 8`
do
	g++ -DTH_NUM=${i} -DDT_SIZE=64 false_sharing.cpp -std=c++2a -O0 && ./a.out
done
