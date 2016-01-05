//#define NO_UARM

#include <arduino.h>
#ifndef NO_UARM
  //#include "uArm_library.h"
  //#include <linreg.h>
//#include <uarm_calibration.h>
  #include <uarm_library.h>
#endif

#ifndef uArm_Controller_h
#define uArm_Controller_h

#define STOPPER 2    // LOW = Pressed

#ifdef NO_UARM
  #define PUMP_EN                 6   // HIGH = Pump ON
  #define VALVE_EN                5   // HIGH = Valve OPEN
#endif

#define NO_VAL 255

// position definitions
#define TOP_ROW    -10
#define MID_ROW    -20
#define BOT_ROW    -30

#define LFT_COL     -4
#define MID_COL      0
#define RGT_COL      4

#define MARKERS_Y   -10
#define X_MARKER_X  -8
#define O_MARKER_X   8

#define BOARD_HGT    12
#define MARKER_HGT   12

#define WAIT_X       0
#define WAIT_Y       -20
#define WAIT_Z       20

// these pseudo positions indicate the two marker stacks and the wait (sensor) position
#define X_MARKER_POS 9
#define O_MARKER_POS 10
#define WAIT_POS 11


class uArm_Controller {
  public:
    uArm_Controller();
    void begin(); // call during main init()
    void reset();
    void wait_ready();
    void wait_start();
    void wait_player();
    void new_game(bool play_first);
    void make_move(byte posn);
    void postgame(byte winner);
    void show_board_position(byte posn);

  private:
    byte my_mark;
    byte their_mark;
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

    void move_wait_position();
    void move_marker(double init_x, double init_y, double init_z, double dest_x, double dest_y, double dest_z, int drop_rotation);
    void pickup_drop(bool pickup, double current_x, double current_y, double current_z, int tgt_rotation);
};

#endif

