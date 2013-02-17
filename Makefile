CXX=clang++
STD=c++11
CXXFLAGS=-std=$(STD) -g -O0 -Wall -Werror -DUNICODE
LDFLAGS=-lglog -lgflags
OBJS=bitboard.o bitmove.o board.o bot.o chessy.o piece.o render.o square.o term.o

all: chessy

chessy: main.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

experimental: experimental.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

bitboard.o: bitboard.cc *.h
bitmove.o: bitmove.cc *.h
board.o: board.cc *.h
bot.o: bot.cc *.h
chessy.o: chessy.cc *.h
experimental.o: experimental.cc *.h
main.o: main.cc *.h
piece.o: piece.cc *.h
render.o: render.cc *.h
square.o: square.cc *.h
term.o: term.cc *.h

clean:
	rm -f chessy *.o

run: chessy
	./chessy
