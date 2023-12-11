#include <queue>
#include <vector>

#include "Board.h"
typedef int16_t i16;

#define MAX_MOVES 100'000'000
#define MAX_DEPTH 1

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))

typedef struct {
    i16 score;
    Board new_board;
    void* child_moves; // MoveQueue*
} Move;

struct DynamicMoveCompare {
    bool isBlack;
    DynamicMoveCompare(bool isBlack) : isBlack(isBlack) {}

    bool operator() (const Move& a, const Move& b) const {
        return isBlack ? a.score > b.score : a.score < b.score;
    }
};

typedef std::priority_queue<Move, std::vector<Move>, DynamicMoveCompare> MoveQueue;

i16 board_evaluate(Board board);
void get_board_moves(Move& move);
Move get_action(Board board);
