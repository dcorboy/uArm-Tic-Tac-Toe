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
//#include "uArm_library.h"
#include <Servo.h>
#include "gameboard.h"
#include "gamelogic.h"

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
  switch (state) {
    case WAIT_READY :
      if (Serial.available() > 0 && Serial.read() == 'r') {
        change_state(WAIT_START);
      }
      break;
    case WAIT_START :
      if (Serial.available() > 0) {
        if (Serial.read() == 'f') {
          start_game(true);
        } else if (Serial.read() == 's') {
          start_game(false);
        }
      }
      break;
    case PLAYER_TURN :
      move = detect_player_move();
      if (move != NO_POSN) {
        board.set_posn(move);
        Serial.print("Player moves to: ");
        Serial.println(move);
        change_state(board.game_over() ? POSTGAME : UARM_TURN);
      } else if (Serial.available() > 0 && Serial.read() == 'q') {
        change_state(POSTGAME);
      }
      break;
    case UARM_TURN :
      move = logic.do_move();
      board.set_posn(move);
      Serial.print("UArm moves to: ");
      Serial.println(move);
      change_state(board.game_over() ? POSTGAME : PLAYER_TURN);
      break;
    case POSTGAME :
      if (Serial.available() > 0 && Serial.read() == 's') {
        change_state(WAIT_READY);
      }
      break;
  }
}

byte detect_player_move() {
  if (Serial.available() > 0) {
    byte m = Serial.read() - '0';
    if (m >= 0 && m < 9) {
      return m;
    }
  } else {
    return NO_POSN;
  }
}

void start_game(bool player_first) {
  logic.new_game(!player_first);
  player_mark = player_first ? 1 : 2;
  Serial.println(F("The game has begun"));
  change_state(player_first ? PLAYER_TURN : UARM_TURN);
}

void change_state(byte new_state) {
  switch (new_state) {
    case WAIT_READY :
      board.reset();
      Serial.println("Waiting for board to be (R)eady...");
      break;
    case WAIT_START :
      Serial.println("Waiting for you to go (F)irst, unless you want to go (S)econd");
      break;
    case PLAYER_TURN :
      break;
    case UARM_TURN :
      break;
    case POSTGAME :
      Serial.println("The game is over  (S)kip");
      break;
  }
  state = new_state;
}


//  if (Serial.available() > 0)
//  {
//
//    char readSerial = Serial.read();
//    Serial.println(readSerial);
//
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
//
//    if (readSerial == 'r')
//    {
//      Serial.println("Reset Board");
//      logic.new_game(false);
//    }
//
//    byte m = readSerial - '0';
//
//    if (m >= 0 && m < 9)
//    {
//      Serial.print("You moved to: ");
//      Serial.println(m);
//      board.set_posn(m);
//      byte cpu = logic.do_move();
//      Serial.print("Computer moved to: ");
//      Serial.println(cpu);
//      board.set_posn(cpu);
//      for (byte i = 0; i < 9; i++) {
//          Serial.print(i);
//          Serial.print(" = ");
//          Serial.println(board.get_posn(i));
//      }
//    }
//
//    if (readSerial == 't')
//    {
//      logic.test();
//      Serial.print("Rand: ");
//      Serial.println(rand() % 2);
//    }

//    //----------------------------------  function 1  ------------------------------------
//    // function below is for move uArm from current position to absolute coordinate
//    // x = 13, y = -13, z = 3
//
//    if (readSerial == '1') {
//      uarm.moveTo(13, -13, 3);
//      delay(1000);
//    }
//
//    //----------------------------------  function 1  ------------------------------------
//    // function below is for move uArm from current position to absolute coordinate
//    // x = 13, y = -13, z = 3
//
//    if (readSerial == '1') {
//      uarm.moveTo(13, -13, 3);
//      delay(1000);
//    }
//
//
//
//    //----------------------------------  function 7  ------------------------------------
//    // function below is atach all servos
//
//    if (readSerial == 'a') {
//      uarm.attachAll();
//    }
//
//    //----------------------------------  function 8  ------------------------------------
//    // function below is detach all servos
//
//    if (readSerial == 'd') {
//      uarm.detachAll();
//    }
//
//    //----------------------------------  function 9  ------------------------------------
//    // function below is for calibrate uArm
//
//    // if (readSerial == 'c') {
//    //   calib.calibrations();
//    // }
//
//    //----------------------------------  function 10  ------------------------------------
//    // function below is for print current x,y,z absolute location
//
//    if (readSerial == 'g') {
//      Serial.print("The current location is ");
//      Serial.print(uarm.getCalX());
//      Serial.print(" , ");
//      Serial.print(uarm.getCalY());
//      Serial.print(" , ");
//      Serial.print(uarm.getCalZ());
//      Serial.println();
//      delay(1000);
//    }
//
//    if (readSerial == 'k') {
//      Serial.print("SERVO_ROT_NUM offset:");
//      Serial.println(uarm.readServoOffset(SERVO_ROT_NUM));
//      Serial.print("SERVO_LEFT_NUM offset:");
//      Serial.println(uarm.readServoOffset(SERVO_LEFT_NUM));
//      Serial.print("SERVO_RIGHT_NUM offset:");
//      Serial.println(uarm.readServoOffset(SERVO_RIGHT_NUM));
//      Serial.print("SERVO_HAND_ROT_NUM offset:");
//      Serial.println(uarm.readServoOffset(SERVO_HAND_ROT_NUM));
//    }



// } // close read available
// }
