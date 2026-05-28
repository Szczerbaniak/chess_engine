#include "MoveGenerator.h"

inline Bitboard mask_knight_attacks(Square sq) {
    Bitboard attacks = 0ULL;
    Bitboard knight = (1ULL << sq);

    attacks |= (knight << 17) & NOT_FILE_A;
    attacks |= (knight << 15) & NOT_FILE_H;
    attacks |= (knight << 10) & NOT_FILE_AB;
    attacks |= (knight << 6) & NOT_FILE_GH;

    attacks |= (knight >> 17) & NOT_FILE_H;
    attacks |= (knight >> 15) & NOT_FILE_A;
    attacks |= (knight >> 10) & NOT_FILE_GH;
    attacks |= (knight >> 6) & NOT_FILE_AB;

    return attacks;
}

inline Bitboard mask_king_attacks(Square sq) {
    Bitboard attacks = 0ULL;
    Bitboard king = (1ULL << sq);

    attacks |= (king << 9) & NOT_FILE_A;
    attacks |= (king << 8);
    attacks |= (king << 7) & NOT_FILE_H;
    attacks |= (king << 1) & NOT_FILE_A;
    
    attacks |= (king >> 9) & NOT_FILE_H;
    attacks |= (king >> 8);
    attacks |= (king >> 7) & NOT_FILE_A;
    attacks |= (king >> 1) & NOT_FILE_H;

    return attacks;
}

inline Bitboard mask_pawn_attacks(Square sq, Color cl) {
    Bitboard attacks = 0ULL;
    Bitboard pawn = (1ULL << sq);

    if(cl == WHITE) {
        attacks |= (pawn << 9) & NOT_FILE_A;
        attacks |= (pawn << 7) & NOT_FILE_H;
    } else {
        attacks |= (pawn >> 9) & NOT_FILE_H;
        attacks |= (pawn >> 7) & NOT_FILE_A;
    }

    return attacks;
}

inline Bitboard mask_pawn_pushes(Square sq, Color cl, Bitboard occupancy) {
    Bitboard pushes = 0Ull;

    if (cl == WHITE) {
        Square single_step = Square(sq + 8);
        
        if (!(occupancy & (1ULL << single_step))) {
            pushes |= 1ULL << single_step;
            
            if (sq >= A2 && sq <= H2) {
                Square double_step = Square(sq + 16);
                if (!(occupancy & (1ULL << single_step))) {
                    pushes |= 1ULL << single_step;
                }
            }

        }
    } else {
        Square single_step = Square(sq - 8);
        
        if (!(occupancy & (1ULL << single_step))) {
            pushes |= 1ULL << single_step;
            
            if (sq >= A7 && sq <= H7) {
                Square double_step = Square(sq - 16);
                if (!(occupancy & (1ULL << single_step))) {
                    pushes |= 1ULL << single_step;
                }
            }

        }
    }


    return pushes;
}

inline Bitboard mask_bishop_attacks(Square sq, Bitboard occupancy) {
    Bitboard attacks = 0ULL;

    int start_r = sq / 8;
    int start_c = sq % 8;

    //lewy gorny skos
    for (int r = start_r + 1, c = start_c + 1; r < 8 && c < 8; r++, c++) {
        Bitboard attack = (1ULL << r * 8 + c);

        attacks |= attack;

        if (occupancy & attack) {
            break;
        }
    }

    //prawy gorny skos
    for (int r = start_r + 1, c = start_c - 1; r < 8 && c >= 0; r++, c--) {
        Bitboard attack = (1ULL << r * 8 + c);

        attacks |= attack;

        if (occupancy & attack) {
            break;
        }
    }

    // #prawy dolny skos
    for (int r = start_r - 1, c = start_c + 1; r >= 0 && c < 8; r--, c++) {
        Bitboard attack = (1ULL << r * 8 + c);

        attacks |= attack;

        if (occupancy & attack) {
            break;
        }
    }

    // lewy dolny skos
    for (int r = start_r - 1, c = start_c - 1; r >= 0 && c >= 0; r--, c--) {
        Bitboard attack = (1ULL << (r * 8 + c));

        attacks |= attack;

        if (occupancy & attack) {
            break;
        }
    }

    return attacks;
}

inline Bitboard mask_rook_attacks(Square sq, Bitboard occupancy) {
    Bitboard attacks = 0ULL;

    int start_r = sq / 8;
    int start_c = sq % 8;

    //up

    for (int r = start_r + 1; r < 8; r++) {
        Bitboard attack = 1ULL << (r * 8 + start_c);
        attacks |= attack;

        if (occupancy & attack)
        {
            break;
        }
        
    }

    //down
    for (int r = start_r - 1; r >= 0; r--) {
        Bitboard attack = 1ULL << (r * 8 + start_c);
        attacks |= attack;

        if (occupancy & attack)
        {
            break;
        }
        
    }
    
    //right
    for (int c = start_c + 1; c < 8; c++) {
        Bitboard attack = 1ULL << (start_r * 8 + c);
        attacks |= attack;

        if (occupancy & attack)
        {
            break;
        }
        
    }

    //left
    for (int c = start_c - 1; c >= 0; c--) {
        Bitboard attack = 1ULL << (start_r * 8 + c);
        attacks |= attack;

        if (occupancy & attack)
        {
            break;
        }
        
    }

    return attacks;
}


inline Bitboard mask_queen_attacks(Square sq, Bitboard occupancy) {
    Bitboard attacks = 0ULL;

    attacks |= mask_bishop_attacks(sq, occupancy);
    attacks |= mask_rook_attacks(sq, occupancy);

    return attacks;
}


inline void init_attacks() {
    for (int i = 0; i < 64; i++) {
        knight_attacks[i] = mask_knight_attacks(Square(i));
        king_attacks[i] = mask_king_attacks(Square(i));
        pawn_attacks[WHITE][i] = mask_pawn_attacks(Square(i), WHITE);
        pawn_attacks[BLACK][i] = mask_pawn_attacks(Square(i), BLACK);
    }
}