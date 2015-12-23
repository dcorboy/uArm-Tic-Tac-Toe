#include "sensor.h"

Sensor::Sensor() {
  reset();
}

void Sensor::reset() {
  turn = 0;
  for (int i = 0; i < 9; i++) {
    board[i] = 0;
  }
}

bool Sensor::board_ready() {
  return false;
}

byte Sensor::detect_start() {
  int object = pixy.getBlocks();
  return NO_VAL;
}

byte Sensor::detect_player_move() {
  // if there is a single piece on the board, that counts as a player move
  return NO_VAL;
}

