#include <queue>
#include <vector>
#include <iostream>
#include <functional>

#include "Board.h"
#include "PieceScores.h"
#include "engine.hpp"

using namespace std;

__attribute__((always_inline))
inline i16 get_score(Positions position, const int score_array[8][8], const int base_value) {
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

    i16 jump_score = 400 * (board.white.num_jump_spells - board.black.num_jump_spells);
    i16 freeze_score = 250 * (board.white.num_freeze_spells - board.black.num_freeze_spells);
    return (pawn_score + knight_bishop_score) + (rook_score + queen_score)
        + (jump_score + freeze_score);
}

#define insert_move(piece, moves, board, from, to, expr) \
    Board* new_board = new Board; \
    memcpy(new_board, &board, sizeof(Board)); \
    new_board->turn = !board.turn; \
    Color &new_color = board.turn ? new_board->black : new_board->white; \
    Color &new_other_color = board.turn ? new_board->white : new_board->black; \
    Color &old_color = board.turn ? board.black : board.white; \
    \
    new_color.piece.piece_arr = (old_color.piece.piece_arr & ~from) | to; \
    new_other_color.pawns.piece_arr   &= ~to; \
    new_other_color.knights.piece_arr &= ~to; \
    new_other_color.bishops.piece_arr &= ~to; \
    new_other_color.rooks.piece_arr   &= ~to; \
    new_other_color.queens.piece_arr  &= ~to; \
    new_other_color.king.piece_arr    &= ~to; \
    expr; \
    \
    i16 score = board_evaluate(*new_board) * (board.turn ? -1 : 1); \
    moves.push({ score, *new_board });

__attribute__((always_inline))
inline void add_pawn_moves(MoveQueue &moves, Board board, const Color move_color, const Color other_color, const int move_direction) {
    u64 from, to;
    u8 row, col;
    u64 all_squares = board_all_squares(board);
    // u64 self_color_pieces = color_pieces(move_color);
    u64 other_color_pieces = color_pieces(other_color);

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            from = 1ULL << (row * 8 + col);
            if (!(move_color.pawns.piece_arr & from)) continue;

            // If there is not a piece in front of the pawn, it can move 1 square
            to = board.turn ? from << 8 : from >> 8;
            bool clear_front = !(all_squares & to);
            if (clear_front) {
                insert_move(pawns, moves, board, from, to, 0);
            }

            // If the pawn is on the 2nd or 7th rank, it can move 2 squares
            if (((move_direction == 1 && row == 6)  || (move_direction == -1 && row == 1)) && clear_front) {
                to = board.turn ? from << 16 : from >> 16;
                if (!(move_color.pawns.piece_arr & to)) {
                    board.turn ? board.black.en_passant = col : board.white.en_passant = col;
                    insert_move(pawns, moves, board, from, to, 0);
                    board.turn ? board.black.en_passant = 0 : board.white.en_passant = 0;
                }
            }

            // If there is an enemy piece diagonally in front of the pawn, it can be captured
            if (col >= 0 && col != 7) {
                // Can capture to the right
                to = board.turn ? from << 9 : from >> 7;
                bool enpassantable = col == other_color.en_passant-2;
                if ((other_color_pieces & to) || enpassantable) { // If there is an enemy piece to the right
                    insert_move(pawns, moves, board, from, to, if (enpassantable) new_other_color.pawns.piece_arr &= ~(from << 1)); // Pass by name...
                }
            }
            if (col <= 7 && col != 0) {
                // Can capture to the left
                to = board.turn ? from << 7 : from >> 9;
                bool enpassantable = col == other_color.en_passant;
                if ((other_color_pieces & to) || enpassantable) { // If there is an enemy piece to the left
                    insert_move(pawns, moves, board, from, to, if (enpassantable) new_other_color.pawns.piece_arr &= ~(from >> 1));
                }
            }
        }
    }
}

MoveQueue get_board_moves(Board board) {
    MoveQueue moves = MoveQueue(MoveCompare());

    Color move_color = board.turn ? board.black : board.white;
    Color other_color = board.turn ? board.white : board.black;
    int move_direction = board.turn ? -1 : 1;
    
    // Pawns
    add_pawn_moves(moves, board, move_color, other_color, move_direction);

    return moves;
}