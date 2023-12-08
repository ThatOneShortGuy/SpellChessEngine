#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <chrono>

#include "engine.hpp"
#include "Board.h"

int main(int argc, char *argv[]) {
    if (argc != 7 && argc != 11) {
        printf("Usage: %s <fen> [<frozen> <jumpable> <spells> <waiting_spells>]\nGot %i params, expected 6 or 10\n", argv[0], argc);
        for (int i = 1; i < argc; i++) {
            if (i < 10)       printf("%i  | %s\n", i, argv[i]);
            else if (i < 100) printf("%i | %s\n", i, argv[i]);
            else              printf("%i| %s\n", i, argv[i]);
        }
        return 1;
    }

    Board board; // Initialize board

    board_from_fen(&board, argv[1], argv[2][0], argv[3], argv[4], argv[5], argv[6]); // Parse FEN into board

    if (argc == 11) {
        board_add_spells_from_fen(&board, argv[7], argv[8], argv[9], argv[10]); // Parse spells into board
    }

    board_print(&board);
    u16 eval = board_evaluate(board);
    printf("Evaluation: %i\n", eval);
    auto start = std::chrono::high_resolution_clock::now();
    MoveQueue moves = get_board_moves(board);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    printf("Time: %lli ns\n", duration);
    getchar();
    Move move;
    u64 num_moves = moves.size();
    for (u64 i = 0; i < num_moves; i++) {
        move = moves.top();
        printf("Move Score: %i\n", move.score);
        board_print(&move.new_board);
        moves.pop();
        getchar();
    }
}