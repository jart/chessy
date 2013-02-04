all: chessy

chessy: chessy.cc
	g++ -std=c++11 -o chessy chessy.cc

clean:
	rm -f chessy

run: chessy
	./chessy
