#pragma once
#include "Types.h"

inline Bitboard knight_attacks[64];
inline Bitboard king_attacks[64];
inline Bitboard pawn_attacks[2][64];

inline Bitboard mask_knight_attacks(Square sq);

inline Bitboard mask_king_attacks(Square sq);

inline Bitboard mask_pawn_attacks(Square sq, Color cl);

inline Bitboard mask_pawn_pushes(Square sq, Color cd, Bitboard occupancy);

inline Bitboard mask_bishop_attacks(Square sq, Bitboard occupancy);

inline Bitboard mask_rook_attacks(Square sq, Bitboard occupancy);

inline Bitboard mask_queen_attacks(Square sq, Bitboard occupancy);

inline void init_attacks();