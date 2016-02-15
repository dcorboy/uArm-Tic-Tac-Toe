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
  Serial.print(F("Move to position: "));
  Serial.print(posn);
  Serial.print(F(" -- X: "));
  Serial.print(board_positions[posn][0]);
  Serial.print(F(", Y: "));
  Serial.print(board_positions[posn][1]);
  Serial.print(F(", Z: "));
  Serial.println(BOARD_HGT);
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
  //uarm.moveTo(x, y, z, 0, 1, false, hand_offset(0));
  move_to_by_angle(x, y, z, hand_offset(0), 1, INTERP_EASE_INOUT);
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
  //uarm.moveTo(WAIT_X, WAIT_Y, WAIT_Z, 0, 1.5, false, hand_offset(0));
  move_to_by_angle(WAIT_X, WAIT_Y, WAIT_Z, hand_offset(0), 1.5, INTERP_EASE_INOUT);

}

// this is the main move procedure to pickup a playing piece at one location and drop it at another
// init_x, init_y and init_z are the initial position (pickup)
// dest_x, dest_y, dest_z are the destiination position (drop)
// drop_rotation is the rotation degree of end-effector to rotate the object before setting it down
void uArm_Controller::move_marker(double init_x, double init_y, double init_z, double dest_x, double dest_y, double dest_z) {
  int start_rot = hand_offset(-round(atan(init_x / init_y) * 180.0 / MATH_PI));
  int end_rot = hand_offset(-round(atan(dest_x / dest_y) * 180.0 / MATH_PI));
  move_to_by_angle(init_x, init_y, init_z, start_rot, 1, INTERP_EASE_INOUT);              // move to the initial position
  pickup_drop(true, init_x, init_y, init_z, start_rot);               // move end-effector downwards until stopper hits something, then pick it up
  //uarm.moveTo(0, 0, 4, true, 1, false, start_rot);                           // lift the captured object up off the pile
  move_to_by_angle(init_x, init_y, init_z, start_rot, 1, INTERP_EASE_INOUT);  // lift the captured object up off the pile by angle
  move_to_by_angle(dest_x, dest_y, dest_z, end_rot, 1, INTERP_EASE_INOUT);              // move to the destination position
  pickup_drop(false, dest_x, dest_y, dest_z, end_rot);  // move end-effector downwards until stopper hits something, then drop it
  // uarm.moveTo(0, 0, 4, true, 1, false, end_rot);      // move the end-effector upwards to clear the playing area
  move_to_by_angle(dest_x, dest_y, 8, end_rot, 1, INTERP_EASE_INOUT);        // move the end-effector upwards to clear the playing area
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
      move_to_by_angle(current_x, current_y, current_z, 0, 0, 0);  // a moveToAtOnce by angle
      //uarm.moveToAtOnce(current_x, current_y, current_z, false, 0);
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
        //uarm.moveToAtOnce(current_x, current_y, current_z, false, tgt_rotation);
        move_to_by_angle(current_x, current_y, current_z, tgt_rotation, 0, 0);  // a moveToAtOnce by angle
        delay(100);
      } else {
        Serial.println("stopper hit!");
      }
    }
  } else {
    //uarm.moveTo(current_x, current_y, 7, false, .5, false, tgt_rotation);
    move_to_by_angle(current_x, current_y, 6, tgt_rotation, .5, INTERP_EASE_INOUT);
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



void uArm_Controller::show_angles(double theta_1, double theta_2, double theta_3, double hand_angle) {
  Serial.print(F("Rot: "));
  Serial.println(theta_1);
  Serial.print(F("Left: "));
  Serial.println(theta_2);
  Serial.print(F("Right: "));
  Serial.println(theta_3);
  Serial.print(F("Hand: "));
  Serial.println(hand_angle);
}

void uArm_Controller::move_to_by_angle(double x, double y, double z, double tgt_hand, float duration, byte ease_type) {

  if (!cur_angles_init) {
    Serial.println("Angles inited!");
    // find current angles
    cur_rot = uarm.readAngle(SERVO_ROT_NUM);
    cur_left = uarm.readAngle(SERVO_LEFT_NUM);
    cur_right = uarm.readAngle(SERVO_RIGHT_NUM);
    cur_hand = uarm.readAngle(SERVO_HAND_ROT_NUM);
    cur_angles_init = true;
    //    } else  {
    //    Serial.println("I thought they were:");
    //    show_angles(cur_rot, cur_left, cur_right, cur_hand);
    //    // find current angles
    //    cur_rot = uarm.readAngle(SERVO_ROT_NUM);
    //    cur_left = uarm.readAngle(SERVO_LEFT_NUM);
    //    cur_right = uarm.readAngle(SERVO_RIGHT_NUM);
    //    cur_hand = uarm.readAngle(SERVO_HAND_ROT_NUM);
    //    Serial.println("But really they were:");
    //    show_angles(cur_rot, cur_left, cur_right, cur_hand);
  }

  // find target angles
  double tgt_rot;
  double tgt_left;
  double tgt_right;
  uarm.getCalAngles(x, y, z, tgt_rot, tgt_left, tgt_right);

  if (duration == 0) {
    uarm.writeAngle(tgt_rot, tgt_left, tgt_right, tgt_hand);  // moveToAtOnce
  } else {

    // interpolate 'em
    double rot[INTERP_INTVLS];
    double left[INTERP_INTVLS];
    double right[INTERP_INTVLS];
    double hand[INTERP_INTVLS];

    interpolate(cur_rot, tgt_rot, 0, INTERP_INTVLS, rot, ease_type);
    interpolate(cur_left, tgt_left, 0, INTERP_INTVLS, left, ease_type);
    interpolate(cur_right, tgt_right, 0, INTERP_INTVLS, right, ease_type);
    interpolate(cur_hand, tgt_hand, 0, INTERP_INTVLS, hand, ease_type);

    execute_move(rot, left, right, hand, duration, false);
    l_movementTrigger = 1;
    uarm.writeAngle(tgt_rot, tgt_left, tgt_right, tgt_hand);
  }
  cur_rot = tgt_rot;
  cur_left = tgt_left;
  cur_right = tgt_right;
  cur_hand = tgt_hand;
  //uarm.detachAll();
}

void uArm_Controller::execute_move(double (&rot)[INTERP_INTVLS], double (&left)[INTERP_INTVLS], double (&right)[INTERP_INTVLS], double (&hand)[INTERP_INTVLS], float duration, bool debug) {
  if (!debug) {
    uarm.attachAll();
  }

  for (byte i = 0; i < INTERP_INTVLS; i++)
  {
    if (!debug) {
      l_movementTrigger = 1;
      uarm.writeAngle(rot[i], left[i], right[i], hand[i]);
      delay(duration * 1000 / INTERP_INTVLS);
    } else {
      show_angles(rot[i], left[i], right[i], hand[i]);
    }
  }
}

void uArm_Controller::interpolate(double start_val, double end_val, byte frame_start, byte frame_dur, double (&interp_val)[INTERP_INTVLS], byte type) {
  double delta = end_val - start_val;
  for (byte f = 0; f < frame_dur; f++) {
    switch (type) {
      case INTERP_LINEAR :
        interp_val[frame_start + f] = delta * f / frame_dur + start_val;
        break;
      case INTERP_EASE_INOUT :
        {
          float t = f / (frame_dur / 2.0);
          //Serial.println(t);
          if (t < 1) {
            interp_val[frame_start + f] = delta / 2 * t * t + start_val;
          } else {
            t--;
            interp_val[frame_start + f] = -delta / 2 * (t * (t - 2) - 1) + start_val;
          }
        }
        break;
      case INTERP_EASE_IN :
        {
          float t = (float)f / frame_dur;
          interp_val[frame_start + f] = delta * t * t + start_val;
        }
        break;
      case INTERP_EASE_OUT :
        {
          float t = (float)f / frame_dur;
          interp_val[frame_start + f] = -delta * t * (t - 2) + start_val;
        }
        break;
    }
  }
}

