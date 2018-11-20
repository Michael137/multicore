LDFLAGS=-pthread
CXXFLAGS=

ifeq "$(DEBUG)" "1"
CXXFLAGS += -g -O0 -fsanitize=thread -DDEBUG
else
CXXFLAGS += -O2
endif

ifeq "$(USE_MUTEX)" "1"
CXXFLAGS += -DUSE_MUTEX
endif

ifeq "$(USE_TATAS)" "1"
CXXFLAGS += -DUSE_TATAS
endif

ifeq "$(USE_RW_TATAS)" "1"
CXXFLAGS += -DUSE_RW_TATAS
endif

ifeq "$(USE_FLAG_RW)" "1"
CXXFLAGS += -DUSE_FLAG_RW
endif

.PHONY:
all: clean compile

compile: harness.c
	gcc $(CXXFLAGS) $(LDFLAGS) $^ -o harness.out

clean:
	rm -f harness.out
	rm -f *.info
