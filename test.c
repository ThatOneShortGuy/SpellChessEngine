#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Board.c"

int main(int argc, char *argv[]) {
    Board board;
    board_setup(&board);
    board_print(&board);
    u64 n_positions = 20*129*20*129;
    u64 board_size = n_positions * sizeof board;
    printf("sizeof board.white = %llu\n", sizeof board.white);
    printf("sizeof board = %llu\n", sizeof board);
    printf("%llu * sizeof board = %llu = %llf MB\n", n_positions, board_size, (double)board_size / (1024*1024));
    
    Board *boards = malloc(board_size);
    for (u64 i = 0; i < n_positions; i++) board_setup(&boards[i]);

    char c = getchar();
    // u64 num = c;
    // printf("num = %d\n", num);
    // board_print(&boards[num]);

}