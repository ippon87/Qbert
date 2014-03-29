//#include "Cube.cpp"

class Qbert{
public:
  int activeCube;
  int lives;
  public:
  Qbert(int, int);
  int getActiveCube(){ return activeCube; };
  void setActiveCube( int );
  //void moveUp( Cube *, vec4 );
  //void moveDown( Cube *, vec4 );
  //void moveLeft( Cube *, vec4 );
  //void moveRight( Cube *, vec4 );
  //void changeCubeColor( Cube *, vec4 );
  int decreaseLives();
};