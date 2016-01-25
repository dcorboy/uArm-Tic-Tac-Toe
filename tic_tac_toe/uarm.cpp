#include "uarm.h"

#ifdef NO_UARM
class uArmClass {
  public:
    uArmClass();

    void moveTo(double x, double y, double z, int relative, double time);
    void moveTo(double x, double y, double z, int relative, double time_spend, int servo_4_relative, double servo_4_angle);
};

uArmClass::uArmClass() {}
void uArmClass::moveTo(double x, double y, double z, int relative, double time) {
}
void uArmClass::moveTo(double x, double y, double z, int relative, double time_spend, int servo_4_relative, double servo_4_angle) {
}
uArmClass uarm;
#endif

uArm_Controller::uArm_Controller() {
}

void uArm_Controller::begin() {
  pinMode(STOPPER, INPUT_PULLUP);  digitalWrite(STOPPER, HIGH);
  pinMode(BTN_D4,   INPUT_PULLUP);  digitalWrite(BTN_D4,   HIGH);
  pinMode(BTN_D7,   INPUT_PULLUP);  digitalWrite(BTN_D7,   HIGH);
  pinMode(BUZZER,   OUTPUT); digitalWrite(BUZZER,   LOW);
  pinMode(PUMP_EN,  OUTPUT); digitalWrite(PUMP_EN,  LOW);
  pinMode(VALVE_EN, OUTPUT); digitalWrite(VALVE_EN, LOW);
  //digitalWrite(STOPPER, HIGH);  // in case of old Arduino
  //wait wait, should this be INPUT_PULLUP?
  my_mark = 1;
}

void uArm_Controller::alert(byte beeps) {
  uarm.alert(beeps, 75, 75);
}

void uArm_Controller::wait_ready() {
  // we may have just woken up, but in any case
  // it is now time to think about playing a game
  // do whatever to show this, then get into position for the sensor to check board state
  move_wait_position();
}

void uArm_Controller::wait_start() {
  move_wait_position();
}

void uArm_Controller::wait_player() {
  move_wait_position();
}

void uArm_Controller::new_game(bool play_first) {
  // a new game has just begun!
  // do whatever to show that the arm is ready to play
  // maybe different depending on who is going first
  my_mark = play_first ? 1 : 2;
  their_mark = play_first ? 2 : 1;
}

void uArm_Controller::make_move(byte posn) {
  //  for (int i = 0; i < 9; i++) {
  //    Serial.print(F("Board positions: "));
  //    Serial.print(i);
  //    Serial.print(F(" -- X: "));
  //    Serial.print(board_positions[i][0]);
  //    Serial.print(", Y: ");
  //    Serial.println(board_positions[i][1]);
  //  }
#if 1
  Serial.print(F("Move to position: "));
  Serial.print(posn);
  Serial.print(F(" -- X: "));
  Serial.print(board_positions[posn][0]);
  Serial.print(F(", Y: "));
  Serial.print(board_positions[posn][1]);
  Serial.print(F(", Z: "));
  Serial.println(BOARD_HGT);
#endif
  move_marker(my_mark == 1 ? X_MARKER_X : O_MARKER_X, MARKERS_Y, MARKER_HGT, board_positions[posn][0], board_positions[posn][1], BOARD_HGT);
}

// debug function
// 0 - 8        = board positions
// X_MARKER_POS = X marker stack position
// O_MARKER_POS = X marker stack position
// WAIT_POS     = wait (observer) position
void uArm_Controller::show_board_position(byte posn) {
  double x, y, z;

  if (posn >= 0 && posn < 9) {
    x = board_positions[posn][0];
    y = board_positions[posn][1];
    z = BOARD_HGT;
  } else if (posn == X_MARKER_POS || posn == O_MARKER_POS) {
    x = (posn == X_MARKER_POS ? X_MARKER_X : O_MARKER_X);
    y = MARKERS_Y;
    z = MARKER_HGT;
  } else if (posn == WAIT_POS) {
    x = WAIT_X;
    y = WAIT_Y;
    z = WAIT_Z;
  }
  Serial.print(F("Move to position: "));
  Serial.print(posn);
  Serial.print(F(" -- X: "));
  Serial.print(x);
  Serial.print(F(", Y: "));
  Serial.print(y);
  Serial.print(F(", Z: "));
  Serial.println(z);
  uarm.moveTo(x, y, z, 0, 1, false, hand_offset(0));
}

void uArm_Controller::show_xyz() {
  double x;
  double y;
  double z;

  uarm.getCalXYZ(x, y, z);
  Serial.print("Curr: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.println(z);

  Serial.print(F("Rot: "));
  Serial.println(uarm.readAngle(SERVO_ROT_NUM));
  Serial.print(F("Left: "));
  Serial.println(uarm.readAngle(SERVO_LEFT_NUM));
  Serial.print(F("Right: "));
  Serial.println(uarm.readAngle(SERVO_RIGHT_NUM));
  Serial.print(F("Hand: "));
  Serial.println(uarm.readAngle(SERVO_HAND_ROT_NUM));
}

// private members

void uArm_Controller::move_wait_position() {
  uarm.moveTo(WAIT_X, WAIT_Y, WAIT_Z, 0, 1.5, false, hand_offset(0));
}

// this is the main move procedure to pickup a playing piece at one location and drop it at another
// init_x, init_y and init_z are the initial position (pickup)
// dest_x, dest_y, dest_z are the destiination position (drop)
// drop_rotation is the rotation degree of end-effector to rotate the object before setting it down
void uArm_Controller::move_marker(double init_x, double init_y, double init_z, double dest_x, double dest_y, double dest_z) {
  int start_rot = hand_offset(-round(atan(init_x / init_y) * 180.0 / MATH_PI));
  int end_rot = hand_offset(-round(atan(dest_x / dest_y) * 180.0 / MATH_PI));
  uarm.moveTo(init_x, init_y, init_z, false, 1, false, start_rot);              // move to the initial position
  pickup_drop(true, init_x, init_y, init_z, start_rot);               // move end-effector downwards until stopper hits something, then pick it up
  uarm.moveTo(0, 0, 4, true, 1, false, start_rot);                           // lift the captured object up off the pile
  uarm.moveTo(dest_x, dest_y, dest_z, false, 1, false, end_rot);              // move to the destination position
  pickup_drop(false, dest_x, dest_y, dest_z, end_rot);  // move end-effector downwards until stopper hits something, then drop it
  uarm.moveTo(0, 0, 4, true, 1, false, end_rot);      // move the end-effector upwards to clear the playing area
}


void uArm_Controller::down_to_touch() {
  double current_x, current_y, current_z;
  uarm.getCalXYZ(current_x, current_y, current_z);
  int stopper = HIGH;  // assume that we're not touching anything
  while (stopper && current_z > 4.5) {
    stopper = digitalRead(STOPPER);  // low means we hit something
    Serial.print(current_z);
    Serial.println(stopper);
    if (stopper == HIGH) { // if we haven't yet, move downwards a bit at a time
      current_z = current_z - .5;  // move downwards 2 cm every .5s (slower will not depress the limit switch)
      uarm.moveToAtOnce(current_x, current_y, current_z, false, 0);
      delay(250);
    }
  }
}

void uArm_Controller::attach_release(bool pickup) {

  // at this point, the stopper is depressed so we can pickup or release via the pump
  switch (pickup) {
    case true:                      // pick up
      digitalWrite(VALVE_EN, LOW);  // valve closed
      digitalWrite(PUMP_EN, HIGH);  // pump on
      delay(500);                   // wait 500 ms to make sure object is captured
      break;
    case false:                     // drop
      digitalWrite(PUMP_EN, LOW);   // pump off
      digitalWrite(VALVE_EN, HIGH); // valve open
      delay(500);                   // let the air escape
      digitalWrite(VALVE_EN, LOW);  // valve closed
      break;
    default:
      break;
  }
}

void uArm_Controller::pickup_drop(bool pickup, double current_x, double current_y, double current_z, int tgt_rotation) {
  int stopper = HIGH;  // assume that we're not touching anything
  if (pickup) {
    while (stopper && current_z > 4.0) {
      stopper = digitalRead(STOPPER);  // low means we hit something
      if (stopper == HIGH) { // if we haven't yet, move downwards a bit at a time
        current_z = current_z - .5;  // move downwards (too slow will not depress the limit switch)
        uarm.moveToAtOnce(current_x, current_y, current_z, false, tgt_rotation);
        delay(100);
      } else {
          Serial.println("stopper hit!");
      }
    }
  } else {
    uarm.moveTo(current_x, current_y, 7, false, .5, false, tgt_rotation);
  }

  // at this point, the stopper is depressed so we can pickup or release via the pump
  switch (pickup) {
    case true:                      // pick up
      digitalWrite(VALVE_EN, LOW);  // valve closed
      digitalWrite(PUMP_EN, HIGH);  // pump on
      delay(500);                   // wait 500 ms to make sure object is captured
      break;
    case false:                     // drop
      digitalWrite(PUMP_EN, LOW);   // pump off
      digitalWrite(VALVE_EN, HIGH); // valve open
      delay(500);                   // let the air escape
      digitalWrite(VALVE_EN, LOW);  // valve closed
      break;
    default:
      break;
  }
}

void uArm_Controller::postgame(byte winner) {
  // if winner, 3 xy circles at sensor hgt
  // if loser, shake "head" back and forth as shrink back to 0,0,0
}

