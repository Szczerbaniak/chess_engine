#include <chrono>
#include <cstdint>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "Board.h"
#include "MoveGenerator.h"

namespace {

PieceType piece_from_char(char piece_char) {
	switch (piece_char) {
		case 'p': return PAWN;
		case 'n': return KNIGHT;
		case 'b': return BISHOP;
		case 'r': return ROOK;
		case 'q': return QUEEN;
		case 'k': return KING;
		default: return NONE;
	}
}

Square square_from_fen(const std::string& square_text) {
	if (square_text.size() != 2) {
		return NO_SQUARE;
	}

	char file = square_text[0];
	char rank = square_text[1];
	if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
		return NO_SQUARE;
	}

	return Square((rank - '1') * 8 + (file - 'a'));
}

void load_fen(Board& board, const std::string& fen) {
	board.clear();

	std::size_t part_begin = 0;
	auto next_part = [&fen, &part_begin]() -> std::string {
		std::size_t part_end = fen.find(' ', part_begin);
		std::string part = fen.substr(part_begin, part_end - part_begin);
		part_begin = (part_end == std::string::npos) ? std::string::npos : part_end + 1;
		return part;
	};

	const std::string board_part = next_part();
	const std::string side_part = next_part();
	const std::string castling_part = next_part();
	const std::string en_passant_part = next_part();
	const std::string halfmove_part = next_part();
	const std::string fullmove_part = next_part();

	if (board_part.empty() || side_part.empty() || castling_part.empty() || en_passant_part.empty() || halfmove_part.empty() || fullmove_part.empty()) {
		throw std::invalid_argument("Invalid FEN");
	}

	int square_index = 56;
	for (char symbol : board_part) {
		if (symbol == '/') {
			square_index -= 16;
			continue;
		}

		if (symbol >= '1' && symbol <= '8') {
			square_index += symbol - '0';
			continue;
		}

		Color color = (symbol >= 'A' && symbol <= 'Z') ? WHITE : BLACK;
		PieceType piece = piece_from_char(static_cast<char>(std::tolower(static_cast<unsigned char>(symbol))));
		if (piece == NONE || square_index < 0 || square_index > 63) {
			throw std::invalid_argument("Invalid FEN board placement");
		}

		board.pieces[color][piece] |= (1ULL << square_index);
		++square_index;
	}

	board.side_to_move = (side_part == "w") ? WHITE : BLACK;

	board.castling_rights = 0;
	if (castling_part.find('K') != std::string::npos) board.castling_rights |= 0b0001;
	if (castling_part.find('Q') != std::string::npos) board.castling_rights |= 0b0010;
	if (castling_part.find('k') != std::string::npos) board.castling_rights |= 0b0100;
	if (castling_part.find('q') != std::string::npos) board.castling_rights |= 0b1000;

	board.en_passant = (en_passant_part == "-") ? NO_SQUARE : square_from_fen(en_passant_part);
	if (board.en_passant == NO_SQUARE && en_passant_part != "-") {
		throw std::invalid_argument("Invalid FEN en passant square");
	}

	for (int type = PAWN; type <= KING; ++type) {
		board.white_pieces |= board.pieces[WHITE][type];
		board.black_pieces |= board.pieces[BLACK][type];
	}

	board.all_pieces = board.white_pieces | board.black_pieces;
}



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

struct PerftCase {
	int depth;
	uint64_t nodes;
	const char* fen;
};

BOOST_AUTO_TEST_CASE(PerftInfo)
{
	init_attacks();

	const std::vector<PerftCase> cases = {
		{5, 4865609, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
		{1, 8, "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2"},
		{1, 8, "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3"},
		{1, 19, "r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2"},
		{1, 5, "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2"},
		{1, 44, "2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2"},
		{1, 39, "rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9"},
		{1, 9, "2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4"},
		{3, 62379, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"},
		{3, 89890, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"},
		{6, 1134888, "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1"},
		{6, 1015133, "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1"},
		{6, 1440467, "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1"},
		{6, 661072, "5k2/8/8/8/8/8/8/4K2R w K - 0 1"},
		{6, 803711, "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1"},
		{4, 1274206, "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1"},
		{4, 1720476, "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1"},
		{6, 3821001, "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1"},
		{5, 1004658, "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1"},
		{6, 217342, "4k3/1P6/8/8/8/8/K7/8 w - - 0 1"},
		{6, 92683, "8/P1k5/K7/8/8/8/8/8 w - - 0 1"},
		{6, 2217, "K1k5/8/P7/8/8/8/8/8 w - - 0 1"},
		{7, 567584, "8/k1P5/8/1K6/8/8/8/8 w - - 0 1"},
		{4, 23527, "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1"},
	};

	for (const auto& test_case : cases) {
		Board board;
		load_fen(board, test_case.fen);

		const auto start_time = std::chrono::high_resolution_clock::now();
		const uint64_t total_nodes = perft(test_case.depth, board);
		const auto end_time = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<double> elapsed = end_time - start_time;
		const uint64_t nps = elapsed.count() > 0.0
			? static_cast<uint64_t>(total_nodes / elapsed.count())
			: 0ULL;

		std::cout << "Laczna liczba wezlow: " << total_nodes << std::endl;
		std::cout << "Czas wykonania: " << elapsed.count() << " s" << std::endl;
		std::cout << "Wydajnosc (NPS): " << nps << " wezlow/s" << std::endl;

		BOOST_TEST_CONTEXT("FEN: " << test_case.fen << ", depth: " << test_case.depth) {
			BOOST_TEST(total_nodes == test_case.nodes);
		}
	}
}
