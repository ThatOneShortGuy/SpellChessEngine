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

    // board_print(&board);
    i16 eval = board_evaluate(board);
    printf("Evaluation: %i\n", eval);
    Move move = {eval, board, NULL, NULL};

    auto start = std::chrono::high_resolution_clock::now();
    Move best_move = minimax(move, 0, -32768, 32767, !board.turn);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    printf("Time: %f ms\n", ((double) duration)/1000/1000);
    // getchar();
    printf("Best move: %i\n", best_move.score);
    board_print(&best_move.new_board);
}