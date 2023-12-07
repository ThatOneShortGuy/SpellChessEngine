#include <queue>
#include <vector>

#include "Board.h"
typedef int16_t i16;

typedef struct {
    i16 score;
    Board new_board;
} Move;

struct MoveCompare {
    bool operator()
        (const Move a, const Move b) const
    {
        return a.score < b.score;
    }
};

typedef std::priority_queue<Move, std::vector<Move>, MoveCompare> MoveQueue;

i16 board_evaluate(Board board);
MoveQueue get_board_moves(Board board);
