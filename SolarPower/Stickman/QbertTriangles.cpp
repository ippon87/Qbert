//
// Display two color cubes.  Allow the user to turn them using the mouse.
// The smaller cube rotates around the larger one.
//
//  Modified by Hjálmtýr Hafsteinsson, Feb. 2014
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an perspective projection
//   as the default projetion.

using namespace std;
#include "Angel.h"
#include "Cube.cpp"
#include "ply.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

static int spinx = 25;       // Snúningur í x-hniti
static int spiny = -40;       // Snúningur í y-hniti
int origx, origy;           // Upphafleg staða músar
GLfloat curr_z = 10.0;       // Núverandi z-hnit auga
GLfloat curr_x = 0.0;       // Núverandi x-hnit auga
GLfloat rotAngle = 0.0;     // Snúningshorn minni tenings

GLint modelview;            // Staðsetning líkanafylkis í hnútalitara
GLint projection;           // Staðsetning ofanvarpfylkis í hnútalitara

const int NumVertices = 1584; //(6 faces)(2 triangles/face)(3 vertices/triangle)(44 cubes)

point4 points[NumVertices]; //óbreytilegir punktar fyrir pýramíddann
color4 colors[NumVertices]; //litir fyrir pýramíddann , placeholder verður inní cubes
vec4 pyramidarray[44]; //heldur utan um miðjur á kubbum
Cube cubearray[44];	//heldur utan um alla kubba, array position notað fyrir move()
int Index = 0; //globar breyta fyrir quad()
int pyrIndex = 0; //heldur utam um hvert við erum komin í cubearray og pyramidarray
int activeCube = 0; //index í cubearray hvar við erum
int quadrant = 1; //á hvaða fjórðung við erum að horfa á sbr. camera angle
Model_PLY PLYfile;


//bjó til mitt eigið abs() func
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

//basic quad func frá fyrri verkefnum, býr til eina hlið á kubb
void quad(vec4 cubecords[], int a, int b, int c, int d, int side )
{
    colors[Index] = green_shades[side]; points[Index] = cubecords[a]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[b]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[c]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[a]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[c]; Index++;
    colors[Index] = green_shades[side]; points[Index] = cubecords[d]; Index++;
}

//skilar true ef að kubbur sem reynt er að byggja er þegar til
bool searchInPyramidArray(GLfloat x, GLfloat y, GLfloat z)
{
	vec4 compare = vec4(x, y, z, 1.0);
	for(int i = 0; i < 44; i++)
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

//Býr til kubb og pushar í pyramidarray og cubearray
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


//Recursive fall sem að býr til pýramíddann (nema útlagar sem við þurftum að "svindla" pínu)
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

//gefur öllum cubes í cubearray left, right, top og bottom sem int sem vísar í indexof() fyrir hreyfingu
//frá þeim kubb
void findDirections()
{
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
}

//fallið sem að býr til allann pýramíddann
void pyramidinator()
{
	for(int i = 0 ; i<4; i++){
		cubinator(0,0-i, 0, i);
		if(i == 2 || i == 3){
			makecube(0.0, -i, -i);
			makecube(0.0, -i, i);
			makecube(i, -i, 0.0);
			makecube(-i, -i, 0.0);
		}
	}

	findDirections();
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

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );
    

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

    // Draw the larger cube
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glutSwapBuffers();
}

//----------------------------------------------------------------------------
// Býr til snúningshorn út frá hreyfingum músar
void mymotion( int x, int y )
{
    spiny = ( spiny + (x - origx) ) % 360 ;
    spinx = ( spinx + (origy - y) ) % 360 ;
    origx = x;
    origy = y;

    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Geymir staðsetningu músar þegar upphaflega er smellt á mús
void mymouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        origx = x;
        origy = y;
    }
}

//----------------------------------------------------------------------------
// Snýr minni tening um þann stærri
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
// Up/niður-örvar færa fær/nær, vinstri/hægri-örvar færa til hliðar
void myspecialkey ( int key, int x, int y )
{
    switch(key) {
        case GLUT_KEY_UP:
			{
			activeCube = cubearray[activeCube].top;
			cout << activeCube;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
			cout << "x: " << spinx << " , y: " << spiny;
            break;
        case GLUT_KEY_DOWN:
            {
			activeCube = cubearray[activeCube].bottom;
			cout << activeCube;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
            break;
        case GLUT_KEY_LEFT:
            {
			activeCube = cubearray[activeCube].left;
			cout << activeCube;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
            break;
        case GLUT_KEY_RIGHT:
            {
			activeCube = cubearray[activeCube].right;
			cout << activeCube;
            cubearray[activeCube].Flag(vec4(1.0,1.0,0.0,1.0));
			}
            break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
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