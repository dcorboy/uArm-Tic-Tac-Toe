#include <arduino.h>
//#include "uArm_library.h"
#include "uarm.h"

uArm_Controller::uArm_Controller() {
}

void uArm_Controller::new_game(bool play_first) {
  my_mark = play_first ? 1 : 2;
  their_mark = play_first ? 2 : 1;
}

void uArm_Controller::make_move(byte posn) {
  pickup_unplayed_marker();
  place_held_marker(posn);
}

void uArm_Controller::pickup_unplayed_marker() {
  // uarm.moveTo(13, -13, 3);
  delay(1000);
}

void uArm_Controller::place_held_marker(byte posn) {
  // uarm.moveTo(-13, 13, 3);
  delay(1000);
}

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
