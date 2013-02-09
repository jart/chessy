all: chessy

CC=g++
STD=c++11
FLAGS=-std=$(STD) -g -O0 -Wall -Werror

all: chessy

chessy: chessy.o board.o bot.o
	$(CC) $(FLAGS) chessy.o board.o bot.o -o chessy

chessy.o: chessy.cc chessy.h
	$(CC) $(FLAGS) -c chessy.cc -o chessy.o

board.o: board.cc board.h
	$(CC) $(FLAGS) -c board.cc -o board.o

bot.o: bot.cc bot.h
	$(CC) $(FLAGS) -c bot.cc -o bot.o

clean:
	rm -f chessy *.o

run: chessy
	./chessy
