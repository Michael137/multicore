all: clean coarse fine region lockfree

ifeq "$(DEBUG)" ""
    DEBUG=0
else
    DEBUG=1
	CFLAGS+="-fsanitize=thread"
endif

INCLUDES=-I/usr/local/include
LDFLAGS=-L/usr/local/lib

.PHONY:
fine:
	gcc $(CFLAGS) $(INCLUDES) $(LDFLAGS) -pthread -DFINE_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_fine

.PHONY:
coarse:
	gcc $(CFLAGS) $(INCLUDES) $(LDFLAGS) -pthread -DCOARSE_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_coarse

.PHONY:
region:
	gcc $(CFLAGS) $(INCLUDES) $(LDFLAGS) -pthread -DREG_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_reg

.PHONY:
lockfree:
	gcc $(CFLAGS) $(INCLUDES) $(LDFLAGS) -pthread -DNO_LCK -DDEBUG=$(DEBUG) ts_swap.c -o ts_swap_lockfree

.PHONY:
clean:
	rm -f ts_swap_coarse
	rm -f ts_swap_fine
	rm -f ts_swap_reg
	rm -f ts_swap_lockfree
