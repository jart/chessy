all: chessy

chessy:
	g++ -std=c++11 -o chessy chessy.cc

run: chessy
	./chessy
