#pragma once
#include <array>
#include "Types.h"

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