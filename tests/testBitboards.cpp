
#include <boost/test/unit_test.hpp>

#include "Types.h"
#include "MoveGenerator.h"
#include "Board.h"

BOOST_AUTO_TEST_CASE(BitboardBasicOps)
{
	Bitboard bb = 0ULL;

	set_bit(bb, A1);
	BOOST_TEST(get_bit(bb, A1));

	set_bit(bb, H8);
	BOOST_TEST(get_bit(bb, H8));

	pop_bit(bb, A1);
	BOOST_TEST(!get_bit(bb, A1));
}

BOOST_AUTO_TEST_CASE(MoveEncoding)
{
	Move m = make_move(E2, E4, MOVE_PAWN_DOUBLE);
	BOOST_TEST(m.from() == E2);
	BOOST_TEST(m.to() == E4);
	BOOST_TEST(m.flag() == MOVE_PAWN_DOUBLE);

	Move promo = make_move(A7, A8, MOVE_PROMO_QUEEN);
	BOOST_TEST(promo.from() == A7);
	BOOST_TEST(promo.to() == A8);
	BOOST_TEST(promo.flag() == MOVE_PROMO_QUEEN);
}

BOOST_AUTO_TEST_CASE(MaskPawnPushesAndAttacks)
{
	// empty occupancy: white pawn on A2 should be able to push to A3 and A4
	Bitboard pushes = mask_pawn_pushes(A2, WHITE, 0ULL);
	BOOST_TEST((pushes & (1ULL << A3)) != 0ULL);
	BOOST_TEST((pushes & (1ULL << A4)) != 0ULL);

	// pawn attacks from B2 should hit A3 and C3 for white
	Bitboard attacks = mask_pawn_attacks(B2, WHITE);
	BOOST_TEST((attacks & (1ULL << A3)) != 0ULL);
	BOOST_TEST((attacks & (1ULL << C3)) != 0ULL);
}

BOOST_AUTO_TEST_CASE(KnightKingRookBishopMasks)
{
	Bitboard katt = mask_knight_attacks(B1);
	BOOST_TEST((katt & (1ULL << A3)) != 0ULL);
	BOOST_TEST((katt & (1ULL << C3)) != 0ULL);

	Bitboard ratt = mask_rook_attacks(A1, 0ULL);
	BOOST_TEST((ratt & (1ULL << A2)) != 0ULL);

	Bitboard batt = mask_bishop_attacks(C1, 0ULL);
	BOOST_TEST((batt & (1ULL << D2)) != 0ULL);

	Bitboard qatt = mask_queen_attacks(D4, 0ULL);
	BOOST_TEST(qatt != 0ULL);
}

BOOST_AUTO_TEST_CASE(MoveListPush)
{
	MoveList ml;
	ml.count = 0;
	ml.push_back(make_move(A1, A2, MOVE_QUIET));
	BOOST_TEST(ml.count == 1);
	BOOST_TEST(ml.begin()->from() == A1);
}

