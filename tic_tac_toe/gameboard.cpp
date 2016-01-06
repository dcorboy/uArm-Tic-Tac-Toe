#include "gameboard.h"


GameBoard::GameBoard() {
  reset();
}

void GameBoard::reset() {
  turn = 0;
  for (byte i = 0; i < 9; i++) {
    board[i] = 0;
  }
}

byte *GameBoard::get_board() {
  return board;
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
  for (byte i = 0; i < 9; i++) {
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
    byte path[3];
  for (byte p = 0; p < 8; p++) {
    get_path(p, path);
    byte winner = board[path[0]];
    if (winner != 0 && board[path[1]] == winner && board[path[2]] == winner) {
      return winner;
    }
  }
  return 0;
}

// Don't hate me, I tried and tried to make a static const array in PROGMEM in every way possible. I failed. I want the memory.
//const byte GameBoard::paths[8][3] PROGMEM = {
//  {0, 1, 2},
//  {3, 4, 5},
//  {6, 7, 8},
//  {0, 3, 6},
//  {1, 4, 7},
//  {2, 5, 8},
//  {0, 4, 8},
//  {2, 4, 6}
//};

// So this function MODIFIES the data on your stack passed as the array pointer. 3 bytes of it. I said don't hate me.
void GameBoard::get_path(byte path, byte path_ary[]) {
  switch(path) {
    case 0 : path_ary[0] = 0; path_ary[1] = 1, path_ary[2] = 2; break;
    case 1 : path_ary[0] = 3; path_ary[1] = 4, path_ary[2] = 5; break;
    case 2 : path_ary[0] = 6; path_ary[1] = 7, path_ary[2] = 8; break;
    case 3 : path_ary[0] = 0; path_ary[1] = 3, path_ary[2] = 6; break;
    case 4 : path_ary[0] = 1; path_ary[1] = 4, path_ary[2] = 7; break;
    case 5 : path_ary[0] = 2; path_ary[1] = 5, path_ary[2] = 8; break;
    case 6 : path_ary[0] = 0; path_ary[1] = 4, path_ary[2] = 8; break;
    case 7 : path_ary[0] = 2; path_ary[1] = 4, path_ary[2] = 6; break;
  }
}
