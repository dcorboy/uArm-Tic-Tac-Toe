#include <arduino.h>
#include "gameboard.h"

GameBoard::GameBoard() {
  reset();
}

void GameBoard::reset() {
  turn = 0;
  for (int i = 0; i < 9; i++) {
    board[i] = 0;
  }
}

void GameBoard::set_posn(byte pos) {
  board[pos] = turn % 2 == 0 ? 1 : 2;
  turn++;
}

byte GameBoard::get_posn(byte pos) {
  return board[pos];
}

byte GameBoard::get_turn() {
  return turn;
}

bool GameBoard::valid_move(byte pos) {
  return (pos >= 0 && pos < 9 && board[pos] == 0);
}

bool GameBoard::full() {
  for (int i = 0; i < 9; i++) {
    if (board[i] == 0) {
      return false;
    }
  }
  return true;
}

bool GameBoard::game_over() {
  return (full() || winner() != 0);
}

byte GameBoard::winner() {
  for (int p = 0; p < 8; p++) {
    int winner = board[paths[p][0]];
    if (winner != 0 && board[(int)paths[p][1]] == winner && board[(int)paths[p][2]] == winner) {
      return winner;
    }
  }
  return 0;
}
