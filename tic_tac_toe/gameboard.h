#include <arduino.h>

#ifndef GameBoard_h
#define GameBoard_h

class GameBoard {
  public:
    GameBoard();
    void reset();
    void set_posn(byte pos);
    byte get_posn(byte pos);
    bool valid_move(byte pos);
    bool full();
    bool game_over();
    byte winner();
    byte get_turn();
    void get_path(byte path, byte path_ary[]);
    byte *get_board();

  private:
    byte board[9];
    byte turn;
};

#endif

