class Dot{
  public:
	int activeCube;
	int moveTime;
	float yOffset;
	bool atEnd;
  public:
	Dot(int, int, float);
	int getActiveCube(){ return activeCube; };
	void setActiveCube( int );
	void setMoveTime( int );
	float getyOffset(){ return yOffset; };
	void setyOffset( float );
	int getMoveTime() { return moveTime; };
	void move();
};