#!/bin/sh
./run-basic-mutex.sh
sleep 5
./run-tatas.sh
sleep 5
./run-rw-tatas.sh
sleep 5
./run-rw-array.sh
