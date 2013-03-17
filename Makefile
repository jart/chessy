# chessy - A toy chess engine just for fun.
# Copyright (c) 2013 Serene Han and Justine Tunney

# Example invocations:
#  - make                          # Bring chessy to life.
#  - make check                    # Run unit tests.
#  - make clean                    # Delete all generated files.
#  - sudo make install             # Allow chessy to stay forever :)
#  - sudo make uninstall           # Kick chessy out of your house :(
#  - make lint                     # Check for C++ style errors.
#  - CXXFLAGS="-O3 -DNDEBUG" make  # Create a faster build.
#  - make -pn | less               # View implicit make rules and variables.

CXX          = clang++
LINK.o       = $(LINK.cc)
PREFIX      ?= /usr/local
TARGET_ARCH ?= -march=native
CXXFLAGS    ?= -g -O2 -DUNICODE
CXXFLAGS    += -std=c++11 -Wall -Werror
LDLIBS      += -lm -lglog -lgflags

ifeq ($(shell hostname),bean)
CXXFLAGS += -I/usr/include/x86_64-linux-gnu/c++/4.7
endif

SOURCES = \
	bitboard.o \
	bitmove.o \
	board.o \
	bot.o \
	chessy.o \
	piece.o \
	render.o \
	square.o \
	term.o

all: chessy
chessy: main.o $(SOURCES)
experimental: experimental.o $(SOURCES)

check: test
	./test --alsologtostderr --gtest_color=yes

clean:
	$(RM) test chessy experimental $(wildcard *.o *.d $(GTEST_DIR)/src/*.o)

install: chessy
	install --mode=0755 chessy $(PREFIX)/bin

uninstall:
	$(RM) $(PREFIX)/bin/chessy

lint:
	cpplint.py --filter=-build/include,-readability/streams,-build/header_guard \
		$(wildcard *.cc *.h)

%.cc: %.rl
	ragel -o $@ $<

# google-test integration magic.
GTEST_DIR ?= gtest-1.6.0
TESTS = $(GTEST_DIR)/src/gtest-all.o $(GTEST_DIR)/src/gtest_main.o \
        $(patsubst %.cc,%.o,$(wildcard *_test.cc))
$(TESTS): CXXFLAGS += -I$(GTEST_DIR)/include -I$(GTEST_DIR) -pthread
$(filter gtest%,$(TESTS)): CXXFLAGS := $(filter-out -MD -Wall,$(CXXFLAGS))
test: $(TESTS) $(SOURCES) ; $(LINK.cc) $^ $(LDLIBS) -lpthread -o $@

# Recompile sources when headers change.
CXXFLAGS += -MD
-include $(wildcard *.d)
