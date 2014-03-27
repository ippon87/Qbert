#include "Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

using namespace std;

class Cube{
public:
	color4 cubeColors[36];
	point4 middle;
	point4 topcords;
	bool flagged;
	int top, bottom, left, right;
	public:
	Cube() {};
	Cube(GLfloat x, GLfloat y,GLfloat z, vec4 color[]){
		middle = vec4(x,y,z,1.0);
		topcords = vec4(middle.x, middle.y+0.5, middle.z, 1.0);
		flagged = false;
		for(int i = 0; i<36; i++)
			cubeColors[i] = color[i];
 }
 
	void Flag(vec4 color)
	{
		if(flagged)
		{	
			return;
		}
		else{
			flagged = true;
			for(int i = 17; i<23; i++){
				cubeColors[i] = color;
		}
		}
	}
};