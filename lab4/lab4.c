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

// Lägg till egna globaler här efter behov.
float CohesionRadius = 200.0f;
float SeperationRadius = 100.0f;
float CohesionWeight = 0.02f;
float SeperationWeight = 0.01f;
float AlignmentWeight = 0.01f;
float maxSpeed = 5.0f;

// add two Fpoints
FPoint addPoint(FPoint p1, FPoint p2)
{
    FPoint p;
    p.v = p1.v + p2.v;
    p.h = p1.h + p2.h;
    return p;
}

// subtract two Fpoints
FPoint subPoint(FPoint p1, FPoint p2)
{
    FPoint p;
    p.v = p1.v - p2.v;
    p.h = p1.h - p2.h;
    return p;
}
FPoint divPoint(FPoint p1, float d)
{
    FPoint p;
    p.h = p1.h / d;
    p.v = p1.v / d;
    return p;
}

FPoint multPoint(FPoint p1, float f)
{
    FPoint p;
    p.h = p1.h * f;
    p.v = p1.v * f;
    return p;
}

float Length(FPoint p) {
    return sqrt(p.h * p.h + p.v * p.v);
}

FPoint normalize(FPoint p) {
    float length = Length(p);
    p.h = p.h / length;
    p.v = p.v / length;
    return p;
}

// get distance between two FPoints
float getDistance(FPoint p1, FPoint p2)
{
    float diffX = p2.h - p1.h;
    float diffY = p2.v - p1.v;
    return sqrt(diffX*diffX + diffY*diffY);
}

FPoint clamp(FPoint p1, float max)
{
    if (p1.h > max)
        p1.h = max;
    if (p1.h < -max)
        p1.h = -max;
    if (p1.v > max)
        p1.v = max;
    if (p1.v < -max)
        p1.v = -max;
    return p1;
}

void SpriteBehavior() // Din kod!
{
    // Lägg till din labbkod här. Det går bra att ändra var som helst i
    // koden i övrigt, men mycket kan samlas här. Du kan utgå från den
    // globala listroten, gSpriteRoot, för att kontrollera alla sprites
    // hastigheter och positioner, eller arbeta från egna globaler.

    int creaturesInCohesionRadius = 1;
    int creaturesInSeperationRadius = 1;

    SpritePtr current = gSpriteRoot;
    SpritePtr other = gSpriteRoot;

    FPoint cohesionPoint = {0, 0};
    FPoint seperationPoint = {0, 0};
    FPoint alignmentPoint = {0, 0};

    FPoint cohesionDir = {0, 0};
    FPoint seperationDir = {0, 0};
    FPoint alignmentDir = {0, 0};

    FPoint diffSpeed = {0, 0};
    FPoint seperationVec = {0, 0};

    float distance = 0;

    while(current) {

        cohesionPoint = current->position;
        cohesionDir = multPoint(cohesionDir, 0.0f);
        seperationDir = multPoint(seperationDir, 0.0f);
        alignmentDir = multPoint(alignmentDir, 0.0f);

        while(other) {

            // ignore when pointing to the same creature
            if (current == other) {
                other = other->next;
                continue;
            }
            distance = getDistance(current->position, other->position);

            // if creature is within the different CohesionRadius
            if (distance < CohesionRadius) {

                creaturesInCohesionRadius++;

                // for cohesion
                cohesionPoint = addPoint(cohesionPoint, other->position);

                // for alignment
                diffSpeed = addPoint(diffSpeed, normalize(subPoint(other->speed, current->speed)));
            }
            if (distance < SeperationRadius) {

                // for seperation
                seperationVec = multPoint(normalize(subPoint(current->position, other->position)), SeperationRadius-distance);
                seperationPoint = addPoint(seperationPoint, seperationVec);
                creaturesInSeperationRadius++;
            }

            other = other->next;
        }

        if (creaturesInSeperationRadius > 1) {

            seperationPoint = divPoint(seperationPoint, creaturesInSeperationRadius);
            seperationDir = multPoint(seperationPoint, SeperationWeight);
        }

        if (creaturesInCohesionRadius > 1) {
            cohesionPoint = divPoint(cohesionPoint, creaturesInCohesionRadius);
            cohesionDir = normalize(subPoint(cohesionPoint, current->position));
            cohesionDir = multPoint(cohesionDir, CohesionWeight);

            alignmentDir = multPoint(diffSpeed, AlignmentWeight);

        }

        FPoint direction = addPoint(addPoint(cohesionDir, seperationDir), alignmentDir);

        current->speed = addPoint(current->speed, direction);

        current->speed = clamp(current->speed, maxSpeed);

        other = gSpriteRoot;
        current = current->next;
        creaturesInCohesionRadius = 1;
        creaturesInSeperationRadius = 1;
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

// read key input to change variables in run time
void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '.':
        CohesionWeight += 0.01;
        printf("kCohesionWeight = %f\n", CohesionWeight);
        break;
    case ',':
        CohesionWeight -= 0.01;
        printf("kCohesionWeight = %f\n", CohesionWeight);
        break;
    case 'l':
        SeperationWeight += 0.01;
        printf("kAvoidanceWeight = %f\n", SeperationWeight);
        break;
    case 'k':
        SeperationWeight -= 0.01;
        printf("kAvoidanceWeight = %f\n", SeperationWeight);
        break;
    case 'o':
        AlignmentWeight += 0.01;
        printf("kAlignmentWeight = %f\n", AlignmentWeight);
        break;
    case 'i':
        AlignmentWeight -= 0.01;
        printf("kAlignmentWeight = %f\n", AlignmentWeight);
        break;
    case 'm':
        CohesionRadius += 5;
        printf("cohesian_distance = %f\n", CohesionRadius);
        break;
    case 'n':
        CohesionRadius -= 5;
        printf("cohesian_distance = %f\n", CohesionRadius);
        break;
    case 'j':
        SeperationRadius += 5;
        printf("separation_distance = %f\n", SeperationRadius);
        break;
    case 'h':
        SeperationRadius -= 5;
        printf("separation_distance = %f\n", SeperationRadius);
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
