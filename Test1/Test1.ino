/******************************************************************************************
  File Name          : Test.ino
  Author             : Joey Song
  Version            : V1.0
  Date               : 26 Aug, 2014
  Modified Date      : 19 Nov, 2015
  Description        : This documents is for quick start with uArm Metal version
  Copyright(C) 2015 uArm Team. All right reserved.
*******************************************************************************************/

/*
   Table of Content

   Function 1 - 4 :    move to a certain point (f)
   Fucntion 5 - 6 :    move a Rectangle or a curve (function 5-6)
   Function 7 - 8 :    attach or detach all servos (function 7-8)
   Function 9     :    uArm calibration
   Function 10    :    read current coordinate x,y,z
   Function 11    :    recording mode

*/

// headers should must include these four headers

#include <EEPROM.h>
#include <Wire.h>
#include <Servo.h>
#include "gameboard.h"
#include "gamelogic.h"
#include "sensor.h"
#include "uarm.h"

// define a uArm
//uArmLibrary uArm;

#define led 13  // built-in LED
#define WAIT_READY    0   // the game control states
#define WAIT_START    1
#define PLAYER_TURN   2
#define UARM_TURN     3
#define POSTGAME      4

GameBoard board;
GameLogic logic(&board);
Sensor sensor;
uArm_Controller uarm;

byte state;
byte player_mark;

void setup() {
  pinMode(led, OUTPUT);
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial port at 9600 bps
  change_state(WAIT_READY);
}


void loop() {
  byte move;
  byte input = NO_VAL;
  if (Serial.available() > 0) {
    input = Serial.read();
  }
  switch (state) {
    case WAIT_READY :
      if (sensor.board_ready() || input == 'r') {
        change_state(WAIT_START);
      }
      break;
    case WAIT_START :
      {
        byte player_first = sensor.detect_start();
        if (player_first != NO_VAL || input != NO_VAL) {
          if (input == 'f') {
            player_first = true;
          } else if (input == 's') {
            player_first = false;
          }
          start_game(player_first);
        }
      }
      break;
    case PLAYER_TURN :
      {
        move = sensor.detect_player_move();
        if (input != NO_VAL) {
          byte num = input - '0';
          if (num >= 1 && num <= 8) {
            if (board.valid_move(num - 1)) {
              move = num - 1;
            } else {
              Serial.println("Move is not valid");
            }
          }
        }
        if (move != NO_VAL) {
          board.set_posn(move);
          Serial.print("Player moves to: ");
          Serial.println(move + 1);
          change_state(board.game_over() ? POSTGAME : UARM_TURN);
        } else if (input == 'q') {
          change_state(POSTGAME);
        }
      }
      break;
    case UARM_TURN :
      move = logic.do_move();
      board.set_posn(move);
      Serial.print("UArm moves to: ");
      Serial.println(move + 1);
      uarm.make_move(move);
      change_state(board.game_over() ? POSTGAME : PLAYER_TURN);
      break;
    case POSTGAME :
      if (input == 's') {
        change_state(WAIT_READY);
      }
      break;
  }
}

void start_game(bool player_first) {
  logic.new_game(!player_first);
  uarm.new_game(!player_first);
  player_mark = player_first ? 1 : 2;
  Serial.println(F("The game has begun"));
  change_state(player_first ? PLAYER_TURN : UARM_TURN);
}

void change_state(byte new_state) {
  switch (new_state) {
    case WAIT_READY :
      board.reset();
      sensor.reset();
      Serial.println("Waiting for board to be (R)eady...");
      break;
    case WAIT_START :
      Serial.println("Waiting for you to go (F)irst, unless you want to go (S)econd");
      break;
    case PLAYER_TURN :
      Serial.println("Waiting for player move (or 1..8)");
      break;
    case UARM_TURN :
      break;
    case POSTGAME :
      {
        byte winner = board.winner();
        Serial.println("The game is over  (S)kip");
        if (winner != 0) {
          Serial.print(winner == player_mark ? "Player" : "uArm");
          Serial.println(" is the winner!");
        } else {
          Serial.println("The game is a draw...");
        }
        break;
      }
  }
  state = new_state;
}


//    if (readSerial == 'h') // Single Quote! This is a character.
//    {
//      digitalWrite(led, HIGH);
//      Serial.print(" LED ON ");
//    }
//
//    if (readSerial == 'l')
//    {
//      digitalWrite(led, LOW);
//      Serial.print(" LED OFF");
//    }

