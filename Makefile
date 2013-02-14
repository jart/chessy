CXX=clang++
STD=c++11
CXXFLAGS=-std=$(STD) -g -O0 -Wall -Werror -DUNICODE
LDFLAGS=-lglog -lgflags

all: chessy

chessy: board.o bot.o chessy.o main.o move.o render.o square.o term.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

board.o: board.cc board.h
bot.o: bot.cc bot.h
chessy.o: chessy.cc chessy.h
main.o: main.cc
move.o: move.cc move.h
render.o: render.cc render.h
square.o: square.cc square.h
term.o: term.cc term.h

clean:
	rm -f chessy *.o

run: chessy
	./chessy
