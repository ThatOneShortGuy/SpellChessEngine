#include <queue>
#include <vector>
#include <iostream>
#include <functional>

#include "Board.h"
#include "PieceScores.h"
#include "engine.hpp"

using namespace std;

__attribute__((always_inline))
inline i16 get_score(Positions position, const int score_array[8][8], int base_value) {
    i16 score = 0;
    int row, col;
    
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            u64 part = (position.row[row] & (1 << col));
            part = part && part;
            score += (score_array[row][col] + base_value) * part;
        }
    }
    return score;
}

i16 board_evaluate(Board board) {
    i16 white_pawn_score = get_score(board.white.pawns, WHITE_PAWN_SCORE_ARRAY, 100);
    i16 black_pawn_score = get_score(board.black.pawns, BLACK_PAWN_SCORE_ARRAY, 100);
    i16 white_knight_bishop_score = get_score(board.white.knights, WHITE_KNIGHT_SCORE_ARRAY, 300) + get_score(board.white.bishops, WHITE_BISHOP_SCORE_ARRAY, 300);
    i16 black_knight_bishop_score = get_score(board.black.knights, BLACK_KNIGHT_SCORE_ARRAY, 300) + get_score(board.black.bishops, BLACK_BISHOP_SCORE_ARRAY, 300);
    i16 white_rook_score = get_score(board.white.rooks, WHITE_ROOK_SCORE_ARRAY, 500);
    i16 black_rook_score = get_score(board.black.rooks, BLACK_ROOK_SCORE_ARRAY, 500);
    i16 white_queen_score = get_score(board.white.queens, WHITE_QUEEN_SCORE_ARRAY, 900);
    i16 black_queen_score = get_score(board.black.queens, BLACK_QUEEN_SCORE_ARRAY, 900);

    i16 pawn_score = white_pawn_score - black_pawn_score;
    i16 knight_bishop_score = white_knight_bishop_score - black_knight_bishop_score;
    i16 rook_score = white_rook_score - black_rook_score;
    i16 queen_score = white_queen_score - black_queen_score;
    return (pawn_score + knight_bishop_score) + (rook_score + queen_score);
}

__attribute__((always_inline))
inline void add_move(Board board, u64 from, u64 to) {
    Board* new_board = new Board;
    memcpy(new_board, &board, sizeof(Board));
    new_board->turn = !board.turn;
    new_board->white.pawns.piece_arr = (board.white.pawns.piece_arr & ~from) | to;
}

__attribute__((always_inline))
inline void add_pawn_moves(MoveQueue moves, Board board, Color move_color, int move_direction) {
    u64 from, to;
    int row, col;
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            from = 1 << (row * 8 + col);
            if (!(move_color.pawns.piece_arr & from)) continue;

            // If there is not a piece in front of the pawn, it can move 1 square
            to = from >> (8 * move_direction);
            if (!(board_all_squares(board) & to)) {
                moves.push({ 0, from, to });
            }

            // If the pawn is on the 2nd or 7th rank, it can move 2 squares
            if (move_direction == 1 && row == 1 ) {
                to = from >> 16;
                if (!(move_color.pawns.piece_arr & to)) {
                    moves.push({ 0, from, to });
                }
            }
        }
    }
}

MoveQueue get_board_moves(Board board) {
    MoveQueue moves = MoveQueue(MoveCompare());

    struct Color move_color = board.turn ? board.black : board.white;
    int move_direction = board.turn ? -1 : 1;
    
    // Pawns
    add_pawn_moves(moves, board, move_color, move_direction);

    return moves;
}