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
    board[i] = 0;
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

byte *Sensor::check_board(byte board[]) {
  uint16_t object_cnt = pixy.getBlocks();
  if (object_cnt) {
    frame_hold++;
    if (frame_hold % 64 == 0) {
      //decode_board(board);
      return board;
    }
  }
  return NULL;
}

byte *Sensor::decode_board(byte board[], uint16_t object_cnt) {
  char buf[32];
  byte pos;
  sprintf(buf, "Detected %d:\n", object_cnt);
  Serial.print(buf);
  for (int j = 0; j < object_cnt; j++)
  {
    sprintf(buf, "  block %d: ", j);
    Serial.print(buf);

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
    sprintf(buf, "  %c mark in position %d: ", pixy.blocks[j].signature == 1 ? 'X' : 'O', pos);
    Serial.print(buf);
    pixy.blocks[j].print();
  }
  return board;
}

