#include "Snake.h"

Snake::Snake( int startPos, int startTime ) {
  activeCube = startPos;
  moveTime = startTime;
}

void Snake::setActiveCube( int newPos ) {
  activeCube = newPos;
}

void Snake::setMoveTime( int newTime ) {
  moveTime = newTime;
}