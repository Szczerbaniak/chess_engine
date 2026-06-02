#include "EvaluationValues.h"
#include "Board.h"
#include <bit>
struct Score {
    int mg = 0;
    int tr = 0;
    int eg = 0;

    constexpr Score() : mg(0), tr(0), eg(0) {}
    constexpr Score(int m, int t, int e) : mg(m), tr(t), eg(e) {}
    constexpr int interpolate(int game_phase) {
        int final_score;
        if (game_phase > 12) {
            int factor = game_phase - 12;
            final_score = (mg * factor + tr * (12 - factor)) / game_phase;
        } else {
            final_score = (eg * game_phase + tr * (12 - game_phase)) / game_phase;
        }

        return final_score;
    };
};

void init_evaluation_tables();
int evaluate_position_for_active_player(const Board& board);