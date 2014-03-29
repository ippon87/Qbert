//
// Display two color cubes.  Allow the user to turn them using the mouse.
// The smaller cube rotates around the larger one.
//
//  Modified by Hj�lmt�r Hafsteinsson, Feb. 2014
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an perspective projection
//   as the default projetion.

using namespace std;
#include "Angel.h"
#include "Cube.cpp"
#include "Qbert.h"
#include "ply.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

int spinx = 25;       // Sn�ningur � x-hniti
int spiny = -40;       // Sn�ningur � y-hniti
int origx, origy;           // Upphafleg sta�a m�sar
GLfloat curr_z = 10.0;       // N�verandi z-hnit auga
GLfloat curr_x = 0.0;       // N�verandi x-hnit auga

GLint modelview;            // Sta�setning l�kanafylkis � hn�talitara
GLint projection;           // Sta�setning ofanvarpfylkis � hn�talitara
Model_PLY PLYfile;
const int NumVertices = 1584; //(6 faces)(2 triangles/face)(3 vertices/triangle)(44 cubes) + sizeOfPly
    
point4 points[NumVertices]; //�breytilegir punktar fyrir p�ram�dd
color4 colors[NumVertices]; //litir fyrir p�ram�ddann , placeholder ver�ur inn� cubes
vec4 pyramidarray[44]; //heldur utan um mi�jur � kubbum
Cube cubearray[44];	//heldur utan um alla kubba, array position nota� fyrir move()
int Index = 0; //globar breyta fyrir quad()
int pyrIndex = 0; //heldur utam um hvert vi� erum komin � cubearray og pyramidarray
int activeCube = 0; //index � cubearray hvar vi� erum
int quadrant = 1; //� hva�a fj�r�ung vi� erum a� horfa � sbr. camera angle



//bj� til mitt eigi� abs() func
double myabs(double x)
{
	if(x < 0)
		return -x;
	else
		return x;
}



// Vertices of a unit cube centered at origin, sides aligned with axes

// RGBA olors
color4 green_shades[6] = {
    color4( 0.2, 0.6, 0.2, 1.0 ),  // black
    color4( 0.32, 0.86, 0.32, 1.0 ),  // red
    color4( 0.05, 0.7, 0.05, 1.0 ),  // yellow
    color4( 0.27, 0.93, 0.27, 1.0 ),  // green
    color4( 0.2, 0.96, 0.2, 1.0 ),  // blue
    color4( 0.26, 0.7, 0.19, 1.0 )
};

//basic quad func fr� fyrri verkefnum, b�r til eina hli� � kubb
void quad(vec4 cubecords[], int a, int b, int c, int d, int side )
{
    colors[Index] = green_shades[side]; points[Index] = cubecords[a]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[b]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[c]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[a]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[c]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[d]; Index++;
}

//skilar true ef a� kubbur sem reynt er a� byggja er �egar til
bool searchInPyramidArray(GLfloat x, GLfloat y, GLfloat z)
{
	vec4 compare = vec4(x, y, z, 1.0);
	for(int i = 0; i <44; i++)
	{
		//if(pyramidarray[i] == vec4(x, y, z ,1.0))
		if(pyramidarray[i].x == x && pyramidarray[i].y == y && pyramidarray[i].z == z )
		{
			//printf("jubb");
			return true;
		}
	}
	//printf("nope\n");
	return false;
}

//B�r til kubb og pushar � pyramidarray og cubearray
void makecube( GLfloat x, GLfloat y, GLfloat z)
{
	point4 cubecords[8] = {
		point4( x-0.5, y-0.5,  z+0.5, 1.0 ),
		point4( x-0.5,  y+0.5,  z+0.5, 1.0 ),
		point4(  x+0.5,  y+0.5,  z+0.5, 1.0 ),
		point4(  x+0.5, y-0.5,  z+0.5, 1.0 ),
		point4( x-0.5, y-0.5, z-0.5, 1.0 ),
		point4( x-0.5,  y+0.5, z-0.5, 1.0 ),
		point4(  x+0.5,  y+0.5, z-0.5, 1.0 ),
		point4(  x+0.5, y-0.5, z-0.5, 1.0 )
	};

	quad( cubecords, 1, 0, 3, 2, 0 );
    quad( cubecords, 2, 3, 7, 6, 1 );
    quad( cubecords, 3, 0, 4, 7, 2 );
    quad( cubecords, 6, 5, 1, 2, 3 );
    quad( cubecords, 4, 5, 6, 7, 4 );
    quad( cubecords, 5, 4, 0, 1, 5 );

	pyramidarray[pyrIndex] = vec4(x, y, z, 1.0);
	cubearray[pyrIndex] = Cube(x, y, z, colors);
	pyrIndex++;
	
}


//Recursive fall sem a� b�r til p�ram�ddann (nema �tlagar sem vi� �urftum a� "svindla" p�nu)
void cubinator(GLfloat x, GLfloat y, GLfloat z, int i)
{

	makecube(x, y, z);
	if( (int)myabs(x) < i && !searchInPyramidArray(x-1.0, y, z))
	{
		cubinator(x-1.0, y, z, i-1);
	}
	if( (int)myabs(x) < i && !searchInPyramidArray(x+1.0, y, z))
	{
		//printf("x+1");
		cubinator(x+1.0, y, z, i-1);
	}
	if( (int)myabs(z) < i && !searchInPyramidArray(x, y, z-1.0))
	{
		//printf("z-1");
		cubinator(x, y, z-1.0, i-1);
	}
	if( (int)myabs(z) < i && !searchInPyramidArray(x, y, z+1.0))
	{
		//printf("z+1");
		cubinator(x, y, z+1.0, i-1);
	}
}

int pyramidIndex(GLfloat x, GLfloat y, GLfloat z)
{
	for(int i = 0; i < 44; i++)
	{
		//if(pyramidarray[i] == vec4(x, y, z ,1.0))
		if(pyramidarray[i].x == x && pyramidarray[i].y == y && pyramidarray[i].z == z )
		{
			return i;
		}
	}
	//printf("nope\n");
	return -1;
}

//gefur �llum cubes � cubearray left, right, top og bottom sem int sem v�sar � indexof() fyrir hreyfingu
//fr� �eim kubb
void findDirections(int quadrant)
{
	switch(quadrant){
	case 1:
		for(int i = 0; i<44; i++)
		{
			if(searchInPyramidArray(pyramidarray[i].x+1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].right = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].right = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x-1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].left = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].left = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z-1.0))
				cubearray[i].top = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z-1.0);
			else
				cubearray[i].top = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z-1.0);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z+1.0))
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z+1.0);
			else
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z+1.0);
		}
		break;
		
	case 2:
		for(int i = 0; i<44; i++)
		{
			if(searchInPyramidArray(pyramidarray[i].x+1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x-1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].top = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].top = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z-1.0))
				cubearray[i].right = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z-1.0);
			else
				cubearray[i].right = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z-1.0);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z+1.0))
				cubearray[i].left = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z+1.0);
			else
				cubearray[i].left = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z+1.0);
		}
		break;

	case 3:
		for(int i = 0; i<44; i++)
		{
			if(searchInPyramidArray(pyramidarray[i].x+1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].left = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].left = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x-1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].right = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].right = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z-1.0))
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z-1.0);
			else
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z-1.0);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z+1.0))
				cubearray[i].top = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z+1.0);
			else
				cubearray[i].top = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z+1.0);
		}
		break;

		case 4:
		for(int i = 0; i<44; i++)
		{
			if(searchInPyramidArray(pyramidarray[i].x+1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].top = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].top = pyramidIndex(pyramidarray[i].x+1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x-1.0, pyramidarray[i].y, pyramidarray[i].z))
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y+1.0, pyramidarray[i].z);
			else
				cubearray[i].bottom = pyramidIndex(pyramidarray[i].x-1.0, pyramidarray[i].y-1.0, pyramidarray[i].z);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z-1.0))
				cubearray[i].left = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z-1.0);
			else
				cubearray[i].left = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z-1.0);

			if(searchInPyramidArray(pyramidarray[i].x, pyramidarray[i].y, pyramidarray[i].z+1.0))
				cubearray[i].right = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y+1.0, pyramidarray[i].z+1.0);
			else
				cubearray[i].right = pyramidIndex(pyramidarray[i].x, pyramidarray[i].y-1.0, pyramidarray[i].z+1.0);
		}
		break;
	}
}

void findRotation(int quadrant)
{
	switch(quadrant)
	{
	case 1:
		spinx = 25;       
		spiny = -40;
		break;
	case 2:
		spinx = 25;       
		spiny = -126;
		break;
	case 3: 
		spinx = 25;
		spiny = -220;
		break;
	case 4:
		spinx = 25;
		spiny = 47;
		break;
	}
	
	
}

void findQuadrant(bool x, bool z)
{
	if(x && z)
		quadrant = 1;
	if(x && !z)
		quadrant = 2;
	if(!x && !z)
		quadrant = 3;
	if(!x && z)
		quadrant = 4;

	findDirections(quadrant);
	findRotation(quadrant);
}

//falli� sem a� b�r til allann p�ram�ddann
void pyramidinator()
{
	for(int i = 0 ; i<4; i++){
		cubinator(0,0-i, 0, i);
		if(i >= 2 ){
			makecube(0.0, -i, -i);
			makecube(0.0, -i, i);
			makecube(i, -i, 0.0);
			makecube(-i, -i, 0.0);
		}
	}

	findDirections(1);
	//activeCube = cubearray[0];
}









//----------------------------------------------------------------------------
// quad generates two triangles for each face and assigns colors
//    to the vertices


//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors


//----------------------------------------------------------------------------
// OpenGL initialization
void
init()
{
    pyramidinator();

	//load ply stuff
	PLYfile.Load( "teapot-n.ply" );
	GLint sizeOfPly = 3*sizeof(vec4)*PLYfile.NumberOfFaces;

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );
    


	PLYfile.Load( "teapot-n.ply" );
	
    GLint SizeOfVertexVector = 3*sizeof(vec4)*PLYfile.NumberOfFaces;
    GLint SizeOfNormalsVector = 3*sizeof(vec3)*PLYfile.NumberOfFaces;
	cout << SizeOfVertexVector << endl;
    // Create and initialize a buffer object
    GLuint buffer1;
    glGenBuffers( 1, &buffer1 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );
    glBufferData( GL_ARRAY_BUFFER, SizeOfVertexVector+SizeOfNormalsVector, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, SizeOfVertexVector, PLYfile.TriangleVertices );
    glBufferSubData( GL_ARRAY_BUFFER, SizeOfVertexVector, SizeOfNormalsVector, PLYfile.TriangleNormals );



    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshaderctm.glsl", "fshaderctm.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(points)) );

    modelview = glGetUniformLocation( program, "model_view" );
    projection = glGetUniformLocation( program, "projection" );

    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

    // Set the projection
    mat4 p = Perspective( 70.0, 1.0, 0.2, 100.0 );
//  mat4 p = Ortho( -2.0, 2.0, -2.0, 2.0, 0.0, 100.0 );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );

    // Set the model view matrix
    mat4 mv;
    mv *= Translate(curr_x, 0.0, -curr_z);
    mv *= RotateX( (GLfloat)spinx ) * RotateY( (GLfloat)spiny );

    // Teikna kubbanna
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

	
	float x = cubearray[activeCube].topcords.x;
	float y = cubearray[activeCube].topcords.y;
	float z = cubearray[activeCube].topcords.z;
	mat4 likan;
	likan = mv;
	likan *= Translate( x, y, z );
	likan *= Scale(0.2, 0.2, 0.2);
    likan *= RotateX( -90.0 );

	// Teikna l�kani�
	glBufferSubData( GL_ARRAY_BUFFER, 0, 3*sizeof(vec4)*PLYfile.NumberOfFaces, PLYfile.TriangleVertices );
	glUniformMatrix4fv( modelview, 1, GL_TRUE, likan );
    glDrawArrays( GL_TRIANGLES, 0, 3*PLYfile.NumberOfFaces );



    glutSwapBuffers();
}

//----------------------------------------------------------------------------
// B�r til sn�ningshorn �t fr� hreyfingum m�sar
void mymotion( int x, int y )
{
    spiny = ( spiny + (x - origx) ) % 360 ;
    spinx = ( spinx + (origy - y) ) % 360 ;
    origx = x;
    origy = y;

    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Geymir sta�setningu m�sar �egar upphaflega er smellt � m�s
void mymouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        origx = x;
        origy = y;
    }
}

//----------------------------------------------------------------------------
// Sn�r minni tening um �ann st�rri
void myidle()
{
    for(int i = 0; i<44; i++)
	{
		for(int j = i*36; j<(i*36 + 36); j++)
		{
			colors[j] = cubearray[i].cubeColors[j - (i*36)];
		}
	}

	
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Up/ni�ur-�rvar f�ra f�r/n�r, vinstri/h�gri-�rvar f�ra til hli�ar
void myspecialkey ( int key, int x, int y )
{
	bool oldXPos = cubearray[activeCube].middle.x < 0;
	bool oldZPos = cubearray[activeCube].middle.z < 0;
    switch(key) {
        case GLUT_KEY_UP:
			if(cubearray[activeCube].top != -1)
			{
			activeCube = cubearray[activeCube].top;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
			cout << "x: " << spinx << " , y: " << spiny << "\n";
            break;
        case GLUT_KEY_DOWN:
			if(cubearray[activeCube].bottom != -1)
            {
			activeCube = cubearray[activeCube].bottom;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
            break;
        case GLUT_KEY_LEFT:
			if(cubearray[activeCube].left != -1)
            {
			activeCube = cubearray[activeCube].left;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
            break;
        case GLUT_KEY_RIGHT:
			if(cubearray[activeCube].right != -1)
            {
			activeCube = cubearray[activeCube].right;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
            break;
    }
	cout << "activeCube x: " << cubearray[activeCube].middle.x << " , z: " << cubearray[activeCube].middle.z << "\n";
    bool newXPos = cubearray[activeCube].middle.x >= 0;
	bool newZPos = cubearray[activeCube].middle.z >= 0;
		if( oldXPos != newXPos || newZPos != oldZPos){
			findQuadrant(newXPos, newZPos);
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	Qbert qbert(1, 3);
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Rotatable Color Cubes" );

    glewInit();

    init();

    glutDisplayFunc( display );
    glutMouseFunc( mymouse );
    glutMotionFunc( mymotion );
    glutIdleFunc( myidle );
    glutSpecialFunc( myspecialkey );

    glutMainLoop();
    return 0;
}