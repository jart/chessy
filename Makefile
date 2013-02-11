all: chessy

CC=g++
STD=c++11
FLAGS=-std=$(STD) -g -O0 -Wall -Werror

all: chessy

chessy: chessy.o board.o move.o bot.o render.o
	$(CC) $(FLAGS) chessy.o board.o move.o bot.o render.o -o chessy


chessy.o: chessy.cc chessy.h
	$(CC) $(FLAGS) -c chessy.cc -o chessy.o

move.o: move.cc move.h
	$(CC) $(FLAGS) -c move.cc -o move.o

board.o: board.cc board.h
	$(CC) $(FLAGS) -c board.cc -l move.h -o board.o

bot.o: bot.cc bot.h
	$(CC) $(FLAGS) -c bot.cc -o bot.o

render.o: render.cc render.h
	$(CC) $(FLAGS) -c render.cc -o render.o

clean:
	rm -f chessy *.o

run: chessy
	./chessy
