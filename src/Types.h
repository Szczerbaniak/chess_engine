#pragma once
#include <cstdint>
#include <bit>

using Bitboard = uint64_t;

enum Color : uint8_t { WHITE, BLACK, BOTH };
enum PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE };

enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8, NO_SQUARE
};

inline bool get_bit(Bitboard bb, Square sq) {
    return bb & (1ULL << sq);
}

inline void set_bit(Bitboard& bb, Square sq) {
    bb |= (1ULL << sq);
}

inline void pop_bit(Bitboard& bb, Square sq) {
    bb &= ~(1ULL << sq);
}

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard NOT_FILE_A = ~FILE_A;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;
constexpr Bitboard NOT_FILE_H = ~FILE_H;
constexpr Bitboard RANK_1 = 0x00000000000000FFULL;
constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;

inline bool on_file_a(Square sq) {
    return FILE_A & (1ULL << sq);
}

inline bool on_file_h(Square sq) {
    return FILE_H & (1ULL << sq);
}

inline bool on_rank_1(Square sq) {
    return RANK_1 & (1ULL << sq);
}

inline bool on_rank_8(Square sq) {
    return RANK_8 & (1ULL << sq);
}