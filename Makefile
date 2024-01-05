objects = host.o game.o board.o defs.o
comp_tags = -c -g -O0 -std=c++17

# General Commands
.PHONY: all
all: board player host

clean:
	rm *.o board player host

# Compile Commands
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

# Link Commands
board: board.o defs.o
	clang++ -g -o board board.o defs.o

player: player.o
	clang++ -g -o player player.o

host: $(objects)
	clang++ -Wall -g -o host $(objects)

# Run Commands
.PHONY: run-board
run-board: board
	./board
PLAYER_ARGS = arg1 arg2
.PHONY: run-player # Pass In (port, hostAddress – 127.0.0.1 || IP)
run-player: player
	./player $(PLAYER_ARGS)
HOST_ARGS = arg1
.PHONY: run-host # Pass In (port, hostAddress – 127.0.0.1 || IP)
run-host: host
	./host $(HOST_ARGS)
