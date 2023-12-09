#include <queue>
#include <vector>

#include "Board.h"
typedef int16_t i16;

#define MAX_MOVES 5'000'000
static u64 MoveCheckCounter = 0;
static int MaxDepth = 0;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))

typedef struct {
    i16 score;
    Board new_board;
    void* parent_move; // Move*
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
Move minimax(Move move, int depth, i16 alpha, i16 beta, bool maximizing_player);
