// Demo of heavily simplified sprite engine
// by Ingemar Ragnemalm 2009
// used as base for lab 4 in TSBK03.
// OpenGL 3 conversion 2013.

// gcc lab4.c ../common/*.c ../common/Mac/MicroGlut.m -o lab4 -framework OpenGL -framework Cocoa SpriteLight.c -I../common/Mac -I../common

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	//#include <GL/gl.h>
	#include "MicroGlut.h"
#endif

#include <stdlib.h>
#include "LoadTGA.h"
#include "SpriteLight.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"

#define radius 100.0

// Lägg till egna globaler här efter behov.

// add two Fpoints
FPoint addPoint(FPoint p1, FPoint p2)
{
    FPoint p;
    p.v = p1.v + p2.v;
    p.h = p1.h + p2.h;
    
    return p;
}

FPoint divPoint(FPoint p1, float d)
{
    FPoint p;
    p.h = p1.h / d;
    p.v = p1.v / d;
    
    return p;
}

float Length(FPoint p) {
    return sqrt(p.h * p.h + p.v * p.v);
}

FPoint normalize(FPoint p) {
    float length = Length(p);
    FPoint retVal;
    retVal.h = p.h / length;
    retVal.v = p.v / length;
    return retVal;
}

// get distance between two FPoints
float getDistance(FPoint p1, FPoint p2)
{
    float diffX = p2.h - p1.h;
    float diffY = p2.v - p1.v;
    
    return sqrt(diffX*diffX + diffY*diffY);
}
void SpriteBehavior() // Din kod!
{
// Lägg till din labbkod här. Det går bra att ändra var som helst i
// koden i övrigt, men mycket kan samlas här. Du kan utgå från den
// globala listroten, gSpriteRoot, för att kontrollera alla sprites
// hastigheter och positioner, eller arbeta från egna globaler.
    
    int creaturesInRadius = 1;

    SpritePtr current = gSpriteRoot;
    SpritePtr other = gSpriteRoot;
    
    FPoint centerPoint = {0, 0};
    
    while(current) {
        centerPoint = current->position;
        while(other) {
            
            // ignore when pointing to the same creature
            if (current == other) {
                other = other->next;
                continue;
            }
            // if next creature is within the radius
            if (getDistance(current->position, other->position) < radius)
            {
                if (getDistance(current->position, other->position) > 10) {
                    centerPoint = addPoint(centerPoint, other->position);
                    creaturesInRadius++;
                }
            }

            other = other->next;
        }
        
        if (creaturesInRadius > 1) {
            centerPoint = divPoint(centerPoint, creaturesInRadius);
            
            FPoint direction = {centerPoint.h - current->position.h,
                                centerPoint.v - current->position.v};

            direction = normalize(direction);
            
            
            float length = Length(current->speed);
            current->speed.h = direction.h * length;
            current->speed.v = direction.v * length;
        }
        other = gSpriteRoot;
        current = current->next;
        creaturesInRadius = 1;
    }
    
}

// Drawing routine
void Display()
{
	SpritePtr sp;
	
	glClearColor(0, 0, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	DrawBackground();
	
	SpriteBehavior(); // Din kod!
	
// Loop though all sprites. (Several loops in real engine.)
	sp = gSpriteRoot;
	do
	{
		HandleSprite(sp); // Callback in a real engine
		DrawSprite(sp);
		sp = sp->next;
	} while (sp != NULL);
	
	glutSwapBuffers();
}

void Reshape(int h, int v)
{
	glViewport(0, 0, h, v);
	gWidth = h;
	gHeight = v;
}

void Timer(int value)
{
	glutTimerFunc(20, Timer, 0);
	glutPostRedisplay();
}

// Example of user controllable parameter
float someValue = 0.0;

void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '+':
    	someValue += 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case '-':
    	someValue -= 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
	
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett får
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart får
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 100, 200, 1, 1);
	NewSprite(sheepFace, 200, 100, 1.5, -1);
	NewSprite(sheepFace, 250, 200, -1, 1.5);
    
    NewSprite(sheepFace, 100, 300, 0.5, 1);
    NewSprite(sheepFace, 200, 400, -1, 1);
    NewSprite(sheepFace, 240, 50, 1, -1);
    NewSprite(sheepFace, 140, 350, 1, 0.5);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("SpriteLight demo / Flocking");
	
	glutDisplayFunc(Display);
	glutTimerFunc(20, Timer, 0); // Should match the screen synch
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	
	InitSpriteLight();
	Init();
	
	glutMainLoop();
	return 0;
}
