#include "Board.h"
#include "MoveGenerator.h"
#include "Search.h"

#include <iostream>

int main() {
    init_attacks();
    init_evaluation_tables();

    Board board;
    board.load_start_position();

    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;
    Move najlepszy_ruch = get_best_move(board, 4);
    std::cout << "Najlepszy znaleziony ruch: " << move_to_notation(board, najlepszy_ruch) << std::endl;

    

    std::string KiwiPete = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    std::string Win = "8/3b2kp/4p1p1/pr1n4/N1N4P/1P4P1/1K3P2/3R4 w - - 0 1";
    std::string Kaufman = "r1bqk2r/pp2bppp/2n1pn2/3p4/3P4/1PN1PN2/P1P2PPP/R1BQKB1R w KQkq - 0 1  ";
    std::string deep = "8/8/4k3/8/5P2/4K3/8/8 w - - 0 1";
    std::string deep2 = "8/8/4k3/8/5P2/4K3/8/8 w - - 0 1";
    std::string mate1 = "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5Q2/PPPP1PPP/RNB1KBNR b KQkq - 0 1";

    board.load_fen(KiwiPete);
    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;
    najlepszy_ruch = get_best_move(board, 4);
    std::cout << "Najlepszy znaleziony ruch: " << move_to_notation(board, najlepszy_ruch) << std::endl;
    
    board.load_fen(Win);
    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;
    board.load_fen(Kaufman);
    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;
    najlepszy_ruch = get_best_move(board, 10);
    std::cout << "Najlepszy znaleziony ruch: " << move_to_notation(board, najlepszy_ruch) << std::endl;
    board.load_fen(deep);
    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;
    najlepszy_ruch = get_best_move(board, 4);
    std::cout << "Najlepszy znaleziony ruch: " << move_to_notation(board, najlepszy_ruch) << std::endl;
    board.load_fen(mate1);
    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;
    najlepszy_ruch = get_best_move(board, 4);
    std::cout << "Najlepszy znaleziony ruch: " << move_to_notation(board, najlepszy_ruch) << std::endl;


    return 0;
} 