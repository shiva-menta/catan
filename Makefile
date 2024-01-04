objects = game.o board.o defs.o
comp_tags = -c -g -O0 -std=c++17

.PHONY: all
all: game

clean:
	rm *.o game

defs.o: defs.cpp defs.hpp
	clang++ $(comp_tags) defs.cpp

board.o: board.cpp board.hpp
	clang++ $(comp_tags) board.cpp

game.o: game.cpp game.hpp
	clang++ $(comp_tags) game.cpp

game: $(objects)
	clang++ -g -o game $(objects)

.PHONY: run
run: 
	./game