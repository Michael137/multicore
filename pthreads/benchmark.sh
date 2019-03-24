#!/bin/sh

MAKE_CMD=
if [ "$(uname)" = "FreeBSD" ]
then
	MAKE_CMD="gmake all"
else
	MAKE_CMD="make all"
fi

${MAKE_CMD}

echo "Coarse lock"
./ts_swap_coarse -v

echo "Fine lock"
./ts_swap_fine -v

echo "Region lock"
./ts_swap_reg -v

echo "Lockfree"
./ts_swap_lockfree -v
