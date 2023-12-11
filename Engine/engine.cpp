#include <queue>
#include <vector>
#include <iostream>

#include "Board.h"
#include "PieceScores.h"
#include "engine.hpp"

using namespace std;

static u64 MoveCheckCounter = 0;

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

#define in_freeze_range(row, col, freeze_pos) \
    (freeze_pos != 64 && ( \
    ((freeze_pos % 8) - 1 <= col && col <= (freeze_pos % 8) + 1) && \
    ((freeze_pos / 8) - 1 <= row && row <= (freeze_pos / 8) + 1)))

#define insert_move(piece, moves, board, from, to, freeze_pos, expr) \
    Board* new_board = new Board; \
    memcpy(new_board, &board, sizeof(Board)); \
    new_board->turn = !board.turn; \
    Color &new_color = board.turn ? new_board->black : new_board->white; \
    Color &new_other_color = board.turn ? new_board->white : new_board->black; \
    const Color &old_color = board.turn ? board.black : board.white; \
    \
    new_color.piece.piece_arr = (old_color.piece.piece_arr & ~from) | to; \
    new_other_color.pawns.piece_arr   &= ~to; \
    new_other_color.knights.piece_arr &= ~to; \
    new_other_color.bishops.piece_arr &= ~to; \
    new_other_color.rooks.piece_arr   &= ~to; \
    new_other_color.queens.piece_arr  &= ~to; \
    new_other_color.king.piece_arr    &= ~to; \
    new_board->freeze_loc = freeze_pos; \
    if (freeze_pos == 64) { \
        new_color.freeze_spell -= new_color.freeze_spell ? 1 : 0;\
    } else {new_color.num_freeze_spells--; new_color.freeze_spell = 3;} \
    expr; \
    \
    i16 score = board_evaluate(*new_board); \
    moves.push({ score, *new_board, NULL}); \
    MoveCheckCounter++;

__attribute__((always_inline))
inline void add_pawn_moves(MoveQueue &moves, const Board board, const Color move_color, const Color other_color) {
    u64 from, to;
    u8 row, col;
    u64 all_squares = board_all_squares(board);
    // u64 self_color_pieces = color_pieces(move_color);
    u64 other_color_pieces = color_pieces(other_color);

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            from = 1ULL << (row * 8 + col);
            for (int freeze_pos = 0; freeze_pos < 65; freeze_pos++) {
                if (freeze_pos != 64 && (!(other_color_pieces & (1ULL << freeze_pos)) || move_color.num_freeze_spells == 0 || move_color.freeze_spell != 0)) {
                    continue;
                }
                if (!(move_color.pawns.piece_arr & from) || in_freeze_range(row, col, freeze_pos) || in_freeze_range(row, col, board.freeze_loc)) continue;

                // If there is not a piece in front of the pawn, it can move 1 square
                to = board.turn ? from << 8 : from >> 8;
                bool clear_front = !(all_squares & to);
                if (clear_front) {
                    insert_move(pawns, moves, board, from, to, freeze_pos, if (to < 256 || to > (1ULL << 8*7)) {new_color.pawns.row[row] = 0; new_color.queens.piece_arr |= to;});
                }

                // If the pawn is on the 2nd or 7th rank, it can move 2 squares
                if (((!board.turn && row == 6)  || (board.turn && row == 1)) && clear_front) {
                    to = board.turn ? from << 16 : from >> 16;
                    if (!(all_squares & to)) {
                        insert_move(pawns, moves, board, from, to, freeze_pos, ;);
                        new_color.en_passant = col + 1;
                    }
                }

                // If there is an enemy piece diagonally in front of the pawn, it can be captured
                if (col >= 0 && col != 7) {
                    // Can capture to the right
                    to = board.turn ? from << 9 : from >> 7;
                    bool enpassantable = (col == other_color.en_passant-2) && (row == (board.turn ? 4 : 5));
                    if ((other_color_pieces & to) || enpassantable) { // If there is an enemy piece to the right
                        insert_move(pawns, moves, board, from, to, freeze_pos, if (enpassantable) {new_other_color.pawns.piece_arr &= ~(from << 1);} if (to < 256 || to > (1ULL << 8*7)) {new_color.pawns.piece_arr &= ~to; new_color.queens.piece_arr |= to;}); // Pass by name...
                    }
                }
                if (col <= 7 && col != 0) {
                    // Can capture to the left
                    to = board.turn ? from << 7 : from >> 9;
                    bool enpassantable = col == other_color.en_passant && (row == (board.turn ? 4 : 5));
                    if ((other_color_pieces & to) || enpassantable) { // If there is an enemy piece to the left
                        insert_move(pawns, moves, board, from, to, freeze_pos, if (enpassantable) {new_other_color.pawns.piece_arr &= ~(from >> 1);} if (to < 256 || to > (1ULL << 8*7)) {new_color.pawns.piece_arr &= ~to; new_color.queens.piece_arr |= to;});
                    }
                }
            }
        }
    }
}

__attribute__((always_inline))
inline void add_knight_moves(MoveQueue &moves, const Board board, const Color move_color, const Color other_color) {
    u64 from, to;
    u8 row, col;

    u64 self_color_pieces = color_pieces(move_color);
    u64 other_color_pieces = color_pieces(other_color);

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            from = 1ULL << (row * 8 + col);
            for (int freeze_pos = 0; freeze_pos < 65; freeze_pos++) {
                if (freeze_pos != 64 && (!(other_color_pieces & (1ULL << freeze_pos)) || move_color.num_freeze_spells == 0 || move_color.freeze_spell != 0)) {
                    continue;
                }
                if (!(move_color.knights.piece_arr & from) || in_freeze_range(row, col, freeze_pos) || in_freeze_range(row, col, board.freeze_loc)) continue;

                // Up 2, right 1
                to = from >> 15;
                if (!(self_color_pieces & to) && row > 1 && col < 7) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Up 2, left 1
                to = from >> 17;
                if (!(self_color_pieces & to) && row > 1 && col > 0) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Up 1, right 2
                to = from >> 6;
                if (!(self_color_pieces & to) && row > 0 && col < 6) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Up 1, left 2
                to = from >> 10;
                if (!(self_color_pieces & to) && row > 0 && col > 1) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Down 1, right 2
                to = from << 10;
                if (!(self_color_pieces & to) && row < 7 && col < 6) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Down 1, left 2
                to = from << 6;
                if (!(self_color_pieces & to) && row < 7 && col > 1) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Down 2, right 1
                to = from << 17;
                if (!(self_color_pieces & to) && row < 6 && col < 7) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }

                // Down 2, left 1
                to = from << 15;
                if (!(self_color_pieces & to) && row < 6 && col > 0) {
                    insert_move(knights, moves, board, from, to, freeze_pos, ;);
                }
            }
        }
    }
}

__attribute__((always_inline))
inline void add_bishop_moves(MoveQueue &moves, const Board board, const Color move_color, const Color other_color) {
    u64 from, to;
    u8 row, col;
    int i, j;

    u64 self_color_pieces = color_pieces(move_color);
    u64 other_color_pieces = color_pieces(other_color);

    #define inserts \
        if (move_color.bishops.piece_arr & from) {insert_move(bishops, moves, board, from, to, freeze_pos, ;)} \
        else {insert_move(queens, moves, board, from, to, freeze_pos, ;)}

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            from = 1ULL << (row * 8 + col);
            for (int freeze_pos = 0; freeze_pos < 65; freeze_pos++) {
                if (freeze_pos != 64 && (!(other_color_pieces & (1ULL << freeze_pos)) || move_color.num_freeze_spells == 0 || move_color.freeze_spell != 0)) {
                    continue;
                }
                if (!((move_color.bishops.piece_arr & from) || (move_color.queens.piece_arr & from)) || in_freeze_range(row, col, freeze_pos) || in_freeze_range(row, col, board.freeze_loc)) continue;

                // Up right
                to = from >> 7;
                i = col; j = row;
                while (!(self_color_pieces & to) && !(other_color_pieces & to) && i < 7 && j > 0) {
                    inserts
                    to >>= 7;
                    i++; j--;
                }
                if (i < 7 && j > 0 && (other_color_pieces & to)) {
                    inserts
                }

                // Up left
                to = from >> 9;
                i = col; j = row;
                while (!(self_color_pieces & to) && !(other_color_pieces & to) && i > 0 && j > 0) {
                    inserts
                    to >>= 9;
                    i--; j--;
                }
                if (i > 0 && j > 0 && (other_color_pieces & to)) {
                    inserts
                }

                // Down right
                to = from << 9;
                i = col; j = row;
                while (!(self_color_pieces & to) && !(other_color_pieces & to) && i < 7 && j < 7) {
                    inserts
                    to <<= 9;
                    i++; j++;
                }
                if (i < 7 && j < 7 && (other_color_pieces & to)) {
                    inserts
                }

                // Down left
                to = from << 7;
                i = col; j = row;
                while (!(self_color_pieces & to) && !(other_color_pieces & to) && i > 0 && j < 7) {
                    inserts
                    to <<= 7;
                    i--; j++;
                }
                if (i > 0 && j < 7 && (other_color_pieces & to)) {
                    inserts
                }
            }
        }
    }
    #undef inserts
}

void get_board_moves(Move& move) {
    Board board = move.new_board;
    MoveQueue *moves = new MoveQueue(DynamicMoveCompare(board.turn));

    Color move_color = board.turn ? board.black : board.white;
    Color other_color = board.turn ? board.white : board.black;

    // cout << "Move color: " << (board.turn ? "black" : "white") << endl;
    
    // Pawns
    add_pawn_moves(*moves, board, move_color, other_color);

    // Knights
    add_knight_moves(*moves, board, move_color, other_color);

    // Bishops and Queens
    add_bishop_moves(*moves, board, move_color, other_color);

    move.child_moves = (void*) moves;
}

i16 minimax(Move& move, const int depth, i16 alpha, i16 beta, const bool maximizing_player) {
    if (!depth || move.new_board.white.king.piece_arr == 0 || move.new_board.black.king.piece_arr == 0) {
        return move.score;
    }

    if (move.child_moves == NULL) {
        get_board_moves(move);
    }

    MoveQueue moves = *(MoveQueue *) move.child_moves;
    Move possible_move;
    const u64 num_moves = moves.size();
    i16 eval, best_score = maximizing_player ? -32768 : 32767;

    if (num_moves == 0) {
        // cout << "No moves" << endl;
        return move.score;
    }

    if (maximizing_player) {
        for (u64 i = 0; i < num_moves; i++) {
            possible_move = moves.top();
            moves.pop();
            eval = minimax(possible_move, depth-1, alpha, beta, !maximizing_player);
            alpha = MAX(alpha, eval);
            // cout << "White's score: " << eval << ", Best Score: " << best_score << ", (Alpha, Beta): " << alpha << ',' << beta << endl;
            // board_print(&possible_move.new_board);
            // getchar();
            best_score = MAX(best_score, eval);
            delete (MoveQueue *) possible_move.child_moves;
            if (beta <= alpha) {
                break;
            }
        }
        return best_score;
    }
    for (u64 i = 0; i < num_moves; i++) {
        possible_move = moves.top();
        moves.pop();
        eval = minimax(possible_move, depth-1, alpha, beta, !maximizing_player);
        beta = MIN(beta, eval);
        // cout << "Black's score: " << eval << ", Best Score: " << best_score << ", (Alpha, Beta): " << alpha << ',' << beta << endl;
        // board_print(&possible_move.new_board);
        // getchar();
        best_score = MIN(best_score, eval);
        delete (MoveQueue *) possible_move.child_moves;
        if (beta <= alpha) {
            break;
        }
    }
    return best_score;
}

Move get_action(Board board) {
    Move move = {0, board, NULL};
    i16 alpha = -32768, beta = 32767, eval;
    i16 best_score = board.turn ? 32767 : -32768;
    bool maximizing_player = !board.turn;
    get_board_moves(move);

    MoveQueue moves = *(MoveQueue *) move.child_moves;
    Move best_action = moves.top();
    u64 num_moves = moves.size();
    
    if (maximizing_player) {
        for (u64 i = 0; i < num_moves; i++) {
            move = moves.top();
            moves.pop();
            eval = minimax(move, MAX_DEPTH, alpha, beta, !maximizing_player);
            if (eval > best_score) {
                best_action = move;
            }
            alpha = MAX(alpha, eval);
            // cout << "White's Score: " << eval << ", Best Score: " << best_score << ", (Alpha, Beta): " << alpha << ',' << beta << endl;
            // cout << (int) move.new_board.white.num_freeze_spells << ',' << (int) move.new_board.white.freeze_spell << endl;
            // board_print(&move.new_board);
            // getchar();
            best_score = MAX(best_score, eval);
            // if (beta <= alpha) {
            //     break;
            // }
        }
        best_action.score = best_score;
        return best_action;
    }
    for (u64 i = 0; i < num_moves; i++) {
        move = moves.top();
        moves.pop();
        // cout << "Black's move\n";
        // board_print(&move.new_board);
        // getchar();
        eval = minimax(move, MAX_DEPTH, alpha, beta, !maximizing_player);
        if (eval < best_score) {
            best_action = move;
        }
        best_score = MIN(best_score, eval);
        beta = MIN(beta, eval);
        if (beta <= alpha) {
            break;
        }
    }
    best_action.score = best_score;
    return best_action;
}