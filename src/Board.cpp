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

    //roszady
    int my_rights = (my_color == WHITE) ? (castling_rights & 0b0011) : (castling_rights & 0b1100);

    if (my_rights != 0) {
        if (my_color == WHITE && (my_rights & 1) && !(all_pieces & ((1ULL << F1) | (1ULL << G1)))) {
            move_list.push_back(make_move(E1, G1, MOVE_KING_CASTLE));
        }
        if (my_color == WHITE && (my_rights & 2) && !(all_pieces & ((1ULL << B1) | (1ULL << C1) | (1ULL << D1)))) {
            move_list.push_back(make_move(E1, C1, MOVE_QUEEN_CASTLE));
        }
        if (my_color == BLACK && (my_rights & 4) && !(all_pieces & ((1ULL << F8) | (1ULL << G8)))) {
            move_list.push_back(make_move(E8, G8, MOVE_KING_CASTLE));
        }
        if (my_color == BLACK && (my_rights & 8) && !(all_pieces & ((1ULL << B8) | (1ULL << C8) | (1ULL << D8)))) {
            move_list.push_back(make_move(E8, C8, MOVE_QUEEN_CASTLE));
        }
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

    if (en_passant != NO_SQUARE) {

        Bitboard ep_candidates = pawn_attacks[opponents_color][en_passant] & pieces[my_color][PAWN];
    
        while (ep_candidates) {
            int from_sq = std::countr_zero(ep_candidates);
            

            move_list.push_back(make_move(Square(from_sq), Square(en_passant), MOVE_EN_PASSANT));
            
            ep_candidates &= (ep_candidates - 1);
        }
    }
}

void Board::generate_pseudo_legal_bishop_moves(MoveList &move_list, Bitboard bishop) {
    Color my_color = side_to_move;
    Color opponents_color = side_to_move ? WHITE : BLACK;

    std::array<Bitboard, 2> pieces_array = {white_pieces, black_pieces};

    while (bishop)
    {
        int bishop_field = std::countr_zero(bishop);
        Bitboard attacks = mask_bishop_attacks(Square(bishop_field), all_pieces) & ~(pieces_array[my_color]);

        while (attacks) {
            int to_sq = std::countr_zero(attacks);
            uint8_t flag = (pieces_array[opponents_color] & (1ULL << to_sq)) ? MOVE_CAPTURE : MOVE_QUIET;

            move_list.push_back(make_move(Square(bishop_field), Square(to_sq), flag));
            attacks &= (attacks - 1);
        }
        bishop &= (bishop - 1);

    }
    
}

void Board::generate_pseudo_legal_rook_moves(MoveList &move_list, Bitboard rook) {
    Color my_color = side_to_move;
    Color opponents_color = side_to_move ? WHITE : BLACK;

    std::array<Bitboard, 2> pieces_array = {white_pieces, black_pieces};

    while (rook)
    {
        int rook_field = std::countr_zero(rook);
        Bitboard attacks = mask_rook_attacks(Square(rook_field), all_pieces) & ~(pieces_array[my_color]);

        while (attacks) {
            int to_sq = std::countr_zero(attacks);
            uint8_t flag = (pieces_array[opponents_color] & (1ULL << to_sq)) ? MOVE_CAPTURE : MOVE_QUIET;

            move_list.push_back(make_move(Square(rook_field), Square(to_sq), flag));
            attacks &= (attacks - 1);
        }
        rook &= (rook - 1);

    }
    
}

void Board::generate_pseudo_legal_queen_moves(MoveList &move_list, Bitboard queen) {
    Color my_color = side_to_move;
    Color opponents_color = side_to_move ? WHITE : BLACK;

    std::array<Bitboard, 2> pieces_array = {white_pieces, black_pieces};

    while (queen)
    {
        int queen_field = std::countr_zero(queen);
        Bitboard attacks = mask_queen_attacks(Square(queen_field), all_pieces) & ~(pieces_array[my_color]);

        while (attacks) {
            int to_sq = std::countr_zero(attacks);
            uint8_t flag = (pieces_array[opponents_color] & (1ULL << to_sq)) ? MOVE_CAPTURE : MOVE_QUIET;

            move_list.push_back(make_move(Square(queen_field), Square(to_sq), flag));
            attacks &= (attacks - 1);
        }
        queen &= (queen - 1);

    }
    
}

void Board::generate_pseudo_legal_moves(MoveList& move_list) {
    Color my_color = side_to_move;

    Bitboard knight = pieces[my_color][KNIGHT];
    Bitboard king   = pieces[my_color][KING];
    Bitboard pawn   = pieces[my_color][PAWN];
    Bitboard bishop = pieces[my_color][BISHOP];
    Bitboard rook   = pieces[my_color][ROOK];
    Bitboard queen  = pieces[my_color][QUEEN];

    generate_pseudo_legal_knight_moves(move_list, knight);
    generate_pseudo_legal_king_moves(move_list, king);
    generate_pseudo_legal_pawn_moves(move_list, pawn);
    generate_pseudo_legal_bishop_moves(move_list, bishop);
    generate_pseudo_legal_rook_moves(move_list, rook);
    generate_pseudo_legal_queen_moves(move_list, queen);
}

bool Board::is_square_attacked(Square sq, Color attacker) {

    if (mask_knight_attacks(sq) & pieces[attacker][KNIGHT])
    {
        return true;
    } else if (mask_king_attacks(sq) & pieces[attacker][KING]) {
        return true;
    }
    else if (mask_pawn_attacks(sq, (attacker == WHITE) ? BLACK : WHITE) & pieces[attacker][PAWN]) {
        return true;        
    } else if (mask_bishop_attacks(sq, all_pieces) & (pieces[attacker][BISHOP] | pieces[attacker][QUEEN]))
    {
        return true;
    } else if (mask_rook_attacks(sq, all_pieces) & (pieces[attacker][ROOK] | pieces[attacker][QUEEN]))
    {
        return true;
    }

    return false;
}

bool Board::make_move_on_board(Move move, UndoState& state) {
    Square from = move.from();
    Square to = move.to();
    uint8_t flag = move.flag();
    
    Color my_color = side_to_move;
    Color opponent = (my_color == WHITE) ? BLACK : WHITE;

    state.castling_rights = castling_rights;
    state.en_passant = en_passant;
    state.captured_piece = NONE;

    PieceType moving_piece;

    for (int piece = PAWN; piece <= KING; piece++) {
        if (pieces[my_color][piece] & (1ULL<< from)) {
            moving_piece = PieceType(piece);
            break;
        }
    }

    if (moving_piece == NONE) {
        return false; 
    }

    bool capture = (flag == MOVE_CAPTURE) || (flag == MOVE_EN_PASSANT) || (flag >= MOVE_PROMO_CAP_KNIGHT);    bool is_en_passant = flag == MOVE_EN_PASSANT;
    bool promotion = flag & 0b1000;

    

    if (capture) {
        if (is_en_passant) {
            state.captured_piece = PAWN;
            Square ep_pawn_sq = (my_color == WHITE) ? Square(to - 8) : Square(to + 8);
            pieces[opponent][PAWN] ^= (1ULL << ep_pawn_sq);
        } else {
            for (int piece = PAWN; piece <= KING; piece++) {
                if (pieces[opponent][piece] & (1ULL << to)) {
                    state.captured_piece = PieceType(piece);
                    pieces[opponent][piece] ^= (1ULL << to);
                    break;
                }
            }
        }
    }
    pieces[my_color][moving_piece] ^= (1ULL << from);
    pieces[my_color][moving_piece] ^= (1ULL << to);

    if (flag & 0b1000) { 
        PieceType new_piece = PieceType((flag & 0b0011) + 1);
    
        pieces[my_color][PAWN] ^= (1ULL << to);
        pieces[my_color][new_piece] ^= (1ULL << to);
    }

    if (flag == MOVE_KING_CASTLE) {
        if (my_color == WHITE) {
            pieces[WHITE][ROOK] ^= (1ULL << H1) | (1ULL << F1); // Wieża z H1 na F1
        } else {
            pieces[BLACK][ROOK] ^= (1ULL << H8) | (1ULL << F8); // Wieża z H8 na F8
        }
    } 
    else if (flag == MOVE_QUEEN_CASTLE) {
        if (my_color == WHITE) {
            pieces[WHITE][ROOK] ^= (1ULL << A1) | (1ULL << D1); // Wieża z A1 na D1
        } else {
            pieces[BLACK][ROOK] ^= (1ULL << A8) | (1ULL << D8); // Wieża z A8 na D8
        }
    }
    
    if (flag == MOVE_PAWN_DOUBLE) {
        en_passant = (my_color == WHITE) ? Square(from + 8) : Square(from - 8);
    } else {
        en_passant = NO_SQUARE;
    }

    if (from == E1 || to == E1) castling_rights &= ~0b0011;
    if (from == E8 || to == E8) castling_rights &= ~0b1100;
    if (from == H1 || to == H1) castling_rights &= ~0b0001;
    if (from == A1 || to == A1) castling_rights &= ~0b0010;
    if (from == H8 || to == H8) castling_rights &= ~0b0100;
    if (from == A8 || to == A8) castling_rights &= ~0b1000;

    white_pieces = 0;
    black_pieces = 0;
    for (int p = PAWN; p <= KING; p++) {
        white_pieces |= pieces[WHITE][p];
        black_pieces |= pieces[BLACK][p];
    }
    all_pieces = white_pieces | black_pieces;

    side_to_move = opponent;

    int king_sq = std::countr_zero(pieces[my_color][KING]);

    // Sprawdzanie legalności roszady na poszczególnych polach
    if (flag == MOVE_KING_CASTLE) {
        Square start_sq = (my_color == WHITE) ? E1 : E8;
        Square passing_sq = (my_color == WHITE) ? F1 : F8;
        if (is_square_attacked(start_sq, opponent) || is_square_attacked(passing_sq, opponent)) {
            unmake_move(move, state);
            return false;
        }
    } else if (flag == MOVE_QUEEN_CASTLE) {
        Square start_sq = (my_color == WHITE) ? E1 : E8;
        Square passing_sq = (my_color == WHITE) ? D1 : D8;
        if (is_square_attacked(start_sq, opponent) || is_square_attacked(passing_sq, opponent)) {
            unmake_move(move, state);
            return false;
        }
    }

    // Ogólne sprawdzanie szacha
    if (is_square_attacked(Square(king_sq), opponent)) {
        unmake_move(move, state);
        return false;
    }

    return true;
}

void Board::unmake_move(Move move, const UndoState& state) {
    Square from = move.from();
    Square to = move.to();
    uint8_t flag = move.flag();
    
    Color opponent = side_to_move; 
    Color my_color = (opponent == WHITE) ? BLACK : WHITE;

    int moving_piece = -1;
    if (flag & 0b1000) {
        static const PieceType promo_pieces[] = { KNIGHT, BISHOP, ROOK, QUEEN };
        moving_piece = promo_pieces[flag & 0b0011];
        pieces[my_color][moving_piece] ^= (1ULL << to); 
        moving_piece = PAWN; 
    } else {
        for (int p = PAWN; p <= KING; p++) {
            if (pieces[my_color][p] & (1ULL << to)) {
                moving_piece = p;
                pieces[my_color][p] ^= (1ULL << to); 
                break;
            }
        }
    }

    if (moving_piece == -1) {
        throw std::runtime_error("FATAL: Próba cofnięcia ruchu, ale na polu docelowym nie ma figury! (Double unmake?)");
    }
    
    pieces[my_color][moving_piece] ^= (1ULL << from);

    if (flag == MOVE_EN_PASSANT) {
        Square ep_pawn_sq = (my_color == WHITE) ? Square(to - 8) : Square(to + 8);
        pieces[opponent][PAWN] ^= (1ULL << ep_pawn_sq); 
    } 
    else if (flag == MOVE_KING_CASTLE) {
        if (my_color == WHITE) pieces[WHITE][ROOK] ^= ((1ULL << H1) | (1ULL << F1));
        else pieces[BLACK][ROOK] ^= ((1ULL << H8) | (1ULL << F8));
    } 
    else if (flag == MOVE_QUEEN_CASTLE) {
        if (my_color == WHITE) pieces[WHITE][ROOK] ^= ((1ULL << A1) | (1ULL << D1));
        else pieces[BLACK][ROOK] ^= ((1ULL << A8) | (1ULL << D8));
    }

    if (state.captured_piece != NONE && flag != MOVE_EN_PASSANT) {
        pieces[opponent][state.captured_piece] ^= (1ULL << to);
    }

    castling_rights = state.castling_rights;
    en_passant = state.en_passant;
    side_to_move = my_color;

    white_pieces = 0;
    black_pieces = 0;
    for (int p = PAWN; p <= KING; p++) {
        white_pieces |= pieces[WHITE][p];
        black_pieces |= pieces[BLACK][p];
    }
    all_pieces = white_pieces | black_pieces;
}

int Board::get_current_phase_value() const {
    int phase_points = 0;

    phase_points += std::popcount(pieces[WHITE][BISHOP] | pieces[BLACK][BISHOP]) * PHASE_WEIGHT_BISHOP;
    phase_points += std::popcount(pieces[WHITE][KNIGHT] | pieces[BLACK][KNIGHT]) * PHASE_WEIGHT_KNIGHT;
    phase_points += std::popcount(pieces[WHITE][ROOK] | pieces[BLACK][ROOK])   * PHASE_WEIGHT_ROOK;
    phase_points += std::popcount(pieces[WHITE][QUEEN] | pieces[BLACK][QUEEN])  * PHASE_WEIGHT_QUEEN;

    if (phase_points > 24) {
        phase_points = 24;
    }
    return phase_points;
}

void Board::load_fen(const std::string& fen) {
    clear();
    std::stringstream ss(fen);
    std::string position, move, castle_rights, en_pass, half_moves, full_moves;

    ss >> position >> move >> castle_rights >> en_pass >> half_moves >> full_moves;

    int rank = 7;
    int column = 0;

    for (char c : position) {
        if (c == '/') {
            rank--;   // Przechodzimy linię niżej
            column = 0; // Resetujemy kolumnę do A
        } else if (std::isdigit(c)) {
            column += (c - '0'); // Cyfra oznacza puste pola, przesuwamy się w prawo
        } 
        else {
            Color color = std::isupper(c) ? WHITE : BLACK;
            char lower_c = std::tolower(c);
            PieceType type = NONE;

            switch (lower_c)
            {
            case 'p':
                type = PAWN;
                break;
            case 'n':
                type = KNIGHT;
                break;
            case 'b':
                type = BISHOP;
                break;
            case 'r':
                type = ROOK;
                break;
            case 'q':
                type = QUEEN;
                break;
            case 'k':
                type = KING;
                break;
            default:
                break;
            }

            if(type != NONE) {
                int sq = rank * 8 + column;
                pieces[color][type] |= (1ULL << sq);
            }
            column++;
        }
    }

    side_to_move = (move == "w") ? WHITE : BLACK;

    castling_rights = 0;
    if (castle_rights != "-") {
        for (char c : castle_rights) {
            if (c == 'K') castling_rights |= 0b0001; // Białe król
            if (c == 'Q') castling_rights |= 0b0010; // Białe hetman
            if (c == 'k') castling_rights |= 0b0100; // Czarne król
            if (c == 'q') castling_rights |= 0b1000; // Czarne hetman
        }
    }

    if (en_pass != "-") {
        int ep_file = en_pass[0] - 'a';
        int ep_rank = en_pass[1] - '1';
        en_passant = Square(ep_rank * 8 + ep_file);
    } else {
        en_passant = NO_SQUARE;
    }

    white_pieces = 0;
    black_pieces = 0;
    for (int p = PAWN; p <= KING; p++) {
        white_pieces |= pieces[WHITE][p];
        black_pieces |= pieces[BLACK][p];
    }
    all_pieces = white_pieces | black_pieces;
}