objects = board.o defs.o
std = -std=c++17

.PHONY: all
all: board

clean:
	rm *.o board

defs.o: defs.cpp defs.hpp
	g++ -c $(std) defs.cpp

board.o: board.cpp board.hpp
	g++ -c $(std) board.cpp

board: $(objects)
	g++ -o board $(objects)

.PHONY: run
run: 
	./board