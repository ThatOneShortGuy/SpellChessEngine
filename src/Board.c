#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ParseFEN.c"
#include "Board.h"

#define max(a,b) ((a) > (b) ? (a) : (b))

void board_from_fen(Board* board, char *postion, char turn, char *castling, char *enpassant, char *halfmoves, char *fullmoves) {
    memset(board, 0, sizeof(Board));
    board->freeze_loc = board->jump_loc = 65;
    parse_position(board, postion);
    parse_turn(board, turn);
    parse_castling(board, castling);
    parse_enpassant(board, enpassant);
    parse_halfmoves(board, halfmoves);
    parse_fullmoves(board, fullmoves);
}

void board_add_spells_from_fen(Board* board, char *frozen, char *jumpable, char *spells, char *waiting_spells) {
    parse_frozen(board, frozen);
    parse_jumpable(board, jumpable);
    parse_spells(board, spells);
    parse_waiting_spells(board, waiting_spells);
}

void board_setup(Board *board) {
    board->white.pawns.piece_arr = 0x000000000000FF00;
    board->white.rooks.piece_arr = 0x0000000000000081;
    board->white.knights.piece_arr = 0x0000000000000042;
    board->white.bishops.piece_arr = 0x0000000000000024;
    board->white.queens.piece_arr = 0x0000000000000008;
    board->white.king.piece_arr = 0x0000000000000010;
    board->white.castling = 0b11;
    board->white.en_passant = 0b0000;
    board->white.num_jump_spells = 0b10;
    board->white.jump_spell = 0b00;
    board->white.num_freeze_spells = 0b101;
    board->white.freeze_spell = 0b00;
    board->black.pawns.piece_arr = 0x00FF000000000000;
    board->black.rooks.piece_arr = 0x8100000000000000;
    board->black.knights.piece_arr = 0x4200000000000000;
    board->black.bishops.piece_arr = 0x2400000000000000;
    board->black.queens.piece_arr = 0x0800000000000000;
    board->black.king.piece_arr = 0x1000000000000000;
    board->black.castling = 0b11;
    board->black.en_passant = 0b0000;
    board->black.jump_spell = 0b00;
    board->black.num_jump_spells = 0b10;
    board->black.freeze_spell = 0b00;
    board->black.num_freeze_spells = 0b101;
    board->turn = 0;
    board->freeze_loc = 65;
    board->jump_loc = 65;
}

__attribute__((optimize("unroll-loops")))
void board_print(Board *board) {
    char arr[64*(2+11)];
    int arr_index = 0;
    char black_pieces[64];
    char white_pieces[64];


    memset(black_pieces, ' ', 64);
    memset(white_pieces, ' ', 64);

    for (int i = 0; i < 64; i++) {
        #define X(piece, color, name) \
        if (board->color.name.piece_arr & (1ULL << i)) { \
            color##_pieces[i] = piece; \
        }
        LIST_OF_PIECES
        #undef X
    }

    for (int i = 0; i < 64; i++) {
        // Set background color
        int freeze_row = board->freeze_loc / 8;
        if (board->jump_loc == i) {
            arr_index += sprintf(arr+arr_index, "\033[102;");
        } else if ((board->freeze_loc != 65 && 
                  (board->freeze_loc >= i + 7 && board->freeze_loc <= i + 9 && i / 8 == freeze_row - 1)) ||
                  (board->freeze_loc >= i - 1 && board->freeze_loc <= i + 1 && i / 8 == freeze_row) ||
                  (board->freeze_loc >= i - 9 && board->freeze_loc <= i - 7 && i / 8 == freeze_row + 1)) {
            arr_index += sprintf(arr+arr_index, "\033[106;");
        } else {
            arr_index += sprintf(arr+arr_index, ((i % 2 == 0 && i / 8 % 2 == 0) || (i % 2 == 1 && i / 8 % 2 == 1)) ? "\033[107;" : "\033[100;");
        }

        // Set foreground color
        arr_index += sprintf(arr+arr_index, (black_pieces[i] > white_pieces[i]) ? "91m" : "94m");

        // Add piece
        arr_index += sprintf(arr+arr_index, " %c ", max(black_pieces[i], white_pieces[i]));

        // Newline
        if (i % 8 == 7) {
            arr_index += sprintf(arr+arr_index, "\033[0m\n");
        }
    }

    printf("%s", arr);
}

__attribute__((always_inline))
inline u64 color_pieces(Color color) {
    return color.pawns.piece_arr | color.rooks.piece_arr | color.knights.piece_arr | color.bishops.piece_arr | color.queens.piece_arr | color.king.piece_arr;
}

__attribute__((always_inline))
inline u64 board_covered_squares(Board board) {
    return color_pieces(board.white) | color_pieces(board.black);
}