all: chessy

chessy: chessy.cc
	g++ -std=c++11 -g -O0 -Wall -Werror -o chessy chessy.cc

clean:
	rm -f chessy

run: chessy
	./chessy
