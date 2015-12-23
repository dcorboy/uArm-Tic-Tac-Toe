#include <arduino.h>
#include <SPI.h>  
#include <Pixy.h>

#define NO_VAL 255

class Sensor {
  public:
    Sensor();
    void reset();
    bool board_ready();
    byte detect_start();
    byte detect_player_move();

  private:
    Pixy pixy;
    byte board[9];
    byte turn;
};
