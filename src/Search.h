#pragma once

#include "Board.h"
#include "MoveGenerator.h"
#include "Evaluate.h"
#include <string>

#include <iostream>

const int INF = 1000000;

inline char piece_to_notation_char(PieceType piece) {
    switch (piece) {
        case PAWN:   return 'p';
        case KNIGHT: return 'n';
        case BISHOP: return 'b';
        case ROOK:   return 'r';
        case QUEEN:  return 'q';
        case KING:   return 'k';
        default:     return '?';
    }
}

inline std::string square_to_notation(Square square) {
    if (square == NO_SQUARE) {
        return "--";
    }

    std::string notation;
    notation += static_cast<char>('a' + (square % 8));
    notation += static_cast<char>('1' + (square / 8));
    return notation;
}

inline PieceType piece_on_square(const Board& board, Square square) {
    Bitboard mask = 1ULL << square;

    for (int color = WHITE; color <= BLACK; ++color) {
        for (int piece = PAWN; piece <= KING; ++piece) {
            if (board.pieces[color][piece] & mask) {
                return static_cast<PieceType>(piece);
            }
        }
    }

    return NONE;
}

inline std::string move_to_notation(const Board& board, Move move) {
    PieceType moving_piece = piece_on_square(board, move.from());
    std::string notation;
    notation += piece_to_notation_char(moving_piece);
    notation += square_to_notation(move.from());
    notation += square_to_notation(move.to());
    return notation;
}

int alphaBetaMin(Board &board, int alpha, int beta, int depthleft);
int alphaBetaMax(Board &board, int alpha, int beta, int depthleft);

Move get_best_move(Board &board, int depth) {
    MoveList move_list;
    board.generate_pseudo_legal_moves(move_list);

    Move best_move;
    int best_score = -INF;
    int alpha = -INF;
    int beta = INF;

    for (Move move : move_list) {
        UndoState state;
        if (board.make_move_on_board(move, state)) {
            
            int score = alphaBetaMin(board, alpha, beta, depth - 1);
            
            board.unmake_move(move, state);

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
            
            if (score > alpha) {
                alpha = score;
            }
        }
    }
    return best_move;
}

int alphaBetaMax(Board &board, int alpha, int beta, int depthleft) {
    if (depthleft == 0) return evaluate_position_for_active_player(board);

    MoveList move_list;
    board.generate_pseudo_legal_moves(move_list);
    int score;

    for (Move move : move_list) {
        UndoState state;
        if (board.make_move_on_board(move, state)) {
            score = alphaBetaMin(board, alpha, beta, depthleft - 1);
            board.unmake_move(move, state);

            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
    }
    return alpha;
}

int alphaBetaMin(Board& board, int alpha, int beta, int depthleft) {
    if (depthleft == 0) return evaluate_position_for_active_player(board);

    MoveList move_list;
    board.generate_pseudo_legal_moves(move_list);
    int score;

    for (Move move : move_list) {
        UndoState state;
        if (board.make_move_on_board(move, state)) {
            score = alphaBetaMax(board, alpha, beta, depthleft - 1);
            board.unmake_move(move, state);

            if (score <= alpha) {
                return alpha;
            }
            if (score < beta) {
                beta = score;
            }
        }
    }
    return beta;
} 