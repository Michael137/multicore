make all

echo "Coarse lock"
./ts_swap_coarse -v

echo "Fine lock"
./ts_swap_fine -v

echo "Region lock"
./ts_swap_reg -v
