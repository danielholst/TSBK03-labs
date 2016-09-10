// Lab 1-1, multi-pass rendering with FBOs and HDR.
// Revision for 2013, simpler light, start with rendering on quad in final stage.
// Switched to low-res Stanford Bunny for more details.
// No HDR is implemented to begin with. That is your task.

// You can compile like this:
// gcc lab1-1.c ../common/*.c -lGL -o lab1-1 -I../common
// gcc lab1-1.c ../common/*.c ../common/Mac/MicroGlut.m -o lab1-1 -framework OpenGL -framework Cocoa -I../common/Mac -I../common

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#ifdef WIN32
// MS
		#include <windows.h>
		#include <stdio.h>
		#include <GL/glew.h>
		#include <GL/glut.h>
	#else
// Linux
		#include <stdio.h>
		#include <GL/gl.h>
		#include "MicroGlut.h"
//		#include <GL/glut.h>
	#endif
#endif

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "zpr.h"

// initial width and heights
#define W 512
#define H 512

#define NUM_LIGHTS 4

void OnTimer(int value);

mat4 projectionMatrix;
mat4 viewMatrix;


GLfloat square[] = {
							-1,-1,0,
							-1,1, 0,
							1,1, 0,
							1,-1, 0};
GLfloat squareTexCoord[] = {
							 0, 0,
							 0, 1,
							 1, 1,
							 1, 0};
GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

Model* squareModel;

//----------------------Globals-------------------------------------------------
Point3D cam, point;
Model *model1;
FBOstruct *originalFBO, *truncFBO, *bloomFBO, *temp;
GLuint phongshader = 0, plaintextureshader = 0, lowpshader = 0, truncshader = 0, addshader = 0;
GLfloat t = 0;

//-------------------------------------------------------------------------------------

void init(void)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	// Load and compile shaders
	plaintextureshader = loadShaders("plaintextureshader.vert", "plaintextureshader.frag");  // puts texture on teapot
	phongshader = loadShaders("phong.vert", "phong.frag");  // renders with light (used for initial renderin of teapot)
    lowpshader = loadShaders("lowPassShader.vert", "lowPassShader.frag"); // shader for the low pass filter
    truncshader = loadShaders("blooming.vert", "blooming.frag"); // truncate to get values over 1
    addshader = loadShaders("addshader.vert", "addshader.frag"); // shader to add different textures

	printError("init shader");

	originalFBO = initFBO(W, H, 0);
	truncFBO = initFBO(W, H, 0);
    bloomFBO = initFBO(W, H, 0);
    temp = initFBO(W, H, 0);
	
    // load the model
//	model1 = LoadModelPlus("teapot.obj");
	model1 = LoadModelPlus("stanford-bunny.obj");

	squareModel = LoadDataToModel(
			square, NULL, squareTexCoord, NULL,
			squareIndices, 4, 6);

	cam = SetVector(0, 5, 15);
	point = SetVector(0, 1, 0);

	glutTimerFunc(5, &OnTimer, 0);

	zprInit(&viewMatrix, cam, point);
}

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(5, &OnTimer, value);
}

void downSampleWithPingPong(int n, FBOstruct* fbo1, FBOstruct* fbo2){
    for(int i = 0; i < n; i++){
        useFBO(fbo2, fbo1, 0L);
        DrawModel(squareModel, lowpshader, "in_Position", NULL, "in_TexCoord");
        useFBO(fbo1, fbo2, 0L);
        DrawModel(squareModel, lowpshader, "in_Position", NULL, "in_TexCoord");
    }
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	mat4 vm2;
	
	// This function is called whenever it is time to render
	//  a new frame; due to the idle()-function below, this
	//  function will get called several times per second

	// render to fbo1!
	useFBO(originalFBO, 0L, 0L);

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
	glUseProgram(phongshader);

	vm2 = viewMatrix;
	// Scale and place bunny since it is too small
	vm2 = Mult(vm2, T(0, -8.5, 0));
	vm2 = Mult(vm2, S(80,80,80));

	glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	//glUniform3fv(glGetUniformLocation(phongshader, "camPos"), 1, &cam.x);
	//glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	DrawModel(model1, phongshader, "in_Position", "in_Normal", NULL);

	// Done rendering the FBO! Set up for rendering on screen, using the result as texture!

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    glUseProgram(truncshader);
    glUniform1f(glGetUniformLocation(truncshader, "texUnit"), 0);
    
    useFBO(truncFBO, originalFBO, 0L);
    DrawModel(squareModel, truncshader, "in_Position", NULL, "in_TexCoord");

    // low pass filtering upg. 1b
    
    glUseProgram(lowpshader);
    
    glUniform1f(glGetUniformLocation(lowpshader, "texUnit"), 0);

    downSampleWithPingPong(8, truncFBO, temp);
    /*
    for (int i = 0; i < 10; i++) {
        useFBO(temp, truncFBO, 0L);
        DrawModel(squareModel, lowpshader, "in_Position", NULL, "in_TexCoord");
        useFBO(truncFBO, temp, 0L);
        DrawModel(squareModel, lowpshader, "in_Position", NULL, "in_TexCoord");
    }
    */
    // use shader to add together textures
    glUseProgram(addshader);
    glUniform1f(glGetUniformLocation(addshader, "texUnit"), 0);
    glUniform1f(glGetUniformLocation(addshader, "texUnit2"), 1);
    
    useFBO(0L, truncFBO, originalFBO);
    DrawModel(squareModel, addshader, "in_Position", NULL, "in_TexCoord");

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	projectionMatrix = perspective(90, ratio, 1.0, 1000);
}


// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new
// frame
void idle()
{
  glutPostRedisplay();
}

//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(W, H);

	glutInitContextVersion(3, 2);
	glutCreateWindow ("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	init();
	glutMainLoop();
	exit(0);
}

