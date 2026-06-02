#include "Board.h"
#include "MoveGenerator.h"
#include "Evaluate.h"

#include <iostream>


int main() {
    init_attacks();
    init_evaluation_tables();

    Board board;
    board.load_start_position();

    std::cout << "wynik pozycji: " << evaluate_position_for_active_player(board) << std::endl;

    board

    return 0;
} 