#pragma once

#include "Types.h"
enum GamePhase { PHASE_EG = 0, PHASE_TR = 1, PHASE_MG = 2 };

struct PhaseValue {
    int mg; // Middlegame
    int tr; // Threshold
    int eg; // Endgame

    constexpr PhaseValue() : mg(0), tr(0), eg(0) {}
    constexpr PhaseValue(int m, int t, int e) : mg(m), tr(t), eg(e) {}
};

inline PhaseValue PST_TABLES[6][64] = { PhaseValue(0,0,0) };


constexpr PhaseValue KAUFMAN_PIECE_VALUES[6] = {
    PhaseValue(0,   0,   0),   // pion (obsługiwany osobną tablicą)
    PhaseValue(320, 320, 320), // kon
    PhaseValue(330, 330, 330), // goniec
    PhaseValue(470, 480, 530), // wieza
    PhaseValue(940, 940, 0),   // krolowa
    PhaseValue(0,   0,   0)    // krol
};

// Dedykowana tablica dla pionków na poszczególnych liniach (a-h)
constexpr PhaseValue KAUFMAN_PAWN_VALUES_BY_FILE[8] = {
    PhaseValue(70,  90, 100), // Linia A
    PhaseValue(85,  90, 100), // Linia B
    PhaseValue(95,  90, 100), // Linia C
    PhaseValue(100, 90, 100), // Linia D
    PhaseValue(100, 90, 100), // Linia E
    PhaseValue(95,  90, 100), // Linia F
    PhaseValue(85,  90, 100), // Linia G
    PhaseValue(70,  90, 100)  // Linia H
};

enum BonusType { BISHOP_PAIR, SECOND_ROOK, SECOND_QUEEN, KNIGHT_PAIR};
constexpr PhaseValue KAUFMAN_PIECE_VALUES_BONUSES[4]{
    PhaseValue(360, 370, 380),    // para goncow
    PhaseValue(450, 490, 500), // wieza x2
    PhaseValue(870, 870, 0),     // krolowa x2
    PhaseValue(320, 310, 300),     // para skoczkow
};

constexpr int PHASE_WEIGHT_BISHOP = 1;
constexpr int PHASE_WEIGHT_KNIGHT = 1;
constexpr int PHASE_WEIGHT_ROOK   = 2;
constexpr int PHASE_WEIGHT_QUEEN  = 4;
constexpr int MAX_PHASE = 24;