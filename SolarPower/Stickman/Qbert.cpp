class Qbert{
  int activeCube;
  int lives;
  public:
  Qbert(int, int);
  int getActiveCube(){ return activeCube };
  void setActiveCube( int );
  void moveUp( Cube, vec4 );
  void moveDown( Cube, vec4 );
  void moveLeft( Cube, vec4 );
  void moveRight( Cube, vec4 );
  void changeCubeColor( Cube, vec4 );
  int decreaseLives();
};

Qbert::Qbert( int startPos, int startLives ) {
  activeCube = startPos;
  lives = startLives;
}

Qbert::setActiveCube( int newPos ) {
  activeCube = newPos;
}

Qbert::moveUp( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->activeCube() ].top );
  this->changeCubeColor( cubeArray, color);
}

Qbert::moveDown( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].bottom );
  this->changeCubeColor( cubeArray, color);
}

Qbert::moveLeft( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].left );
  this->changeCubeColor( cubeArray, color);
}

Qbert::moveRight( Cube *cubeArray, vec4 color ) {
  this->setActiveCube( cubeArray[ this->getActiveCube() ].right );
  this->changeCubeColor( cubeArray, color);
}

Qbert::changeCubeColor( Cube *cubeArray, vec4 color ) {
  cubeArray[ this->getActiveCube() ].Flag( color );
}

Qbert::decreaseLives() {
  lives -= 1;
  return lives;
}