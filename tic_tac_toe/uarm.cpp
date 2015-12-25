#include "uarm.h"

uArm_Controller::uArm_Controller() {
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
  move_marker(my_mark == 1 ? X_MARKER_X : O_MARKER_X, MARKERS_Y, MARKER_HGT, board_positions[posn][0], board_positions[posn][1], BOARD_HGT, 0);
}

// private members

void uArm_Controller::move_wait_position() {
  uarm.moveTo(0, -13, 20, false, 3);
}

// this is the main move procedure to pickup a playing piece at one location and drop it at another
// init_x, init_y and init_z are the initial position (pickup)
// dest_x, dest_y, dest_z are the destiination position (drop)
// drop_rotation is the rotation degree of end-effector to rotate the object before setting it down
void uArm_Controller::move_marker(double init_x, double init_y, double init_z, double dest_x, double dest_y, double dest_z, int drop_rotation) {
  uarm.moveTo(init_x, init_y, init_z, false, 4);              // move to the initial position
  pickup_drop(true, init_x, init_y, init_z, 0);               // move end-effector downwards until stopper hits something, then pick it up
  uarm.moveTo(0, 0, 10, true, 1.5);                           // lift the captured object up off the pile
  uarm.moveTo(dest_x, dest_y, dest_z, false, 3);              // move to the destination position
  pickup_drop(false, dest_x, dest_y, dest_z, drop_rotation);  // move end-effector downwards until stopper hits something, then drop it
  uarm.moveTo(0, 0, 6, true, 1.5, false, drop_rotation);      // move the end-effector upwards to clear the playing area
  // uarm.moveTo(dest_x, dest_y, dest_z);                     // move to destination
}


void uArm_Controller::pickup_drop(bool pickup, double current_x, double current_y, double current_z, int tgt_rotation) {
  int stopper = HIGH;  // assume that we're not touching anything
  while (stopper) {
    stopper = digitalRead(STOPPER);  // low means we hit something
    if (stopper == HIGH) { // if we haven't yet, move downwards a bit at a time
      uarm.moveTo(current_x, current_y, current_z, false, 0.5, false, tgt_rotation);
      current_z = current_z - 0.2;  // move downwards 0.2 cm every .5s
      delay(50);
    }
  }

  // at this point, the stopper is depressed so we can pickup or release via the pump
  switch (pickup) {
    case true:                  // pick up
      digitalWrite(6, HIGH);    // pump on
      digitalWrite(5, LOW);     // valve closed
      delay(500);               // wait 500 ms to make sure object is captured
      break;
    case false:                     // drop
      digitalWrite(6, LOW);     // pump off
      digitalWrite(5, HIGH);    // valve open
      break;
    default:
      break;
  }
}

void uArm_Controller::postgame(byte winner) {
  // if winner, 3 xy circles at sensor hgt
  // if loser, shake "head" back and forth as shrink back to 0,0,0
}

