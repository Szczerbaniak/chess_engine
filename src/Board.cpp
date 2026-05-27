#include "Board.h"

void Board::clear() {
    pieces = {};
    white_pieces = 0;
    black_pieces = 0;
    all_pieces = 0;

    side_to_move = WHITE;
    en_passant = NO_SQUARE;
    castling_rights = 0;

}

void Board::load_start_position() {
    clear();

    pieces[WHITE][PAWN] = (1ULL << A2) | (1ULL << B2) | (1ULL << C2) | (1ULL << D2) | (1ULL << E2) | (1ULL << F2) | (1ULL << G2) | (1ULL << H2);
    pieces[BLACK][PAWN] = (1ULL << A7) | (1ULL << B7) | (1ULL << C7) | (1ULL << D7) | (1ULL << E7) | (1ULL << F7)  | (1ULL << G7) | (1ULL << H7);
    pieces[WHITE][ROOK] = (1ULL << A1) | (1ULL << H1);
    pieces[BLACK][ROOK] = (1ULL << A8) | (1ULL << H8);
    pieces[WHITE][KNIGHT] = (1ULL << B1) | (1ULL << G1);
    pieces[BLACK][KNIGHT] = (1ULL << B8) | (1ULL << G8);
    pieces[WHITE][BISHOP] = (1ULL << C1) | (1ULL << F1);
    pieces[BLACK][BISHOP] = (1ULL << C8) | (1ULL << F8);
    pieces[WHITE][QUEEN] = (1ULL << D1);
    pieces[BLACK][QUEEN] = (1ULL << D8);
    pieces[WHITE][KING] = (1ULL << E1);
    pieces[BLACK][KING] = (1ULL << E8);

    for (int type = PAWN; type <= KING; type++) {
        white_pieces |= pieces[WHITE][type];
        black_pieces |= pieces[BLACK][type];
    }

    all_pieces = white_pieces | black_pieces;

    castling_rights = 0b1111;
}