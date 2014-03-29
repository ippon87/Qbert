#include "Qbert.h" 

Qbert::Qbert( int startPos, int startLives ) {
  activeCube = startPos;
  lives = startLives;
}

void Qbert::setActiveCube( int newPos ) {
  activeCube = newPos;
}

/*void Qbert::moveUp( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].top );
  this->changeCubeColor( cubeArray, color);
}

void Qbert::moveDown( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].bottom );
  this->changeCubeColor( cubeArray, color);
}

void Qbert::moveLeft( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].left );
  this->changeCubeColor( cubeArray, color);
}

void Qbert::moveRight( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].right );
  this->changeCubeColor( cubeArray, color);
}

void Qbert::changeCubeColor( Cube *cubeArray, vec4 color ) {
  cubeArray[ this->getActiveCube() ].Flag( color );
}*/

int Qbert::decreaseLives() {
  lives -= 1;
  return lives;
}