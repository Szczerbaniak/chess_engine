#include <chrono>
#include <cstdint>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "Board.h"
#include "MoveGenerator.h"

namespace {



uint64_t perft(int depth, Board& board) {
	if (depth == 0) {
		return 1ULL;
	}

	uint64_t nodes = 0;
	MoveList move_list;
	board.generate_pseudo_legal_moves(move_list);

	for (Move move : move_list) {
		UndoState state;
		if (board.make_move_on_board(move, state)) {
			nodes += perft(depth - 1, board);
			board.unmake_move(move, state);
		}
	}

	return nodes;
}

} // namespace


// 3. Ustawienie głębokości testu
// Oczekiwane wyniki dla pozycji startowej:
// Glebokosc 1: 20
// Glebokosc 2: 400
// Glebokosc 3: 8 902
// Glebokosc 4: 197 281
// Glebokosc 5: 4 865 609


BOOST_AUTO_TEST_CASE(PerftInfo)
{
	init_attacks();

	Board board;
	board.load_start_position();

	const int depth = 5;

    const auto start_time = std::chrono::high_resolution_clock::now();
	const uint64_t total_nodes = perft(depth, board);
	const auto end_time = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double> elapsed = end_time - start_time;

	std::cout << "Laczna liczba wezlow: " << total_nodes << std::endl;
	std::cout << "Czas wykonania: " << elapsed.count() << " s" << std::endl;

	const uint64_t nps = elapsed.count() > 0.0
		? static_cast<uint64_t>(total_nodes / elapsed.count())
		: 0ULL;
	std::cout << "Wydajnosc (NPS): " << nps << " wezlow/s" << std::endl;


	BOOST_TEST(total_nodes == 4865609);
}
