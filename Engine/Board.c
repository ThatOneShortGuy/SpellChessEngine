#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ParseFEN.c"
#include "Board.h"

#define max(a,b) ((a) > (b) ? (a) : (b))

void board_from_fen(Board* board, char *postion, char turn, char *castling, char *enpassant, char *halfmoves, char *fullmoves) {
    memset(board, 0, sizeof(Board));
    board->freeze_loc = board->jump_loc = 64;
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
void board_print(const Board *board) {
    char arr[64*(2+12)];
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
        } else if (board->freeze_loc != 64 && (
                  (board->freeze_loc >= i + 7 && board->freeze_loc <= i + 9 && i / 8 == freeze_row - 1) ||
                  (board->freeze_loc >= i - 1 && board->freeze_loc <= i + 1 && i / 8 == freeze_row) ||
                  (board->freeze_loc >= i - 9 && board->freeze_loc <= i - 7 && i / 8 == freeze_row + 1))) {
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

u64 color_pieces(const Color color) {
    return color.pawns.piece_arr | color.rooks.piece_arr | color.knights.piece_arr | color.bishops.piece_arr | color.queens.piece_arr | color.king.piece_arr;
}

u64 board_all_squares(const Board board) {
    return color_pieces(board.white) | color_pieces(board.black);
}

char* board_to_fen(Board board) {
    char *fen =  (char*) calloc(100, sizeof(char));
    int fen_index = 0;

    for (int row = 0; row < 8; row++) {
        int empty = 0;
        for (int col = 0; col < 8; col++) {
            int i = row * 8 + col;
            #define X(piece, color, name) \
            if (board.color.name.piece_arr & (1ULL << i)) { \
                if (empty != 0) fen[fen_index++] = '0' + empty; \
                empty = 0; \
                fen[fen_index++] = piece; \
                continue; \
            }
            LIST_OF_PIECES
            #undef X
            empty++;
        }
        if (empty != 0) {
            fen[fen_index++] = '0' + empty;
        }
        fen[fen_index++] = '/';
    }

    fen_index--;
    fen[fen_index++] = ' ';
    fen[fen_index++] = board.turn ? 'b' : 'w';
    fen[fen_index++] = ' ';
    fen[fen_index++] = board.white.castling ? 'K' : '-';
    fen[fen_index++] = board.white.castling & 0b10 ? 'Q' : '-';
    fen[fen_index++] = board.black.castling ? 'k' : '-';
    fen[fen_index++] = board.black.castling & 0b10 ? 'q' : '-';
    fen[fen_index++] = ' ';
    if (board.white.en_passant == 0) {
        fen[fen_index++] = '-';
    } else {
        fen[fen_index++] = 'a' + board.white.en_passant;
        fen[fen_index++] = board.turn ? '6' : '3';
    }
    fen[fen_index++] = ' ';
    fen[fen_index++] = '0';
    fen[fen_index++] = ' ';
    fen[fen_index++] = '0';
    fen[fen_index++] = ' ';
    if (board.freeze_loc == 64) {
        fen[fen_index++] = '-';
    } else {
        fen[fen_index++] = 'a' + __builtin_clzll(board.freeze_loc) % 8;
        fen[fen_index++] = '8' - __builtin_clzll(board.freeze_loc) / 8;
    }
    fen[fen_index++] = ' ';
    if (board.jump_loc == 64) {
        fen[fen_index++] = '-';
    } else {
        fen[fen_index++] = 'a' + __builtin_clzll(board.jump_loc) % 8;
        fen[fen_index++] = '8' - __builtin_clzll(board.jump_loc) / 8;
    }
    fen[fen_index++] = ' ';
    fen[fen_index++] = '0' + board.white.num_freeze_spells;
    fen[fen_index++] = '0' + board.white.num_jump_spells;
    fen[fen_index++] = '0' + board.black.num_freeze_spells;
    fen[fen_index++] = '0' + board.black.num_jump_spells;
    fen[fen_index++] = ' ';
    fen[fen_index++] = '0' + board.white.freeze_spell;
    fen[fen_index++] = '0' + board.white.jump_spell;
    fen[fen_index++] = '0' + board.black.freeze_spell;
    fen[fen_index++] = '0' + board.black.jump_spell;

    return fen;
}

void print_bitboard(u64 bitboard) {
    for (int i = 0; i < 64; i++) {
        printf("%llu", (bitboard >> i) & 1);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");
}

char* board_diff(const Board prev_board, const Board curr_board) {
    const u64 all_board1 = board_all_squares(prev_board);
    const u64 all_board2 = board_all_squares(curr_board);


    // board_print(&board1);
    // board_print(&board2);

    // print_bitboard(prev_turn_color_board);
    // print_bitboard(curr_turn_other_board);

    const u64 diff = all_board1 ^ all_board2;
    const u64 from = diff & all_board1;
    const u64 to = diff & all_board2;

    const int from_loc = __builtin_ctzll(from);
    int to_loc = __builtin_ctzll(to);
    if (to == 0) {
        const u64 prev_turn_color_board = prev_board.turn ? color_pieces(prev_board.white) : color_pieces(prev_board.black);
        const u64 curr_turn_other_board = curr_board.turn ? color_pieces(curr_board.white) : color_pieces(curr_board.black);
        // printf("Same location\n");
        // printf("%llu\n", prev_turn_color_board & curr_turn_other_board);
        to_loc = __builtin_ctzll(prev_turn_color_board & curr_turn_other_board);
    }
    const int from_col = from_loc % 8;
    const int from_row = from_loc / 8;
    const int to_col = to_loc % 8;
    const int to_row = to_loc / 8;
    // printf("(%d, %d) -> %d %d %d %d\n", from_loc, to_loc, from_col, from_row, to_col, to_row);

    char *diff_str = (char*) calloc(100, sizeof(char));
    int diff_index = 0;

    diff_index += sprintf(diff_str+diff_index, "%c%c %c%c ", 'a' + from_col, '8' - from_row, 'a' + to_col, '8' - to_row);

    if (prev_board.freeze_loc == curr_board.freeze_loc || curr_board.freeze_loc == 64) {
        diff_index += sprintf(diff_str+diff_index, "- ");
    } else {
        printf("Freeze loc: %d, white freezes: %d, black freezes: %d\n", curr_board.freeze_loc, curr_board.white.num_freeze_spells, curr_board.black.num_freeze_spells);
        diff_index += sprintf(diff_str+diff_index, "%c%c ", 'a' + curr_board.freeze_loc % 8, '8' - curr_board.freeze_loc / 8);
    }

    if (prev_board.jump_loc == curr_board.jump_loc || curr_board.jump_loc == 64) {
        diff_index += sprintf(diff_str+diff_index, "- ");
    } else {
        printf("Jump loc: %d, white jumps: %d, black jumps: %d\n", curr_board.jump_loc, curr_board.white.num_jump_spells, curr_board.black.num_jump_spells);
        diff_index += sprintf(diff_str+diff_index, "%c%c ", 'a' + curr_board.jump_loc % 8, '8' - curr_board.jump_loc / 8);
    }

    return diff_str;
}