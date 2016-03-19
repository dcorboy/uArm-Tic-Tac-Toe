#include "uarm.h"

#ifdef NO_UARM

#define SERVO_ROT_NUM              1
#define SERVO_LEFT_NUM              2
#define SERVO_RIGHT_NUM             3
#define SERVO_HAND_ROT_NUM            4

#define MATH_PI  3.141592653589793238463

class uArmClass {
  public:
    uArmClass();

    void moveTo(double x, double y, double z, int relative, double time);
    void moveTo(double x, double y, double z, int relative, double time_spend, int servo_4_relative, double servo_4_angle);

    void getCalXYZ(double theta_1, double theta_2, double theta_3, double& x, double& y, double &z) {};
    void getCalXYZ(double& x, double& y, double &z) {};
    void alert(byte times, byte runTime, byte stopTime);

    double readAngle(byte servo_num) {};
    void moveToAtOnce(double x, double y, double z, int relative, double servo_4_angle) {};
};

uArmClass::uArmClass() {}
void uArmClass::moveTo(double x, double y, double z, int relative, double time) {
}
void uArmClass::moveTo(double x, double y, double z, int relative, double time_spend, int servo_4_relative, double servo_4_angle) {
}
void uArmClass::alert(byte times, byte runTime, byte stopTime) {}

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
  debug_detach();
}

void uArm_Controller::wait_start() {
  move_wait_position();
  debug_detach();
}

void uArm_Controller::wait_player() {
  move_wait_position();
  debug_detach();
}

void uArm_Controller::new_game(bool play_first) {
  // a new game has just begun!
  // do whatever to show that the arm is ready to play
  // maybe different depending on who is going first
  my_mark = play_first ? 1 : 2;
  their_mark = play_first ? 2 : 1;
}

void uArm_Controller::make_move(byte posn) {
  ttt_serial.print(F("Move to position: "));
  ttt_serial.print(posn);
  ttt_serial.print(F(" -- X: "));
  ttt_serial.print(board_positions[posn][0]);
  ttt_serial.print(F(", Y: "));
  ttt_serial.print(board_positions[posn][1]);
  ttt_serial.print(F(", Z: "));
  ttt_serial.println(BOARD_HGT);
  move_marker(my_mark == 1 ? X_MARKER_X : O_MARKER_X, my_mark == 1 ? X_MARKER_Y : O_MARKER_Y, MARKER_HGT, board_positions[posn][0], board_positions[posn][1], BOARD_HGT);
  debug_detach();
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
    y = (posn == X_MARKER_POS ? X_MARKER_Y : O_MARKER_Y);
    z = MARKER_HGT;
  } else if (posn == WAIT_POS) {
    x = WAIT_X;
    y = WAIT_Y;
    z = WAIT_Z;
  }
  ttt_serial.print(F("Move to position: "));
  ttt_serial.print(posn);
  ttt_serial.print(F(" -- X: "));
  ttt_serial.print(x);
  ttt_serial.print(F(", Y: "));
  ttt_serial.print(y);
  ttt_serial.print(F(", Z: "));
  ttt_serial.println(z);
  move_to(x, y, z, hand_offset(0), 1);
  debug_detach();
}

void uArm_Controller::show_xyz() {
  double x;
  double y;
  double z;

  uarm.getCalXYZ(x, y, z);
  ttt_serial.print(F("Curr: "));
  ttt_serial.print(x);
  ttt_serial.print(F(", "));
  ttt_serial.print(y);
  ttt_serial.print(F(", "));
  ttt_serial.println(z);

  ttt_serial.print(F("Rot: "));
  ttt_serial.println(uarm.readAngle(SERVO_ROT_NUM));
  ttt_serial.print(F("Left: "));
  ttt_serial.println(uarm.readAngle(SERVO_LEFT_NUM));
  ttt_serial.print(F("Right: "));
  ttt_serial.println(uarm.readAngle(SERVO_RIGHT_NUM));
  ttt_serial.print(F("Hand: "));
  ttt_serial.println(uarm.readAngle(SERVO_HAND_ROT_NUM));
}

// private members

void uArm_Controller::move_wait_position() {
  move_to(WAIT_X, WAIT_Y, WAIT_Z, hand_offset(0), 1.5);
}

// this is the main move procedure to pickup a playing piece at one location and drop it at another
// init_x, init_y and init_z are the initial position (pickup)
// dest_x, dest_y, dest_z are the destiination position (drop)
// drop_rotation is the rotation degree of end-effector to rotate the object before setting it down
void uArm_Controller::move_marker(double init_x, double init_y, double init_z, double dest_x, double dest_y, double dest_z) {
  int start_rot = hand_offset(-round(atan(init_x / init_y) * 180.0 / MATH_PI * 0.85));
  int end_rot = hand_offset(-round(atan(dest_x / dest_y) * 180.0 / MATH_PI * 0.85));
  //  ttt_serial.print("Start Rot: ");
  //  ttt_serial.println(start_rot);
  //  ttt_serial.print("End Rot: ");
  //  ttt_serial.println(end_rot);
  move_to(init_x, init_y, init_z, start_rot, 1);              // move to the initial position
    // uarm.moveToOpts(init_x, init_y, init_z, 0, F_HAND_ROT_REL, 1, PATH_ANGLES, INTERP_EASE_INOUT);
  pickup_drop(true, init_x, init_y, init_z, start_rot);               // move end-effector downwards until stopper hits something, then pick it up
  move_to(WAIT_X, WAIT_Y, init_z, hand_offset(0), 1);              // move to a common central position
    // uarm.moveToOpts(WAIT_X, WAIT_Y, init_z, 0, F_HAND_ROT_REL, 1, PATH_ANGLES, INTERP_EASE_INOUT);
  move_to(dest_x, dest_y, dest_z, end_rot, 1);              // move to the destination position
    // uarm.moveToOpts(dest_x, dest_y, dest_z, 0, F_HAND_ROT_REL, 1, PATH_ANGLES, INTERP_EASE_INOUT);
  pickup_drop(false, dest_x, dest_y, dest_z, end_rot);  // move end-effector downwards until stopper hits something, then drop it
  debug_detach();
}

void uArm_Controller::pickup_drop(bool pickup, double current_x, double current_y, double current_z, int tgt_rotation) {
  int stopper = HIGH;  // assume that we're not touching anything
  if (pickup) {
    while (stopper && current_z > 4.0) {
      stopper = digitalRead(STOPPER);  // low means we hit something
      if (stopper == HIGH) { // if we haven't yet, move downwards a bit at a time
        current_z = current_z - .5;  // move downwards (too slow will not depress the limit switch)
        move_to(current_x, current_y, current_z, tgt_rotation, 0);  // a moveToAtOnce
          // uarm.moveToOpts(current_x, current_y, current_z, 0, F_HAND_RELATIVE, 0, PATH_ANGLES, INTERP_EASE_INOUT);
        delay(100);
      } else {
        //ttt_serial.println(F("stopper hit!"));
      }
    }
  } else {  // dropping
    move_to(current_x, current_y, DROP_HGT, tgt_rotation, .5);
      // uarm.moveToOpts(current_x, current_y, DROP_HGT, 0, F_HAND_RELATIVE, .5, PATH_ANGLES, INTERP_EASE_INOUT);
  }

  // at this point, the stopper is depressed so we can pickup or release via the pump
  switch (pickup) {
    case true:                      // pick up
      digitalWrite(VALVE_EN, LOW);  // valve closed
      digitalWrite(PUMP_EN, HIGH);  // pump on
      delay(500);                   // wait 500 ms to make sure object is captured
      move_to(current_x, current_y, current_z + 3, tgt_rotation, 1);  // lift the captured object up off the pile
      break;
    case false:                     // drop
      digitalWrite(PUMP_EN, LOW);   // pump off
      digitalWrite(VALVE_EN, HIGH); // valve open
      delay(750);                   // let the air escape
      move_to(current_x, current_y, DROP_HGT + 3, tgt_rotation, .5);  // lift the head up to clear the playing area
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

void uArm_Controller::debug_detach() {
  uarm.detachAll();
  ttt_serial.println(F("Servos detached!"));
}

