#define NO_PIXY

#include <arduino.h>
#include "gameboard.h"
#include "ttt_serial.h"
#include <SPI.h>
//#ifndef NO_PIXY
#include <Pixy.h>
//#endif

#define NO_VAL 255

#define STABLE_HOLD   3  // board must appear the same for this many cycles for it to be considered valid
#define BLANK_HOLD   500  // board must appear blank for this many cycles for it to be considered valid

#define CALIB_MARKER_MIN_SIZE 40  // calibration markers must be larger than this
#define CALIB_CENTER_X        160  // center of X resolution
#define CALIB_CENTER_Y        100  // center of X resolution

class Sensor {
  public:
    Sensor(GameBoard* the_board);
    void begin(); // call during main init()
    void reset();
    bool board_ready();
    byte detect_start();
    byte detect_player_move();
    byte *valid_board(byte board[]);
    byte *check_board(byte board[]);
    bool show_raw_values();
    bool calibrate();

  private:
    int top = 12; // defaults
    int left = 53;
    int bottom = 188;
    int right = 237;
    GameBoard* game_board;
    byte frame_hold = 0;
    // uint16_t blank_hold = 0;
    byte stable_hold = 0;
    byte cached_board[9];
    byte player_mark;
    bool shown = false;

    bool boards_equal(byte board_a[], byte board_b[]);
    byte *decode_board(byte board[], uint16_t object_cnt);
    byte *decode_block(byte board[], Block block);
    byte pos_covered(Block block, byte results[9]);
    byte board_count(byte board[]);
    byte diff_game_board(byte board[]);
};
