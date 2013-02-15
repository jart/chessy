CXX=clang++
STD=c++11
CXXFLAGS=-std=$(STD) -g -O0 -Wall -Werror -DUNICODE
LDFLAGS=-lglog -lgflags
OBJS=board.o bot.o chessy.o move.o render.o square.o term.o

all: chessy

chessy: main.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

experimental: experimental.o square.o term.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

bitboard.o: bitboard.cc bitboard.h
board.o: board.cc board.h
bot.o: bot.cc bot.h
chessy.o: chessy.cc chessy.h
experimental.o: experimental.cc
main.o: main.cc
move.o: move.cc move.h
render.o: render.cc render.h
square.o: square.cc square.h
term.o: term.cc term.h

clean:
	rm -f chessy *.o

run: chessy
	./chessy
