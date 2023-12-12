#include <stdint.h>

#ifndef Board_H
#define Board_H

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint64_t u64;

typedef union {
    struct {
        u64 a8 : 1;
        u64 b8 : 1;
        u64 c8 : 1;
        u64 d8 : 1;
        u64 e8 : 1;
        u64 f8 : 1;
        u64 g8 : 1;
        u64 h8 : 1;
        u64 a7 : 1;
        u64 b7 : 1;
        u64 c7 : 1;
        u64 d7 : 1;
        u64 e7 : 1;
        u64 f7 : 1;
        u64 g7 : 1;
        u64 h7 : 1;
        u64 a6 : 1;
        u64 b6 : 1;
        u64 c6 : 1;
        u64 d6 : 1;
        u64 e6 : 1;
        u64 f6 : 1;
        u64 g6 : 1;
        u64 h6 : 1;
        u64 a5 : 1;
        u64 b5 : 1;
        u64 c5 : 1;
        u64 d5 : 1;
        u64 e5 : 1;
        u64 f5 : 1;
        u64 g5 : 1;
        u64 h5 : 1;
        u64 a4 : 1;
        u64 b4 : 1;
        u64 c4 : 1;
        u64 d4 : 1;
        u64 e4 : 1;
        u64 f4 : 1;
        u64 g4 : 1;
        u64 h4 : 1;
        u64 a3 : 1;
        u64 b3 : 1;
        u64 c3 : 1;
        u64 d3 : 1;
        u64 e3 : 1;
        u64 f3 : 1;
        u64 g3 : 1;
        u64 h3 : 1;
        u64 a2 : 1;
        u64 b2 : 1;
        u64 c2 : 1;
        u64 d2 : 1;
        u64 e2 : 1;
        u64 f2 : 1;
        u64 g2 : 1;
        u64 h2 : 1;
        u64 a1 : 1;
        u64 b1 : 1;
        u64 c1 : 1;
        u64 d1 : 1;
        u64 e1 : 1;
        u64 f1 : 1;
        u64 g1 : 1;
        u64 h1 : 1;
    } layout;
    u64 piece_arr;
    u8 row[8];
} Positions;

typedef struct {
    Positions pawns;
    Positions rooks;
    Positions knights;
    Positions bishops;
    Positions queens;
    Positions king;
    u8 castling         : 2; // 0b00 = no castling, 0b01 = kingside, 0b10 = queenside, 0b11 = both
    u8 en_passant       : 4; // 0b0000 = no en passant, 0b0001 = a, 0b0010 = b, 0b0011 = c, 0b0100 = d, 0b0101 = e, 0b0110 = f, 0b0111 = g, 0b1000 = h
    u8 num_jump_spells  : 2; // 0b00 = 0 jump spells, 0b01 = 1 jump spell, 0b10 = 2 jump spells
    u8 jump_spell       : 2; // 0b00 = jump spell ready, 0b01 = 1 more turn until jump spell, 0b10 = 2 more turns, 0b11 = 3 more turns
    u8 num_freeze_spells: 3; // 0b000 = 0 freeze spells, 0b001 = 1 freeze spell, 0b010 = 2 freeze spells, 0b011 = 3 freeze spells, 0b100 = 4 freeze spells, 0b101 = 5 freeze spells
    u8 freeze_spell     : 2; // 0b00 = freeze spell ready, 0b01 = 1 more turn until freeze spell, 0b10 = 2 more turns, 0b11 = 3 more turns
} Color;

typedef struct {
    Color white;
    Color black;
    u8 turn      : 1; // 0 = white, 1 = black
    u8 freeze_loc: 7; // Each location is represented by a number from 0 to 63, starting from a1 and ending at h8 with 64 = no freeze
    u8 jump_loc  : 7; // Each location is represented by a number from 0 to 63, starting from a1 and ending at h8 with 64 = no jump
} Board;

void board_from_fen(Board* board, char *postion, char turn, char *castling, char *enpassant, char *halfmoves, char *fullmoves);
void board_add_spells_from_fen(Board* board, char *frozen, char *jumpable, char *spells, char *waiting_spells);
void board_setup(Board *board);
void board_print(const Board *board);
u64 color_pieces(const Color color);
u64 board_all_squares(const Board board);
char* board_to_fen(Board board);
char* board_diff(const Board board1, const Board board2);

#endif