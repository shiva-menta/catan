objects = board.o defs.o
std = -std=c++17

.PHONY: all
all: board

clean:
	rm *.o board

defs.o: defs.cpp defs.hpp
	clang++ -c -g -O0 $(std) defs.cpp

board.o: board.cpp board.hpp
	clang++ -c -g -O0 $(std) board.cpp

board: $(objects)
	clang++ -g -o board $(objects)

.PHONY: run
run: 
	./board