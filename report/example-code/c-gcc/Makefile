LDFLAGS=-lpthread
CXXFLAGS=

ifeq "$(DEBUG)" "1"
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -O2
endif

.PHONY:
all: clean compile

compile: harness.c
	gcc $(CXXFLAGS) $(LDFLAGS) $^ -o harness.out

clean:
	rm -f harness.out
