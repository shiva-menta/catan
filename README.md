# C++ Catan
LAN-supported C++-implementation of Catan.

## Considerations
- Board: The board will be a hexagonal board with customizable hexagonal size, using the formula (3n^2 - 3n + 1), with roughly evenly distributed resource tiles assigned to each position. The board's tiles should be in an array starting from the top and going to the bottom. First, board tiles should be assigned resources. Then, board tiles should be assigned numerical values. 
- Game State: The game state should be responsible for keeping track of overall score of the game, a stack of development cards, a stack of resource cards, longest road and largest army cards, the position of the robber, the board and the placement of each of its pieces. The game state should OWN the board, and be able to declare winning and losing conditions.
- Player: Player should have an inventory of towns, cities, roads, development cards. Players should be able to build resources at valid locations if they have enough materials. Players should be able to initiate trades with other players, or be able to trade with the bank at a 4:1 base rate, or anything slightly better given by ports. Players should aim to reach 10 points.
- Development Cards: The game state should have a deck of development cards based on a pre-determined ordering of cards, initiate a random ordering and a stack of development cards to be picked from. Development cards should have some base implemented logic across all cards, and then an action function that allows them to modify the game state as needed.

## Grid Representation
For ease of use, I have provided a compact grid in which users can reference specific positions they wish to modify.

## Gameplay Process
### Pre-Game
- All users join the game. This entails keeping a counter of total number of players and returning an integer corresponding to each player. Each player should be given building resources (15 Roads, 5 Towns, 4 Cities).
- Initialize board, full stack of resource cards and development cards.
- Initialize robber on desert tile.
### Game
- First Turn: Randomize an order for players to select their initial settlements and connecting roads, and then invert that ordering for the second placement.
- Subsequent Turns: each new turn should be accompanied with two dice rolls that display on screen. Users that have settlements on corresponding locations should receive those resources. After resource acquisition, users should be able to place new objects under the following conditions.
- Roads can be placed if a user has one brick and one tree, is adjacent to another road of theirs, and isn't already occupied. Each road placement should lead to a recalculation of longestRoad (extra).
- Towns can be placed if a user has one sheep, one wheat, one brick, one tree, is connected to at least one road of theirs, isn't roadwise adjacent to another settlement, and isn't already taken. (+1)
- Cities can be placed if a user has two wheat, three stone and already owns the town. (+1)
- Every turn, the overall score will be checked against the WIN threshold, and the game will terminate operation (and stop accepting inputs) if reached by a certain player.
### Post-Game
- Post game will include termination of the program due to a WIN threshold being reached.

## Development Process
- V1: Board Creation
- V2: Game State and Player Creation / Development Card Creation
- V3: Enabling Trades
- V4: Adding Trade Ports
- V5: LAN-Enabled Play Creation
