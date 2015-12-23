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
  mode = MODE_HARD;

  if (mode == MODE_EASY) {
    logic_node = &GameLogic::move_easy;
  } else if (mode == MODE_MEDIUM) {
    logic_node = &GameLogic::move_medium;
  } else if (play_first) {
    logic_node = &GameLogic::first_x_hard;
  } else {
    logic_node = &GameLogic::first_o_hard;
  }
}

byte GameLogic::do_move() {
  (this->*logic_node)();
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

byte GameLogic::first_x_hard() {
  logic_node = &GameLogic::second_x_hard;
  return open_corner();
}

byte GameLogic::second_x_hard() {
  logic_node = &GameLogic::play_out_hard;
  byte trap;
  if (open(4)) {
    if ((trap = second_x_trap()) != NO_POSN) {  // a more fun way to win!
      return trap;
    } else {
      return unblocked_corner();  // there should always be one at this point
    }
  } else {
    return opposite_corner();  // there should always be one at this point
  }
}

byte GameLogic::first_o_hard() {
  if (theirs(4)) {
    logic_node = &GameLogic::o_center_hard;
    return open_corner();
  } else if (theirs(0) || theirs(2) || theirs(6) || theirs(8)) {
    logic_node = &GameLogic::o_corner_hard;
    return 4;
  } else {  // edge move, worst move for player
    logic_node = &GameLogic::o_edge_hard;
    return 4;
  }
}

byte GameLogic::o_center_hard() {
  logic_node = &GameLogic::play_out_hard;
  byte block;
  if ((block = win_possible(their_mark)) != NO_POSN) {
    return block;
  } else { return unblocked_corner(); }
}

byte GameLogic::o_corner_hard() {
  logic_node = &GameLogic::play_out_hard;
  byte block;
  if ((block = win_possible(their_mark)) != NO_POSN) {
    return block;
  } else { return open_edge(); }
}

byte GameLogic::o_edge_hard() {
  logic_node = &GameLogic::play_out_hard;
  byte block;
  if ((block = win_possible(their_mark)) != NO_POSN) {
    return block;
  } else if ((block = blocking_corner()) != NO_POSN) {
    return block;
  } else {
    return open_corner();
  }
}

byte GameLogic::play_out_hard() {
  byte posn;
  if ((posn = win_possible(my_mark)) != NO_POSN) {
    return posn;
  } else if ((posn = win_possible(their_mark)) != NO_POSN) {
    return posn;
  } else if ((posn = unblocked_corner()) != NO_POSN) {
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
  return move_options[rand() % count];
}

byte GameLogic::open_edge() {
  byte move_options[9];
  int count = 0;
  if (open(1)) { move_options[count++] = 1; }
  if (open(3)) { move_options[count++] = 3; }
  if (open(5)) { move_options[count++] = 5; }
  if (open(7)) { move_options[count++] = 7; }
  if (count) {
    return move_options[rand() % count];
  } else { return NO_POSN; }
}

byte GameLogic::open_corner() {
  byte move_options[4];
  int count = 0;
  if (open(0)) { move_options[count++] = 0; }
  if (open(2)) { move_options[count++] = 2; }
  if (open(6)) { move_options[count++] = 6; }
  if (open(8)) { move_options[count++] = 8; }
  if (count) {
    return move_options[(rand() % count)];
  } else { return NO_POSN; }
}

byte GameLogic::unblocked_corner() {
  byte move_options[4];
  int count = 0;
  if (open(0) && ((mine(2) && open(1)) || (mine(6) && open(3)))) { move_options[count++] = 0; }
  if (open(2) && ((mine(0) && open(1)) || (mine(8) && open(5)))) { move_options[count++] = 2; }
  if (open(6) && ((mine(0) && open(3)) || (mine(8) && open(7)))) { move_options[count++] = 6; }
  if (open(8) && ((mine(2) && open(5)) || (mine(6) && open(7)))) { move_options[count++] = 8; }
  if (count) {
    return move_options[rand() % count];
  } else { return NO_POSN; }
}

byte GameLogic::opposite_corner() {
  if (open(0) && mine(8)) { return 0; }
  else if (open(2) && mine(6)) { return 2; }
  else if (open(6) && mine(2)) { return 6; }
  else if (open(8) && mine(0)) { return 8; }
  else { return NO_POSN; }
}

byte GameLogic::blocking_corner() {
  byte move_options[4];
  int count = 0;
  if (open(0) && theirs(1) && theirs(3)) { move_options[count++] = 0; }
  if (open(2) && theirs(1) && theirs(5)) { move_options[count++] = 2; }
  if (open(6) && theirs(3) && theirs(7)) { move_options[count++] = 6; }
  if (open(8) && theirs(5) && theirs(7)) { move_options[count++] = 8; }
  if (count) {
    return move_options[rand() % count];
  } else { return NO_POSN; }
}

byte GameLogic::second_x_trap() {
  if ((mine(6) && theirs(5)) || (mine(2) && theirs(7))) { return 0; }
  else if ((mine(8) && theirs(3)) || (mine(0) && theirs(7))) { return 2; }
  else if ((mine(8) && theirs(1)) || (mine(0) && theirs(5))) { return 6; }
  else if ((mine(2) && theirs(3)) || (mine(6) && theirs(1))) { return 8; }
  else { return NO_POSN; }
}

