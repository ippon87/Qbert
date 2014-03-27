//
// Display two color cubes.  Allow the user to turn them using the mouse.
// The smaller cube rotates around the larger one.
//
//  Modified by Hjálmtýr Hafsteinsson, Feb. 2014
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an perspective projection
//   as the default projetion.

#include "Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

static int spinx = 0;       // Snúningur í x-hniti
static int spiny = 0;       // Snúningur í y-hniti
int origx, origy;           // Upphafleg staða músar
GLfloat curr_z = 2.0;       // Núverandi z-hnit auga
GLfloat curr_x = 0.0;       // Núverandi x-hnit auga
GLfloat rotAngle = 0.0;     // Snúningshorn minni tenings

GLint modelview;            // Staðsetning líkanafylkis í hnútalitara
GLint projection;           // Staðsetning ofanvarpfylkis í hnútalitara

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};


//----------------------------------------------------------------------------
// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------
// OpenGL initialization
void
init()
{
    colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

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

    // Draw the smaller cube
    mv *= RotateY( rotAngle );
    mv *= Translate( 2.0, 0.0, 0.0 );
    mv *= Scale( 0.2, 0.2, 0.2  );
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

	// Draw the smaller cube
    mv *= RotateY( 2*rotAngle );
    mv *= Translate( 1.0, 0.0, 0.0 );
    mv *= Scale( 0.5, 0.5, 0.5  );
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
    rotAngle += 0.5;
    if( rotAngle > 360.0 ) rotAngle -= 360.0;

    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Up/niður-örvar færa fær/nær, vinstri/hægri-örvar færa til hliðar
void myspecialkey ( int key, int x, int y )
{
    switch(key) {
        case GLUT_KEY_UP:
            curr_z += 0.1;
            break;
        case GLUT_KEY_DOWN:
            curr_z -= 0.1;
            break;
        case GLUT_KEY_LEFT:
            curr_x -= 0.1;
            break;
        case GLUT_KEY_RIGHT:
            curr_x += 0.1;
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