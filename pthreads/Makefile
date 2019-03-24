all: clean coarse fine region

ifeq "$(DEBUG)" ""
    DEBUG=0
else
    DEBUG=1
	CFLAGS+="-fsanitize=thread"
endif

.PHONY:
fine:
	gcc $(CFLAGS) -pthread -DFINE_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_fine

.PHONY:
coarse:
	gcc $(CFLAGS) -pthread -DCOARSE_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_coarse

.PHONY:
region:
	gcc $(CFLAGS) -pthread -DREG_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_reg

.PHONY:
clean:
	rm -f ts_swap_coarse
	rm -f ts_swap_fine
	rm -f ts_swap_reg
