//
// Created by Theophilus Homawoo on 08/11/2023.
//

#include "GomukuBoard.hpp"

void GomukuBoard::set(int x, int y, bool isPlayer) {
    int index = x + y * BOARD_SIZE;
    if (isPlayer) {
        player.set(index);
    } else {
        opponent.set(index);
    }
}

bool GomukuBoard::isOccupied(int x, int y) const {
    int index = x + y * BOARD_SIZE;
    return player.test(index) || opponent.test(index);
}
