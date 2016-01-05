#include "sensor.h"

#ifndef NO_PIXY
Pixy pixy;
#endif

void print_board(byte board[]);

Sensor::Sensor(GameBoard* the_board) {
  game_board = the_board;
  reset();
}

void Sensor::begin() {
#ifndef NO_PIXY
  pixy.init();
#endif
}

void Sensor::reset() {
  for (int i = 0; i < 9; i++) {
    cached_board[i] = 0;
  }
}

bool Sensor::board_ready() {
  byte board[9];
  return (valid_board(board) && board_count(board) == 0);
}

// return true if the player is going first
byte Sensor::detect_start() {
  byte board[9];
  if (valid_board(board) && board_count(board) == 1) {
    for (int x = 0; x < 9; x++) {
      if (board[x] != 0) {
        player_mark = board[x] == 1 ? 2 : 1;
        return player_mark == 1;
      }
    }
  }
  return NO_VAL;
}

byte Sensor::detect_player_move() {
  // if there is a single piece on the board, and that piece is the player's color, that counts as a player move{
  byte board[9];
  if (valid_board(board)) {
    if (board_count(board) == game_board->get_turn() + 1) {
      byte move_pos = diff_game_board(board);
      Serial.println(F("Someone made a move?"));
      if (move_pos != NO_VAL && board[move_pos] == player_mark) {
        return move_pos;
      }
    } else {
      Serial.print(board_count(board));
      Serial.print(" vs. ");
      Serial.println(game_board->get_turn() + 1);
    }
  }
  return NO_VAL;
}

#ifdef NO_PIXY

byte *Sensor::valid_board(byte board[]) {
  return NULL;
}
byte *Sensor::check_board(byte board[]) {
  return NULL;
}

#else

byte *Sensor::valid_board(byte board[]) {
  if (check_board(board)) {
    if (boards_equal(board, cached_board)) {
      //Serial.println(stable_hold);
      if (++stable_hold >= STABLE_HOLD) {
        stable_hold = 0;
        print_board(board);
        return board;
      }
    } else {
      memcpy(cached_board, board, 9);
      stable_hold = 0;
    }
  }
  return NULL;
}

// returns a board after an appropriate amount of time, otherwise NULL
byte *Sensor::check_board(byte board[]) {
  uint16_t object_cnt = pixy.getBlocks();
  //Serial.println(object_cnt);
  if (object_cnt > 0) {
    frame_hold++;
    //blank_hold = 0;
    if (frame_hold % 64 == 0) {
      decode_board(board, object_cnt);
      return board;
    }
    //  } else {
    //    //delay(20);
    //    if (blank_hold++ == BLANK_HOLD) {
    //      decode_board(board, object_cnt);
    //      blank_hold = 0;
    //      return board;
    //    } else {Serial.println(blank_hold);}
  }
  return NULL;
}

// always returns whatever board it sees, overwriting board parameter completely
// will probably be incorrect if some Pixy functions are called too quickly
byte *Sensor::decode_board(byte board[], uint16_t object_cnt) {
  char buf[32];
  byte pos;
  int top = 4;
  int left = 80;
  int bottom = 146;
  int right = 239;

  for (int x = 0; x < 9; x++) {
    board[x] = 0;
  }

  if (object_cnt) {
    for (int j = 0; j < object_cnt; j++) {
      int xpos = pixy.blocks[j].x;
      int ypos = pixy.blocks[j].y;
      if (xpos > left && xpos < right && ypos > top && ypos < bottom) {
        int width = right - left;
        int height = bottom - top;
        int vdiv_1 = left + (width / 3);
        int vdiv_2 = vdiv_1 + (width / 3);
        int hdiv_1 = top + (height / 3);
        int hdiv_2 = hdiv_1 + (height / 3);

        if (ypos < hdiv_1) {
          pos = 0;
        } else if (ypos < hdiv_2) {
          pos = 3;
        } else {
          pos = 6;
        }

        if (xpos > vdiv_2) {
          pos += 2;
        } else if (xpos > vdiv_1) {
          pos += 1;
        }
        board[pos] = (byte)pixy.blocks[j].signature;
        // sprintf(buf, "  %c mark in position %d: ", pixy.blocks[j].signature == 1 ? 'X' : 'O', pos);
        // Serial.print(buf);
        // pixy.blocks[j].print();
      } else {
        // Serial.print("Outside board: ");
        // pixy.blocks[j].print();
      }
    }
  } else {
    Serial.println(F("No objects"));
  }
  return board;
}

#endif

bool Sensor::boards_equal(byte board_a[], byte board_b[]) {
  Serial.print(board_count(board_a));
  Serial.print(F(" vs. "));
  Serial.println(board_count(board_b));
  for (int i = 0; i < 9; i++) {
    if (board_a[i] != board_b[i]) {
      return false;
    }
  }
  return true;
}

byte Sensor::board_count(byte board[]) {
  byte count = 0;
  for (int i = 0; i < 9; i++) {
    if (board[i] != 0) {
      count++;
    }
  }
  return count;
}

// this could do a lot of things
// it could return a true diff, to be interrogated to ensure only one move changed
// here, we report the first new marker and ignore other changes
byte Sensor::diff_game_board(byte board[]) {
  for (int i = 0; i < 9; i++) {
    if (board[i] != 0 && game_board->get_board()[i] == 0) {
      return i;
    }
  }
  return NO_VAL;
}




//byte Sensor::detect_start() {
//  byte board[9];
//  uint16_t object_cnt = pixy.getBlocks();
//  if (object_cnt) {
//    frame_hold++;
//    if (frame_hold % 64 == 0) {
//      decode_board(board, object_cnt);
//    }
//  }
//  return NO_VAL;
//}

// {
//    sprintf(buf, "Detected %d:\n", object_cnt);
//    Serial.print(buf);
//    for (int j = 0; j < object_cnt; j++)
//    {
//      sprintf(buf, "  block %d: ", j);
//      Serial.print(buf);
//
//      if (pixy.blocks[j].y < 67) {
//        pos = 0;
//      } else if (pixy.blocks[j].y < 133) {
//        pos = 3;
//      } else {
//        pos = 6;
//      }
//
//      if (pixy.blocks[j].x > 220) {
//        pos += 2;
//      } else if (pixy.blocks[j].x > 120) {
//        pos += 1;
//      }
//      sprintf(buf, "  %c mark in position %d: ", pixy.blocks[j].signature == 1 ? 'X' : 'O', pos);
//      Serial.print(buf);
//      pixy.blocks[j].print();
//    }
//
// }
