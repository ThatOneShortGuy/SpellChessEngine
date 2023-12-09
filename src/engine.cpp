#include <queue>
#include <vector>
#include <iostream>

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
    i16 white_king_score = board.white.king.piece_arr ? 10000 : 0;
    i16 black_king_score = board.black.king.piece_arr ? 10000 : 0;

    i16 pawn_score = white_pawn_score - black_pawn_score;
    i16 knight_bishop_score = white_knight_bishop_score - black_knight_bishop_score;
    i16 rook_score = white_rook_score - black_rook_score;
    i16 queen_score = white_queen_score - black_queen_score;
    i16 king_score = white_king_score - black_king_score;

    i16 jump_score = 400 * (board.white.num_jump_spells - board.black.num_jump_spells);
    i16 freeze_score = 250 * (board.white.num_freeze_spells - board.black.num_freeze_spells);
    return (pawn_score + knight_bishop_score) + (rook_score + queen_score)
        + (jump_score + freeze_score) + king_score;
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
    i16 score = board_evaluate(*new_board); \
    moves.push({ score, *new_board, (void *) &board, NULL}); \
    MoveCheckCounter++;

__attribute__((always_inline))
inline void add_pawn_moves(MoveQueue &moves, Board board, const Color move_color, const Color other_color) {
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
                insert_move(pawns, moves, board, from, to, ;);
            }

            // If the pawn is on the 2nd or 7th rank, it can move 2 squares
            if (((!board.turn && row == 6)  || (board.turn && row == 1)) && clear_front) {
                to = board.turn ? from << 16 : from >> 16;
                if (!(move_color.pawns.piece_arr & to)) {
                    board.turn ? board.black.en_passant = col : board.white.en_passant = col;
                    insert_move(pawns, moves, board, from, to, ;);
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

void get_board_moves(Move& move) {
    Board board = move.new_board;
    MoveQueue *moves = new MoveQueue(DynamicMoveCompare(board.turn));

    Color move_color = board.turn ? board.black : board.white;
    Color other_color = board.turn ? board.white : board.black;
    
    // Pawns
    add_pawn_moves(*moves, board, move_color, other_color);

    move.child_moves = (void*) moves;
}

Move minimax(Move move, const int depth, i16 alpha, i16 beta, const bool maximizing_player) {
    if (MoveCheckCounter > MAX_MOVES) {
        return move;
    }
    if (depth > MaxDepth) {
        MaxDepth = depth;
        cout << "MaxDepth: " << MaxDepth << endl;
    }

    if (move.child_moves == NULL) {
        get_board_moves(move);
    }

    MoveQueue moves = *(MoveQueue *) move.child_moves;
    Move next_move;
    Move best_move;
    best_move.score = maximizing_player ? -32768 : 32767;
    const u64 num_moves = moves.size();

    if (num_moves == 0) {
        cout << "No moves" << endl;
        return move;
    }

    if (maximizing_player) {
        for (u64 i = 0; i < num_moves/3; i++) {
            next_move = moves.top();
            moves.pop();
            next_move = minimax(next_move, depth+1, alpha, beta, !maximizing_player);

            best_move = next_move.score > best_move.score ? next_move: best_move;

            alpha = MAX(alpha, best_move.score);
            if (beta <= alpha) {
                break;
            }
        }
        return best_move;
    }
    else {
        for (u64 i = 0; i < num_moves/3; i++) {
            next_move = moves.top();
            moves.pop();
            next_move = minimax(next_move, depth+1, alpha, beta, !maximizing_player);

            best_move = next_move.score < best_move.score ? next_move: best_move;

            beta = MIN(beta, best_move.score);
            if (beta <= alpha) {
                break;
            }
        }
        return best_move;
    }
}