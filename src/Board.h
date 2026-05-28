#pragma once
#include <array>
#include "Types.h"

// Jak zmieścić ruch w 16 bitach?
// Bity 0–5 (6 bitów): Pole startowe from
// Bity 6–11 (6 bitów): Pole docelowe to
// Bity 12–15 (4 bity): Specjalne flagi ruchu (roszady, bicia, promocje).

class Board {
public:
    std::array<std::array<Bitboard, 6>, 2> pieces; 
    Bitboard white_pieces;
    Bitboard black_pieces;
    Bitboard all_pieces;

    Color side_to_move;
    Square en_passant;
    //white: short long black:short long
    uint8_t castling_rights;

    void clear();
    void load_start_position();

    Board() { clear(); };
    ~Board();

};