class Dot{
  public:
	int activeCube;
	int moveTime;
	float yOffset;
  public:
	Dot(int, int, float);
	int getActiveCube(){ return activeCube; };
	void setActiveCube( int );
	void setMoveTime( int );
	int getMoveTime() { return moveTime; };
	void move();
};