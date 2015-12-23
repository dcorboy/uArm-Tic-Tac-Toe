#include <arduino.h>

#ifndef GameBoard_h
#define GameBoard_h

class GameBoard {
  public:
    GameBoard();
    void set_posn(byte pos);
    byte get_posn(byte pos);
    bool valid_move(byte pos);
    bool full();
    bool game_over();
    byte winner();
    byte get_turn();
    void reset();
    byte paths[8][3] = {
      {0, 1, 2},
      {3, 4, 5},
      {6, 7, 8},
      {0, 3, 6},
      {1, 4, 7},
      {2, 5, 8},
      {0, 4, 8},
      {2, 4, 6}
    };

  private:
    byte board[9];
    byte turn;
};

#endif

