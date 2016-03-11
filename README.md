# uArm-Tic-Tac-Toe

uArm Tic Tac Toe is an Arduino C++ sketch for the [uArm Metal](https://ufactory.cc/uarm/) robotic arm, allowing for completely autonomous play of tic-tac-toe.

## Motivation


## Installation

Tip runs against uArm-Developer/UArmForArduino@5b81537bcd8 so make sure that you have this library installed in your Arduino environment and that you have calibrated your uArm for use with the Arduino firmware.

## Comments

I wanted to publish this, but have not had time to document it. The code is currently a mix of debug/test routines and calibrations, in addition to the actual gameplay engine.

The code is designed to run in either "NO_PIXY" mode or "NO_UARM" mode so I could test each side of the same codebase before they were integrated. With the recent changes to the uArm-Developer/UArmForArduino, the NO_UARM version likely needs updating.

I would like to document more fully later.

More information on the project is available at [Cormerica Robot Tic-Tac-Toe](http://www.cormerica.com/robot-tic-tac-toe/)

## Files

* **tic_tac_toe.ino** Game controller and debug/calibration menus. Moves are entered via the serial monitor if NO_PIXY.
* **gameboard** Holds current gameboard state.
* **gamelogic** Logic for determining moves in the game. Implements three levels of difficulty (one unbeatable).
* **sensor** Manages Pixy and responds with the observed move.
* **uarm** Manages uArm manipulation of game markers and camera position.

## License

[GNU General Public License](http://www.gnu.org/licenses/)

&copy; Copyright 2016 Dave Corboy <dave@corboy.com>

These files constitute the uArm-Tic-Tac-Toe software.

uArm-Tic-Tac-Toe is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

uArm-Tic-Tac-Toe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

If you did not receive a copy of the GNU General Public License
along with uArm-Tic-Tac-Toe, see <http://www.gnu.org/licenses/>.