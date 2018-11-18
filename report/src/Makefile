LDFLAGS=-lpthread
CXXFLAGS += -O2 # -fsanitize=thread

ifeq "$(USE_MUTEX)" "1"
CXXFLAGS += -DUSE_MUTEX
endif

ifeq "$(USE_TATAS)" "1"
CXXFLAGS += -DUSE_TATAS
endif

ifeq "$(USE_RW_TATAS)" "1"
CXXFLAGS += -DUSE_RW_TATAS
endif

.PHONY:
all: clean compile

compile: harness.c
	gcc $(CXXFLAGS) $(LDFLAGS) $^ -o harness.out

clean:
	rm -f harness.out
	rm -f *.result
	rm -f *.info
