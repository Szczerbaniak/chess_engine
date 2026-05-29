#include "Board.h"
#include "MoveGenerator.h"

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

void Board::generate_pseudo_legal_knight_moves(MoveList& move_list, Bitboard knight) {
    Color my_color = side_to_move;
    Color opponents_color = side_to_move ? WHITE : BLACK;

    std::array<Bitboard, 2> pieces_array = {white_pieces, black_pieces};

    while (knight) {
        int knight_field = std::countr_zero(knight);

        Bitboard attacks = knight_attacks[knight_field] & ~(pieces_array[my_color]);
        
        while (attacks) {
            int to_sq = std::countr_zero(attacks);
            uint8_t flag = (pieces_array[opponents_color] & (1ULL << to_sq)) ? MOVE_CAPTURE : MOVE_QUIET;
            
            move_list.push_back(make_move(Square(knight_field), Square(to_sq), flag));
            attacks &= (attacks - 1);
        }
        
        knight &= (knight - 1);
    }
}

void Board::generate_pseudo_legal_king_moves(MoveList& move_list, Bitboard king) {
    Color my_color = side_to_move;
    Color opponents_color = side_to_move ? WHITE : BLACK;

    std::array<Bitboard, 2> pieces_array = {white_pieces, black_pieces};

    while (king) {
        int king_field = std::countr_zero(king);
        Bitboard attacks = king_attacks[king_field] & ~(pieces_array[my_color]);

        while (attacks) {
            int to_sq = std::countr_zero(attacks);
            uint8_t flag = (pieces_array[opponents_color] & (1ULL << to_sq)) ? MOVE_CAPTURE : MOVE_QUIET;

            move_list.push_back(make_move(Square(king_field), Square(to_sq), flag));
            attacks &= (attacks - 1);
        }
        king &= (king - 1);

    }
}

void Board::generate_pseudo_legal_pawn_moves(MoveList& move_list, Bitboard pawn) {
    Color my_color = side_to_move;
    Color opponents_color = side_to_move ? WHITE : BLACK;

    std::array<Bitboard, 2> pieces_array = {white_pieces, black_pieces};

    while (pawn) {
        int pawn_field = std::countr_zero(pawn);
        Bitboard attacks = pawn_attacks[my_color][pawn_field] & pieces_array[opponents_color];

        Bitboard pawn_moves = mask_pawn_pushes(Square(pawn_field), my_color, all_pieces);
        int double_push_shift = (my_color == WHITE) ? 16 : -16;

        while (pawn_moves) {
            int to_sq = std::countr_zero(pawn_moves);
            int flag = ((to_sq - pawn_field) == double_push_shift) ? MOVE_PAWN_DOUBLE : MOVE_QUIET;

            bool is_promotion = (my_color == WHITE) ? (to_sq >= A8) : (to_sq <= H1);
            if (is_promotion)
            {
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_KNIGHT));
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_BISHOP));
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_ROOK));
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_QUEEN));
                        }
            else
            {
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), flag));
            }
            pawn_moves &= (pawn_moves - 1);
        }

        while (attacks) {
            int to_sq = std::countr_zero(attacks);

            bool is_promotion = (my_color == WHITE) ? (to_sq >= A8) : (to_sq <= H1);
            if (is_promotion)
            {
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_CAP_KNIGHT));
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_CAP_BISHOP));
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_CAP_ROOK));
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_PROMO_CAP_QUEEN));
                        }
            else
            {
                move_list.push_back(make_move(Square(pawn_field), Square(to_sq), MOVE_CAPTURE));
            }
            attacks &= (attacks - 1);
        }
        pawn &= (pawn - 1);
    }
}

void Board::generate_pseudo_legal_moves(MoveList& move_list) {
    Color my_color = side_to_move;
    

    Bitboard knight = pieces[my_color][KNIGHT];
    Bitboard king = pieces[my_color][KING];
    Bitboard pawn = pieces[my_color][PAWN];

    generate_pseudo_legal_knight_moves(move_list, knight);
    generate_pseudo_legal_king_moves(move_list, king);
    generate_pseudo_legal_pawn_moves(move_list, pawn);
}