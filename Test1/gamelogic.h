#define NO_POSN 255

#define MODE_EASY   0
#define MODE_MEDIUM 1
#define MODE_HARD   2

class GameLogic {
  public:
    GameLogic(GameBoard* the_board);
    void new_game(bool play_first);
    byte do_move();
    void test();

  private:
    GameBoard* board;
    byte (GameLogic::*logic_node)();
    byte my_mark;
    byte their_mark;
    byte mode;
    byte defense_pattern;

    // move logics
    byte move_easy();
    byte move_medium();
    byte first_x_hard();
    byte first_o_hard();

    // test functions
    bool open(byte posn);
    bool mine(byte posn);
    bool theirs(byte posn);
    bool test(byte posn, byte player);
    byte win_possible(byte player);
    byte win_path(byte player, const byte path[3]);

    // move selections
    byte any_open();
    byte open_edge();
};
