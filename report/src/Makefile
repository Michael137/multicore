LDFLAGS=-lpthread
CXXFLAGS += -O2

.PHONY:
all: clean compile

compile: harness.c
	gcc $(CXXFLAGS) $(LDFLAGS) $^ -o harness.out

clean:
	rm -f harness.out
	rm -f *.result
	rm -f *.info
