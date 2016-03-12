# uArm-Tic-Tac-Toe

uArm Tic Tac Toe is an Arduino C++ sketch for the [uArm Metal](https://ufactory.cc/uarm/) robotic arm, allowing for completely autonomous play of tic-tac-toe.

![uArm Tic-Tac-Toe](uArm-Tic-Tac-Toe.jpg?raw=true "uArm Tic-Tac-Toe")

## Motivation

The goal for the robot is to play fully autonomously, like a human. This defines five major components I decided were needed to fulfill that goal.

* **Understand the Game**  
The robot must understand how the game is played and have some sense of a strategy for winning.
* **Sense the Board**  
The robot must be able to interpret the moves that the player makes and understand the positions of  the pieces on the board.
* **Move the Pieces**  
To robot must make its own moves at the appropriate times, using whichever marker has been assigned.
* **Play Autonomously**  
All the programming must reside within the robot itself and it should play without any connected computer or external signal.
* **Convey Emotion**  
This is a “stretch goal” to see if the robot can convey emotion to the player, based on the status of the game.

## Installation

The tip runs against [uArm-Developer/UArmForArduino:commit:5b81537](https://github.com/uArm-Developer/UArmForArduino/commit/5b81537bcd81900fea62f8b81161c5e8a5b6bf5c) so make sure that you have this library installed in your Arduino environment and that you have calibrated your uArm for use with the Arduino firmware.

## Comments

I wanted to publish this, but have not had time to document it. The code is currently a mix of debug/test routines and calibrations, in addition to the actual gameplay engine.

The code is designed to run in either "NO_PIXY" mode or "NO_UARM" mode so I could test each side of the same codebase before they were integrated. With the recent changes to the uArm-Developer/UArmForArduino, the NO_UARM version likely needs updating.

I would like to document more fully later.

More information on the project is available at [Cormerica Robot Tic-Tac-Toe](http://www.cormerica.com/robot-tic-tac-toe/)

## Releases

I thought this section was important given the hardware implications of the upcoming commits. SHA references are to [uArm-Developer/UArmForArduino](https://github.com/uArm-Developer/UArmForArduino)

* **v1.0** uArm [b81537](https://github.com/uArm-Developer/UArmForArduino/commit/5b81537bcd81900fea62f8b81161c5e8a5b6bf5c) Plays in NO_PIXY mode over standard serial connection

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