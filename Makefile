objects = host.o game.o board.o defs.o
comp_tags = -c -g -O0 -std=c++17

.PHONY: all
all: player host

clean:
	rm *.o player host

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

player: player.o
	clang++ -g -o player player.o

host: $(objects)
	clang++ -Wall -g -o host $(objects)