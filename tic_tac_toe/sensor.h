#include <arduino.h>
#include <SPI.h>  
#include <Pixy.h>

#define NO_VAL 255

class Sensor {
  public:
    Sensor();
    void begin(); // call during main init()
    void reset();
    bool board_ready();
    byte detect_start();
    byte detect_player_move();
    byte *decode_board(byte board[], uint16_t object_cnt);
    byte *check_board(byte board[]);

  private:
    byte frame_hold = 0;
    byte board[9];
    byte turn;
};
