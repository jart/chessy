CXX=clang++
STD=c++11
CXXFLAGS=-std=$(STD) -g -O0 -Wall -Werror -DUNICODE
LDFLAGS=

all: chessy

chessy: chessy.o board.o move.o bot.o render.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

chessy.o: chessy.cc chessy.h
move.o: move.cc move.h
board.o: board.cc board.h
bot.o: bot.cc bot.h
render.o: render.cc render.h

clean:
	rm -f chessy *.o

run: chessy
	./chessy
