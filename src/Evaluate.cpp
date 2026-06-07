#include "Evaluate.h"
#include "EvaluationValues.h"
#include "PestoTables.h" 

void init_evaluation_tables() {
    const int* pesto_mg[6] = { mg_pawn_table, mg_knight_table, mg_bishop_table, mg_rook_table, mg_queen_table, mg_king_table };
    const int* pesto_eg[6] = { eg_pawn_table, eg_knight_table, eg_bishop_table, eg_rook_table, eg_queen_table, eg_king_table };
 
    for (int piece = PAWN; piece <= KING; piece++) {
        for (int sq = 0; sq < 64; ++sq) {
            int kaufman_mg = 0, kaufman_tr = 0, kaufman_eg = 0;

            if (piece == PAWN) {
                int column = sq % 8;
                kaufman_mg = KAUFMAN_PAWN_VALUES_BY_FILE[column].mg;
                kaufman_tr = KAUFMAN_PAWN_VALUES_BY_FILE[column].tr;
                kaufman_eg = KAUFMAN_PAWN_VALUES_BY_FILE[column].eg;
            } else {
                kaufman_mg = KAUFMAN_PIECE_VALUES[piece].mg;
                kaufman_tr = KAUFMAN_PIECE_VALUES[piece].tr;
                kaufman_eg = KAUFMAN_PIECE_VALUES[piece].eg;
            }

            int pesto_offset_mg = pesto_mg[piece][sq];
            int pesto_offset_tr = (pesto_mg[piece][sq] + pesto_eg[piece][sq]) / 2;
            int pesto_offset_eg = pesto_eg[piece][sq];

            PST_TABLES[piece][sq].mg = kaufman_mg + pesto_offset_mg;
            PST_TABLES[piece][sq].tr = kaufman_tr + pesto_offset_tr;
            PST_TABLES[piece][sq].eg = kaufman_eg + pesto_offset_eg;
        }
    }
}

int evaluate_position_for_active_player(const Board& board) {
    Score white_score;
    Score black_score;
    Color playing_side = board.side_to_move;
    for (int piece = PAWN; piece <= KING; piece++)
    {
        uint64_t w_pieces = board.pieces[WHITE][piece]; // Jeśli pieces jest prywatne, użyj swojego gettera
        while (w_pieces) {
            int sq = std::countr_zero(w_pieces);
            white_score.mg += PST_TABLES[piece][sq].mg;
            white_score.tr += PST_TABLES[piece][sq].tr;
            white_score.eg += PST_TABLES[piece][sq].eg;

            w_pieces &= (w_pieces - 1); 
        }

        uint64_t b_pieces = board.pieces[BLACK][piece];
        while (b_pieces) {
            int sq = std::countr_zero(b_pieces);

            // Odbicie lustrzane dla czarnych, żeby czytały PST od swojej strony szachownicy
            int flipped_sq = sq ^ 56; 
            black_score.mg += PST_TABLES[piece][flipped_sq].mg;
            black_score.tr += PST_TABLES[piece][flipped_sq].tr;
            black_score.eg += PST_TABLES[piece][flipped_sq].eg;

            b_pieces &= (b_pieces - 1);
        }
    }

    //dla bialych
    int w_num_of_bishops = std::popcount(board.pieces[WHITE][BISHOP]);
    int w_num_of_knights = std::popcount(board.pieces[WHITE][KNIGHT]);
    int w_num_of_rooks   = std::popcount(board.pieces[WHITE][ROOK]);
    int w_num_of_queens  = std::popcount(board.pieces[WHITE][QUEEN]);

    if (w_num_of_bishops == 2) {
        white_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[BISHOP_PAIR].mg - KAUFMAN_PIECE_VALUES[BISHOP].mg);
        white_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[BISHOP_PAIR].tr - KAUFMAN_PIECE_VALUES[BISHOP].tr);
        white_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[BISHOP_PAIR].eg - KAUFMAN_PIECE_VALUES[BISHOP].eg);
    }

    if (w_num_of_rooks >= 2) {
        white_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_ROOK].mg - KAUFMAN_PIECE_VALUES[ROOK].mg);
        white_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_ROOK].tr - KAUFMAN_PIECE_VALUES[ROOK].tr);
        white_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_ROOK].eg - KAUFMAN_PIECE_VALUES[ROOK].eg);
    }
    if (w_num_of_queens >= 2) {
        white_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_QUEEN].mg - KAUFMAN_PIECE_VALUES[QUEEN].mg);
        white_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_QUEEN].tr - KAUFMAN_PIECE_VALUES[QUEEN].tr);
        white_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_QUEEN].eg - KAUFMAN_PIECE_VALUES[QUEEN].eg);
    }
    if (w_num_of_knights >= 2) {
        white_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[KNIGHT_PAIR].mg - KAUFMAN_PIECE_VALUES[KNIGHT].mg);
        white_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[KNIGHT_PAIR].tr - KAUFMAN_PIECE_VALUES[KNIGHT].tr);
        white_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[KNIGHT_PAIR].eg - KAUFMAN_PIECE_VALUES[KNIGHT].eg);
    }

    //dla czarnych
    int b_num_of_bishops = std::popcount(board.pieces[BLACK][BISHOP]);
    int b_num_of_knights = std::popcount(board.pieces[BLACK][KNIGHT]);
    int b_num_of_rooks   = std::popcount(board.pieces[BLACK][ROOK]);
    int b_num_of_queens  = std::popcount(board.pieces[BLACK][QUEEN]);

    if (b_num_of_bishops == 2) {
        black_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[BISHOP_PAIR].mg - KAUFMAN_PIECE_VALUES[BISHOP].mg);
        black_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[BISHOP_PAIR].tr - KAUFMAN_PIECE_VALUES[BISHOP].tr);
        black_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[BISHOP_PAIR].eg - KAUFMAN_PIECE_VALUES[BISHOP].eg);
    }

    if (b_num_of_rooks >= 2) {
        black_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_ROOK].mg - KAUFMAN_PIECE_VALUES[ROOK].mg);
        black_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_ROOK].tr - KAUFMAN_PIECE_VALUES[ROOK].tr);
        black_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_ROOK].eg - KAUFMAN_PIECE_VALUES[ROOK].eg);
    }
    if (b_num_of_queens >= 2) {
        black_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_QUEEN].mg - KAUFMAN_PIECE_VALUES[QUEEN].mg);
        black_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_QUEEN].tr - KAUFMAN_PIECE_VALUES[QUEEN].tr);
        black_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[SECOND_QUEEN].eg - KAUFMAN_PIECE_VALUES[QUEEN].eg);
    }
    if (b_num_of_knights >= 2) {
        black_score.mg += (KAUFMAN_PIECE_VALUES_BONUSES[KNIGHT_PAIR].mg - KAUFMAN_PIECE_VALUES[KNIGHT].mg);
        black_score.tr += (KAUFMAN_PIECE_VALUES_BONUSES[KNIGHT_PAIR].tr - KAUFMAN_PIECE_VALUES[KNIGHT].tr);
        black_score.eg += (KAUFMAN_PIECE_VALUES_BONUSES[KNIGHT_PAIR].eg - KAUFMAN_PIECE_VALUES[KNIGHT].eg);
    }

    if (board.side_to_move == WHITE) {
        white_score.mg += 20; white_score.tr += 20; white_score.eg += 20;
    } else {
        black_score.mg += 20; black_score.tr += 20; black_score.eg += 20;
    }

    Score final_score;
    final_score.mg = white_score.mg - black_score.mg;
    final_score.tr = white_score.tr - black_score.tr;
    final_score.eg = white_score.eg - black_score.eg;

    int current_phase = board.get_current_phase_value();
    int evaluation = final_score.interpolate(current_phase);

    return (board.side_to_move == WHITE) ? evaluation : -evaluation;
}