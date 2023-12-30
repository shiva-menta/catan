# C++ Catan
LAN-supported C++-implementation of Catan.

## Considerations
- Board: The board will be a hexagonal board with customizable hexagonal size, using the formula (3n^2 - 3n + 1), with roughly evenly distributed resource tiles assigned to each position. The board's tiles should be in an array starting from the top and going to the bottom. First, board tiles should be assigned resources. Then, board tiles should be assigned numerical values. 
- Game State: The game state should be responsible for keeping track of overall score of the game, a stack of development cards, a stack of resource cards, longest road and largest army cards, the position of the robber, the board and the placement of each of its pieces. The game state should OWN the board, and be able to declare winning and losing conditions.
- Player: Player should have an inventory of towns, cities, roads, development cards. Players should be able to build resources at valid locations if they have enough materials. Players should be able to initiate trades with other players, or be able to trade with the bank at a 4:1 base rate, or anything slightly better given by ports. Players should aim to reach 10 points.
- Development Cards: The game state should have a deck of development cards based on a pre-determined ordering of cards, initiate a random ordering and a stack of development cards to be picked from. Development cards should have some base implemented logic across all cards, and then an action function that allows them to modify the game state as needed.

## Grid Representation
For ease of use, we wanted to make this grid presentable on a 15 by 25 grid so it is easy to print out.

Sea: ~
Settlement: o, T, C
Roads: / | \
Tile: (R)##[Br, Or, Wh, Wo, Sh]

1: 3 settlements    ooo
2: 6 roads  /\/\/\
3: 4 settlements    oooo
4: 4 roads, 3 tiles | ##Ti | ##Ti | ##Ti |
5: 4 settlements oooo
6: 8 roads /\/\/\/\
7: 5 settlements ooooo
8: 5 roads, 4 tiles
9: 5 settlements
10: 10 roads
11: 6 settlements
12: 6 roads, 5 tiles
13: 6 settlements
14: 10 roads
15: 5 settelements
16: 5 roads, 4 tiles
17: 5 settlements
18: 8 roads
19: 4 settlements
20: 4 roads, 3 tiles
21: 4 settlements
22: 6 roads
23: 3 settlements

## Development Process
- V1: Board Creation
- V2: Game State and Player Creation
- V3: Development Card Creation
- V4: LAN-Enabled Play Creation
