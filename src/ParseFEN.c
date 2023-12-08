#include <stdio.h>
#include <stdlib.h>

#ifndef Board_H
#include "Board.h"
#endif

#ifndef PARSEFEN_H
#define PARSEFEN_H

#define LIST_OF_PIECES \
    X('r', black, rooks) \
    X('n', black, knights) \
    X('b', black, bishops) \
    X('q', black, queens) \
    X('k', black, king) \
    X('p', black, pawns) \
    X('R', white, rooks) \
    X('N', white, knights) \
    X('B', white, bishops) \
    X('Q', white, queens) \
    X('K', white, king) \
    X('P', white, pawns)

void parse_position(Board *board, char *position) {
    Color *black = &board->black;
    Color *white = &board->white;

    int i = 0;
    for (int parsing_index = 0; i < 64; parsing_index++) {
        switch (position[parsing_index]) {
            #define X(piece, color, name) \
            case piece: \
                color->name.piece_arr |= (1ULL << i); \
                break;
            LIST_OF_PIECES
            #undef X
            case '1' ... '8':
                i += position[parsing_index] - '0' - 1;
                break;
            case '/':
                break;
            default:
                printf("Invalid character while parsing position FEN in position (%i): %c\n", i, position[parsing_index]);
                exit(1);
        }
        if (position[parsing_index] != '/') ++i;
    }
}

void parse_turn(Board *board, char turn) {
    switch (turn) {
        case 'w':
            board->turn = 0;
            break;
        case 'b':
            board->turn = 1;
            break;
        default:
            printf("Invalid character while parsing turn FEN: %c\n", turn);
            exit(1);
    }
}

void parse_castling(Board *board, char *castling) {
    Color *black = &board->black;
    Color *white = &board->white;
    #define LIST_OF_CASTLING \
        X('K', white, 0b01) \
        X('Q', white, 0b10) \
        X('k', black, 0b01) \
        X('q', black, 0b10)
    
    if (castling[0] == '-') return; // No castling allowed

    for (int i = 0; i < 4; i++) {
        switch (castling[i]) {
            #define X(piece, color, value) \
            case piece: \
                color->castling |= value; \
                break;
            LIST_OF_CASTLING
            #undef X
            default:
                printf("Invalid character while parsing castling FEN: %c\n", castling[i]);
                exit(1);
        }
    }
}

void parse_enpassant(Board *board, char *enpassant) {
    if (enpassant[0] == '-') return; // No en passant allowed

    if (enpassant[0] < 'a' || enpassant[0] > 'h') {
        printf("Invalid column while parsing en passant FEN: %c\n", enpassant[0]);
        exit(1);
    }
    if (enpassant[1] != '3' && enpassant[1] != '6') {
        printf("Invalid row (can't enpassant on row %c)\n", enpassant[1]);
        exit(1);
    }

    if (enpassant[1] == '6') {
        board->black.en_passant = enpassant[0] - 'a' + 1;
    } else {
        board->white.en_passant = enpassant[0] - 'a' + 1;
    }
}

void parse_halfmoves(Board *board, char *halfmoves) {
    // fprintf(stderr, "Not using halfmoves\n");
}

void parse_fullmoves(Board *board, char *fullmoves) {
    // fprintf(stderr, "Not using fullmoves\n");
}

void parse_frozen(Board *board, char *frozen) {
    if (frozen[0] == '-') {
        board->freeze_loc = 64;
        return;
    }

    if (frozen[0] < 'a' || frozen[0] > 'h') {
        printf("Invalid column while parsing frozen FEN: %c\n", frozen[0]);
        exit(1);
    }
    if (frozen[1] < '1' || frozen[1] > '8') {
        printf("Invalid row while parsing frozen FEN: %c\n", frozen[1]);
        exit(1);
    }

    board->freeze_loc = (frozen[0] - 'a') + 8 * (frozen[1] - '1');
}

void parse_jumpable(Board *board, char *jumpable) {
    if (jumpable[0] == '-') {
        board->jump_loc = 64;
        return;
    }

    if (jumpable[0] < 'a' || jumpable[0] > 'h') {
        printf("Invalid column while parsing jumpable FEN: %c\n", jumpable[0]);
        exit(1);
    }
    if (jumpable[1] < '1' || jumpable[1] > '8') {
        printf("Invalid row while parsing jumpable FEN: %c\n", jumpable[1]);
        exit(1);
    }

    board->jump_loc = (jumpable[0] - 'a') + 8 * (jumpable[1] - '1');

}

void parse_spells(Board *board, char *spells) {
    board->white.num_freeze_spells = spells[0] - '0';
    board->white.num_jump_spells = spells[1] - '0';
    board->black.num_freeze_spells = spells[2] - '0';
    board->black.num_jump_spells = spells[3] - '0';
}

void parse_waiting_spells(Board *board, char *waiting_spells) {
    board->white.freeze_spell = waiting_spells[0] - '0';
    board->white.jump_spell = waiting_spells[1] - '0';
    board->black.freeze_spell = waiting_spells[2] - '0';
    board->black.jump_spell = waiting_spells[3] - '0';
}

#endif