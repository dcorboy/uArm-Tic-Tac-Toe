#include "sensor.h"

Pixy pixy;

Sensor::Sensor() {
  reset();
}

void Sensor::begin() {
  pixy.init();
}

void Sensor::reset() {
  turn = 0;
  for (int i = 0; i < 9; i++) {
    cached_board[i] = 0;
  }
}

bool Sensor::board_ready() {
  return false;
}

byte Sensor::detect_start() {
  byte board[9];
  uint16_t object_cnt = pixy.getBlocks();
  if (object_cnt) {
    frame_hold++;
    if (frame_hold % 64 == 0) {
      decode_board(board, object_cnt);
    }
  }
  return NO_VAL;
}

byte Sensor::detect_player_move() {
  // if there is a single piece on the board, that counts as a player move
  return NO_VAL;
}

byte *Sensor::valid_board(byte board[]) {
  if (check_board(board)) {
    if (boards_equal(board, cached_board)) {
      if (--stable_hold == 0) {
        stable_hold = STABLE_HOLD;
        return board;
      }
    } else {
      memcpy(cached_board, board, 9);
      stable_hold = STABLE_HOLD;
    }
  }
  return NULL;
}

// returns a board after an appropriate amount of time, otherwise NULL
byte *Sensor::check_board(byte board[]) {
  uint16_t object_cnt = pixy.getBlocks();
  if (object_cnt > 0) {
    frame_hold++;
    if (frame_hold % 64 == 0) {
      decode_board(board, object_cnt);
      return board;
    }
  }
  return NULL;
}

// always returns whatever board it sees, overwriting board parameter completely
// will probably be incorrect if some Pixy functions are called too quickly
byte *Sensor::decode_board(byte board[], uint16_t object_cnt) {
  char buf[32];
  byte pos;
  for (int x = 0; x < 9; x++) {
    board[x] = 0;
  }
  if (object_cnt) {
    for (int j = 0; j < object_cnt; j++) {
      if (pixy.blocks[j].y < 67) {
        pos = 0;
      } else if (pixy.blocks[j].y < 133) {
        pos = 3;
      } else {
        pos = 6;
      }

      if (pixy.blocks[j].x > 220) {
        pos += 2;
      } else if (pixy.blocks[j].x > 120) {
        pos += 1;
      }
      board[pos] = (byte)pixy.blocks[j].signature;
//      sprintf(buf, "  %c mark in position %d: ", pixy.blocks[j].signature == 1 ? 'X' : 'O', pos);
//      Serial.print(buf);
//      pixy.blocks[j].print();
    }
  } else {
    Serial.println("No objects");
  }
  return board;
}

bool Sensor::boards_equal(byte board_a[], byte board_b[]) {
  for (int i = 0; i < 9; i++) {
    if (board_a[i] != board_b[i]) {
      return false;
    }
  }
  return true;
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
