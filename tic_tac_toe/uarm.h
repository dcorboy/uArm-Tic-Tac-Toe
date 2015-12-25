#include <arduino.h>
#include "uArm_library.h"

#ifndef uArm_Controller_h
#define uArm_Controller_h

#define STOPPER 2    // LOW = Pressed
#define VALVE   5    // HIGH = Open
#define PUMP    6    // HIGH = On

#define NO_VAL 255

// position definitions
#define TOP_ROW    -10
#define MID_ROW    -13
#define BOT_ROW    -16

#define LFT_COL     -3
#define MID_COL      0
#define RGT_COL      3

#define MARKERS_Y   -6
#define X_MARKER_X  -5
#define O_MARKER_X   5

#define BOARD_HGT    2
#define MARKER_HGT   6

const signed char board_positions[9][2] PROGMEM = {
  {LFT_COL, TOP_ROW},
  {MID_COL, TOP_ROW},
  {RGT_COL, TOP_ROW},
  {LFT_COL, MID_ROW},
  {MID_COL, MID_ROW},
  {RGT_COL, MID_ROW},
  {LFT_COL, BOT_ROW},
  {MID_COL, BOT_ROW},
  {RGT_COL, BOT_ROW}
};

class uArm_Controller {
  public:
    uArm_Controller();
    void wait_ready();
    void wait_start();
    void wait_player();
    void new_game(bool play_first);
    void make_move(byte posn);
    void postgame(byte winner);

  private:
    byte board[9];
    byte my_mark;
    byte their_mark;

    void move_wait_position();
    void move_marker(double init_x, double init_y, double init_z, double dest_x, double dest_y, double dest_z, int drop_rotation);
    void pickup_drop(bool pickup, double current_x, double current_y, double current_z, int tgt_rotation);
};

#endif

