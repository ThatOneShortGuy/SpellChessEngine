# Introduction
This repo is for creating a chess engine specifically for [chess.com's Spell Chess](https://www.chess.com/variants/spell-chess) variant using the Minimax algorithm. The engine prototype is written in Python and will eventually be ported to C or Rust (undetermined) for performance reasons. The engine will not be able to investigate very far into the game tree because the branching factor is absurdly high.

# Plan
## State Representation
The state of the game will be stored in 16 64-bit integers. One integer will store the position of all black pawns where a bit set to one means a pawn exists while a zero means it doesn't exist in that position. Bit 0 will be the square "a8" going across where bit 7 is the square "h8". One advantage of this trick is to allow a bitwise "and" to reduce the set of potential moves.