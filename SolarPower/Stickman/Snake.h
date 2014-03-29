class Snake{
  public:
  int activeCube;
  int moveTime;
  public:
  Snake(int, int);
  int getActiveCube(){ return activeCube; };
  void setActiveCube( int );
  void setMoveTime( int );
  int getMoveTime() { return moveTime; };
};