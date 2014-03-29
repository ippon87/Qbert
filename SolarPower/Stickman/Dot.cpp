#include "Dot.h"

Dot::Dot( int startPos, int startTime, float offset ) {
  activeCube = startPos;
  moveTime = startTime;
  yOffset = offset;
}

void Dot::setActiveCube( int newPos ) {
  activeCube = newPos;
}

void Dot::setMoveTime( int newTime ) {
	moveTime = newTime;
}

void Dot::setyOffset( float amount) {
	yOffset += amount;
}

void Dot::move() {

}