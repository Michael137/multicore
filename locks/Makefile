LDFLAGS=-pthread
CXXFLAGS=

ifeq "$(DEBUG)" "1"
CXXFLAGS += -DDEBUG -g -O0
else
CXXFLAGS += -O2
endif

ifeq "$(SANITIZE)" "1"
CXXFLAGS += -fsanitize=thread
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

ifeq "$(USE_RW_ARRAY)" "1"
CXXFLAGS += -DUSE_RW_ARRAY
endif

ifeq "$(USE_ANDERSON)" "1"
CXXFLAGS += -DUSE_ANDERSON
endif

.PHONY:
all: clean compile

compile: harness.c
	gcc $(CXXFLAGS) $(LDFLAGS) $^ -o harness.out

clean:
	rm -f harness.out
	rm -f *.info
