objects = game.o board.o defs.o
comp_tags = -c -g -O0 -std=c++17

.PHONY: all
all: player host

clean:
	rm *.o game

defs.o: defs.cpp defs.hpp
	clang++ $(comp_tags) defs.cpp

board.o: board.cpp board.hpp
	clang++ $(comp_tags) board.cpp

game.o: game.cpp game.hpp
	clang++ $(comp_tags) game.cpp

player.o: player.cpp player.hpp
	clang++ $(comp_tags) player.cpp

host.o: host.cpp host.hpp
	clang++ $(comp_tags) host.cpp

player: player.o $(objects)
	clang++ -g -o player player.o $(objects)

host: host.o $(objects)
	clang++ -g -o host host.o $(objects)

.PHONY: run player
run player: 
	./player

.PHONY: run host
run host: 
	./host