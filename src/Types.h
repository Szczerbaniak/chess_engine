#pragma once
#include <cstdint>
#include <bit>
#include <array>


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

inline bool get_bit(Bitboard& bb, Square sq) {
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
constexpr Bitboard FILE_B = 0x0202020202020202ULL;
constexpr Bitboard NOT_FILE_B = ~FILE_B;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;
constexpr Bitboard NOT_FILE_H = ~FILE_H;
constexpr Bitboard FILE_G = 0x4040404040404040ULL;
constexpr Bitboard NOT_FILE_G = ~FILE_G;

constexpr Bitboard NOT_FILE_AB = ~(FILE_A | FILE_B);
constexpr Bitboard NOT_FILE_GH = ~(FILE_G | FILE_H);

constexpr Bitboard RANK_1 = 0x00FFULL;
constexpr Bitboard RANK_8 = 0xFF00ULL;

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

//flagi:
//ruchy bez bicia
constexpr uint8_t MOVE_QUIET = 0b0000;
constexpr uint8_t MOVE_PAWN_DOUBLE = 0b0001;
constexpr uint8_t MOVE_KING_CASTLE = 0b0010;
constexpr uint8_t MOVE_QUEEN_CASTLE = 0b0011;
constexpr uint8_t MOVE_CAPTURE = 0b0100;
constexpr uint8_t MOVE_EN_PASSANT = 0b0101;

// Promocje pionka (bity 3 i 4 oznaczają, że to promocja, a dwa najmłodsze to typ figury)
constexpr uint8_t MOVE_PROMO_KNIGHT = 0b1000;
constexpr uint8_t MOVE_PROMO_BISHOP = 0b1001;
constexpr uint8_t MOVE_PROMO_ROOK = 0b1010;
constexpr uint8_t MOVE_PROMO_QUEEN = 0b1011;

// Promocja z jednoczesnym biciem
constexpr uint8_t MOVE_PROMO_CAP_KNIGHT = 0b1100; 
constexpr uint8_t MOVE_PROMO_CAP_BISHOP = 0b1101;
constexpr uint8_t MOVE_PROMO_CAP_ROOK = 0b1110;
constexpr uint8_t MOVE_PROMO_CAP_QUEEN = 0b1111;

struct Move
{
    uint16_t data;

    inline Square from() const { return Square(data & 0b00111111); }
    inline Square to() const { return Square((data >> 6) & 0b00111111); }
    inline uint8_t flag() const { return ((data >> 12) & 0b1111); }
};

inline Move make_move(Square from, Square to, uint8_t flag) {
    return Move{ static_cast<uint16_t>(flag << 12 | to << 6 | from) };
}

struct MoveList {
    // Górna granica ilosci ruchów to:
    // 9 hetmanów x 21 ruchów = 189
    // 2 wieże x 14 ruchów = 28
    // 2 gońce x 13 ruchów = 26
    // 2 skoczki x 8 ruchów = 16
    // 1 król x 8 ruchów = 8
    // = 267, jest to wynik nieosiągalny i realny znajduje się w okolicy 70 (max jaki jest osiągalny w warunkach laboratoryjnych hehe, to 118) - dlatego możemy sobie pozwolić na deklarację tablicy 256 elementowej (gdzie każdy ruch zajmuje 2 bajty) zamiast vectora, który w niektórych sytuacjach mógłby być wolny

    std::array<Move, 265> moves;
    int count = 0;

    inline void push_back(Move mv) {
        moves[count] = mv;
        count += 1;
    }

    inline Move* begin() {
        return &moves[0];
    }

    inline Move* end() {
        return &moves[count];
    }
};