#pragma once
#include <array>
#include <bit>
#include <algorithm>
#include <stdexcept>
#include "Types.h"

struct UndoState {
    uint8_t castling_rights;
    Square en_passant;
    PieceType captured_piece; 
};

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
    ~Board() {};

    void generate_pseudo_legal_moves(MoveList &move_list);
    void generate_pseudo_legal_knight_moves(MoveList &move_list, Bitboard knight);
    void generate_pseudo_legal_king_moves(MoveList &move_list, Bitboard king);
    void generate_pseudo_legal_pawn_moves(MoveList &move_list, Bitboard piece);
    void generate_pseudo_legal_bishop_moves(MoveList &move_list, Bitboard bishop);
    void generate_pseudo_legal_rook_moves(MoveList &move_list, Bitboard rook);
    void generate_pseudo_legal_queen_moves(MoveList &move_list, Bitboard queen);

    void unmake_move(Move move, const UndoState &state);
    bool is_square_attacked(Square sq, Color attacker);
    bool make_move_on_board(Move move, UndoState &state);
};