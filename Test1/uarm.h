#include <arduino.h>
#include "uArm_library.h"

#define NO_VAL 255

class uArm_Controller {
  public:
    uArm_Controller();
    void wait_ready();
    void wait_start();
    void new_game(bool play_first);
    void make_move(byte posn);
    void postgame(byte winner);

  private:
    byte board[9];
    byte my_mark;
    byte their_mark;

    void pickup_unplayed_marker();
    void place_held_marker(byte posn);
};
