LDFLAGS=-lpthread
CXXFLAGS += -O2 -DSINGLE_CORE=$(SINGLE_CORE)

.PHONY:
all: clean compile

compile: harness.c
	gcc $(CXXFLAGS) $(LDFLAGS) $^ -o harness.out

clean:
	rm -f harness.out
	rm -f *.result
	rm -f *.info
