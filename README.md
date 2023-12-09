# Introduction
This repo is for creating a chess engine specifically for [chess.com's Spell Chess](https://www.chess.com/variants/spell-chess) variant using the Minimax algorithm. The engine is written in C and the playable game is made with Unity.

# Plan
## State Representation
The state of the game will be stored in as few bytes as possible. One integer will store the position of all black pawns where a bit set to one means a pawn exists while a zero means it doesn't exist in that position. Bit 0 will be the square "a8" going across where bit 7 is the square "h8". One advantage of this trick is to allow a bitwise "and" to reduce the set of potential moves.

## Comunication between Unity and C
The current plan is to have Unity call the compiled C program as a subprocess with the state of the game as an extended FEN code. The C program will then return the move it thinks is best in the format of "a2a4" for example. The Unity program will then make the move and repeat the process.

# Compiling the Engine
With gcc installed, run the following command in the root directory of the repo:
```bash
make
```

# Running the Engine
To run the engine, run the following command in the root directory of the repo:
```bash
main <FEN> [<frozen> <jumpable> <spells> <waiting_spells>]
```
Where:
- `<FEN>`: the FEN code of the current state of the game. See [here](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) for more information.
- `<frozen>`: The square with the current usage of freeze (there can only be one square at a time). If no square is frozen, the "-" char will be used. (ex. `a2` means the square "a2" and the nearest squares to in all directions are frozen)
- `<jumpable>`: The square with the current usage of jump. As with the frozen square there can only be one at a time. If no square is jumpable, the "-" char is used. (ex. `a2` means the square "a2" is jumpable)
- `<spells>`: Number of spells each player has: It has freeze followed by jump. White's spells are first. (ex. `5241` means white has 5 freezes and 2 jumps while black has 4 freezes and 1 jump)
- `<waiting_spells>`: How many half turns until each spell can be used. It goes freeze then jump, where white is first. (ex. `0000` means all spells are ready to be used while `1300` means white has to wait 1 half turn to use freeze and 3 half turns to use jump while black can use both spells now)
