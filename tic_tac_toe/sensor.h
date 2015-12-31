#include <arduino.h>
#include <SPI.h>  
#include <Pixy.h>

#define NO_VAL 255

#define STABLE_HOLD  3  // board must appear the same for this many cycles for it to be considered valid

class Sensor {
  public:
    Sensor();
    void begin(); // call during main init()
    void reset();
    bool board_ready();
    byte detect_start();
    byte detect_player_move();
    byte *valid_board(byte board[]);
    byte *check_board(byte board[]);

  private:
    byte frame_hold = 0;
    byte stable_hold = STABLE_HOLD;
    byte cached_board[9];
    byte turn;
    bool boards_equal(byte board_a[], byte board_b[]);
    byte *decode_board(byte board[], uint16_t object_cnt);
};
