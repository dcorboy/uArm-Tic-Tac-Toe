#include <arduino.h>
#include "gameboard.h"
#include "gamelogic.h"

GameLogic::GameLogic(GameBoard* the_board) {
  board = the_board;
}

void GameLogic::test() {
  board->set_posn(4);
}

void GameLogic::new_game(bool play_first) {
  my_mark = play_first ? 1 : 2;
  their_mark = play_first ? 2 : 1;
  mode = MODE_EASY;
}

byte GameLogic::do_move() {
  if (mode == MODE_EASY) {
    return move_easy();
  } else if (mode == MODE_MEDIUM) {
    return move_medium();
  } else {
    return move_hard();
  }
}

byte GameLogic::move_easy() {
  byte posn;
  if ((board->get_turn() / 2) == 0) {  // this is the first move, let's make it a bad one
    return open_edge();
  } else if ((posn = win_possible(my_mark)) != NO_POSN) {
    return posn;
  } else if (((posn = win_possible(their_mark)) != NO_POSN) && ((rand() % 2) == 0)) { // only sees opponents win oppty half the time
    return posn;
  } else {
    return any_open();
  }
}

byte GameLogic::move_medium() {
  byte posn;
  if ((posn = win_possible(my_mark)) != NO_POSN) {
    return posn;
  } else if ((posn = win_possible(their_mark)) != NO_POSN) {
    return posn;
  } else {
    return any_open();
  }
}

byte GameLogic::move_hard() {
  byte posn;
  if ((posn = win_possible(my_mark)) != NO_POSN) {
    return posn;
  } else if ((posn = win_possible(their_mark)) != NO_POSN) {
    return posn;
  } else {
    return any_open();
  }
}

bool GameLogic::open(byte posn) {
  return (board->get_posn(posn) == 0);
}

bool GameLogic::mine(byte posn) {
  return (board->get_posn(posn) == my_mark);
}

bool GameLogic::theirs(byte posn) {
  return (board->get_posn(posn) == their_mark);
}

bool GameLogic::test(byte posn, byte player) {
  return (board->get_posn(posn) == player);
}

byte GameLogic::win_possible(byte player) {
  for (int p = 0; p < 8; p++) {
    int win_posn = win_path(player, board->paths[p]);
    if (win_posn != NO_POSN) {
      return win_posn;
    }
  }
  return NO_POSN;
}

byte GameLogic::win_path(byte player, const byte path[3]) {
  int count = 0;
  byte win_posn;

  for (int i = 0; i < 3; i++) {
    if (open(path[i])) {     // if a space is open, it may be a winner
      win_posn = path[i];
    } else if (test(path[i], player)) { // if a space is the player, it may be 1 of 2 needed
      count++;
    } else {  // otherwise it is the other guy, so a win here is not possible
      return NO_POSN;
    }
  }

  if (count == 2) { // 2 of the player piece and no opponents, makes that empty spot a winner
    return win_posn;
  } else {
    return NO_POSN;
  }
}

byte GameLogic::any_open() {
  byte move_options[9];
  int count = 0;
  for (byte i = 0; i < 9; i++) {
    if (open(i)) {
      move_options[count++] = i;
    }
  }
  return move_options[0];
}

byte GameLogic::open_edge() {
  byte move_options[9];
  int count = 0;
  if (open(1)) { move_options[count++] = 1; }
  if (open(3)) { move_options[count++] = 3; }
  if (open(5)) { move_options[count++] = 5; }
  if (open(7)) { move_options[count++] = 7; }
  return move_options[0];
}

