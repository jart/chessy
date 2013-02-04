all: chessy

chessy: chessy.cc
	clang++ -std=c++11 -Wall -Werror -o chessy chessy.cc

clean:
	rm -f chessy

run: chessy
	./chessy
