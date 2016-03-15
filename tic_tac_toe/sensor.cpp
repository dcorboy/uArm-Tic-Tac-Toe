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
      ttt_serial.println(F("Someone made a move?"));
      if (move_pos != NO_VAL && board[move_pos] == player_mark) {
        return move_pos;
      }
    } else {
      ttt_serial.print(board_count(board));
      ttt_serial.print(F(" vs. "));
      ttt_serial.println(game_board->get_turn() + 1);
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

bool Sensor::calibrate() {
  return true;
}

bool Sensor::show_raw_values(){
  return true;
}
#else

byte *Sensor::valid_board(byte board[]) {
  if (check_board(board)) {
    if (boards_equal(board, cached_board)) {
      //ttt_serial.println(stable_hold);
      if (++stable_hold >= STABLE_HOLD) {
        stable_hold = 0;
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
  //ttt_serial.println(object_cnt);
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
    //    } else {ttt_serial.println(blank_hold);}
  }
  return NULL;
}

// always returns whatever board it sees, overwriting board parameter completely
// will probably be incorrect if some Pixy functions are called too quickly
byte *Sensor::decode_board(byte board[], uint16_t object_cnt) {
  char buf[32];

  for (int x = 0; x < 9; x++) {
    board[x] = 0;
  }

  if (object_cnt) {
    for (int j = 0; j < object_cnt; j++) {
      int xpos = pixy.blocks[j].x;
      int ypos = pixy.blocks[j].y;
      if (xpos > left && xpos < right && ypos > top && ypos < bottom) { // filter blocks outside the board
        decode_block(board, pixy.blocks[j]);
      } else {
        // ttt_serial.print("Outside board: ");
        // pixy.blocks[j].print();
      }
    }
  } else {
    ttt_serial.println(F("No objects"));
  }
  return board;
}

// always returns whatever board it sees, overwriting board parameter completely
// will probably be incorrect if some Pixy functions are called too quickly
byte *Sensor::decode_block(byte board[], Block block) {
  byte results[9];
  byte covers = pos_covered(block, results);
  for (byte i = 0; i < covers; i++) {
    board[results[i]] = block.signature;
  }
}

// this could be improved by adding a margin so that a block must cover the center point PLUS some
// this could sort out valid length or width seperately
byte Sensor::pos_covered(Block block, byte results[9]) {

  if (block.width < CALIB_MARKER_MIN_SIZE || block.height < CALIB_MARKER_MIN_SIZE) {  // filter small blocks
    return 0;
  }

  byte count = 0;

  int block_top = block.y - (block.height / 2);
  int block_left = block.x - (block.width / 2);
  int block_bottom = block.y + (block.height / 2);
  int block_right = block.x + (block.width / 2);


  char buf[64];
  //sprintf(buf, "Block %2d:  top: %3d left: %3d bot: %3d rgt: %3d\n", block.signature, block_top, block_left, block_bottom, block_right);
  //ttt_serial.print(buf);

  int vsixth = (right - left) / 6;
  int hsixth = (bottom - top) / 6;

  int p_left = left + hsixth;
  int p_hmiddle = left + (3 * hsixth);
  int p_right = left + (5 * hsixth);
  int p_top = top + vsixth;
  int p_vmiddle = top + (3 * vsixth);
  int p_bottom = top + (5 * vsixth);

  if (!shown) {
    //sprintf(buf, "left %3d: middle: %3d right: %3d\n", p_left, p_hmiddle, p_right);
    //ttt_serial.print(buf);
    //sprintf(buf, "top %3d: middle: %3d bottom: %3d\n", p_top, p_vmiddle, p_bottom);
    //ttt_serial.print(buf);
    shown = true;
  }

  if (block_top < p_top ) {
    if (block_left < p_left) {
      results[count++] = 0;
    }
    if (block_left < p_hmiddle && block_right > p_hmiddle) {
      results[count++] = 1;
    }
    if (block_right > p_right) {
      results[count++] = 2;
    }
  }

  if (block_top < p_vmiddle && block_bottom > p_vmiddle) {
    if (block_left < p_left) {
      results[count++] = 3;
    }
    if (block_left < p_hmiddle && block_right > p_hmiddle) {
      results[count++] = 4;
    }
    if (block_right > p_right) {
      results[count++] = 5;
    }
  }

  if (block_bottom > p_bottom) {
    if (block_left < p_left) {
      results[count++] = 6;
    }
    if (block_left < p_hmiddle && block_right > p_hmiddle) {
      results[count++] = 7;
    }
    if (block_right > p_right) {
      results[count++] = 8;
    }
  }
  if (count <= 3) {
    //ttt_serial.print("This block covers: ");
    //ttt_serial.println(count);
    return count;
  } else {
    return 0;
  }
}

bool Sensor::calibrate() {
  uint16_t object_cnt = pixy.getBlocks();
  byte found = 0;

  if (object_cnt > 0) {
    frame_hold++;
    if (frame_hold % 64 == 0) {
      for (int i = 0; i < object_cnt; i++) {
        if (pixy.blocks[i].width > CALIB_MARKER_MIN_SIZE && pixy.blocks[i].height > CALIB_MARKER_MIN_SIZE) {  // filter small blocks
          if (pixy.blocks[i].signature == 1 && pixy.blocks[i].x < CALIB_CENTER_X && pixy.blocks[i].y < CALIB_CENTER_Y) {
            // if there's an X in the upper left that is big enough, we get here
            top = pixy.blocks[i].y - (pixy.blocks[i].height / 2);
            left = pixy.blocks[i].x - (pixy.blocks[i].width / 2);
            found++;
            char buf[64];
            sprintf(buf, "Found top: %3d  left: %3d\n", top, left);
            ttt_serial.print(buf);
          } else if (pixy.blocks[i].signature == 2 && pixy.blocks[i].x > CALIB_CENTER_X && pixy.blocks[i].y > CALIB_CENTER_Y) {
            // if there's an O in the bottom-right that is big enough, we get here
            bottom = pixy.blocks[i].y + (pixy.blocks[i].height / 2);
            right = pixy.blocks[i].x + (pixy.blocks[i].width / 2);
            found++;
            char buf[64];
            sprintf(buf, "Found bottom: %3d  right: %3d\n", bottom, right);
            ttt_serial.print(buf);
          }
        }
      }
    }
  }
  return (found == 2);
}

bool Sensor::show_raw_values() {
  uint16_t object_cnt = pixy.getBlocks();

  if (object_cnt > 0) {
    frame_hold++;
    if (frame_hold % 64 == 0) {
      char buf[64];
      for (int i = 0; i < object_cnt; i++) {
        sprintf(buf, "Block %2d:  x: %3d y: %3d -- wid: %3d hgt: %3d\n", pixy.blocks[i].signature, pixy.blocks[i].x, pixy.blocks[i].y, pixy.blocks[i].width, pixy.blocks[i].height);
        ttt_serial.print(buf);
      }
      return true;
    }
  }
  return false;
}
#endif

bool Sensor::boards_equal(byte board_a[], byte board_b[]) {
  ttt_serial.print(board_count(board_a));
  ttt_serial.print(F(" vs. "));
  ttt_serial.println(board_count(board_b));
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
//    ttt_serial.print(buf);
//    for (int j = 0; j < object_cnt; j++)
//    {
//      sprintf(buf, "  block %d: ", j);
//      ttt_serial.print(buf);
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
//      ttt_serial.print(buf);
//      pixy.blocks[j].print();
//    }
//
// }
