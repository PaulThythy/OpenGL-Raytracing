#include <GL/glut.h>
#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#include <iostream>
#include <vector>

#include "sphere.h"
#include "vector3.h"

int screenHeight = 500;
int screenWidth = 500;

float fov = 45.f;   // champ de vision
float zNear = 0.1f; // Distance de l'écran par rapport à la caméra
float aspectRatio = (float)screenWidth / (float)screenHeight;

void display();
void displayBasis();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void mouseMotion(int, int);

// global variables for openGL
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX, cameraAngleY;
float cameraDistance = 0;

void initOpenGl();
Vector3 getRayDirection(int, int);

int main(int argc, char **argv)
{
    /* glut init and window creation */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Raytracing");

    /* OpenGL init */

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    initOpenGl();

    glutMainLoop();
    return 0;
}

void displayBasis() {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0., 0.);
    glVertex2f(1., 0.);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(0., 0.);
    glVertex2f(0., 1.);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., 1.);
    glEnd();
}

void display()
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0, 0, cameraDistance);
    glRotatef(cameraAngleX, 1., 0., 0.);
    glRotatef(cameraAngleY, 0., 1., 0.);

    displayBasis();

    Sphere sphere(Vector3(0, 0, 0), 2);

    for (int x = 0; x < screenWidth; ++x)
    {
        for (int y = 0; y < screenHeight; ++y)
        {
            Vector3 origin(0,0,10);
            Vector3 direction = getRayDirection(x, y);
            Ray ray(origin, direction);

            if(sphere.intersect(ray)) {
                Vector3* intersection = sphere.intersect(ray);
                glPointSize(2.0f);
                glBegin(GL_POINTS);
                glColor3d(1.0, 0.0, 0.0);
                glVertex3d(intersection->x, intersection->y, intersection->z); 
                glEnd();
            }
        }
    }

    glPopMatrix();
    /* force result display */
    glFlush();
}

Vector3 getRayDirection(int x, int y) {
    float normalizedX = (x / (float)screenWidth - 0.5f) * 2.0f; // Convertir en coordonnées normalisées de l'espace écran
    float normalizedY = (y / (float)screenHeight - 0.5f) * 2.0f;

    float imagePlaneHeight = 2.0f * tan(fov * 0.5f * M_PI / 180.0f) * zNear; // Hauteur du plan d'image
    float imagePlaneWidth = imagePlaneHeight * aspectRatio; // Largeur du plan d'image

    Vector3 imagePoint = Vector3(
        normalizedX * imagePlaneWidth / 2.0f, 
        normalizedY * imagePlaneHeight / 2.0f,
        -zNear // La caméra regarde vers le -z
    );

    Vector3 rayDirection = imagePoint - Vector3(0, 0, 10); // Direction du rayon de la caméra au point d'image
    rayDirection.normalize(); // Normaliser la direction du rayon

    return rayDirection;
}

void initOpenGl() {
    glClearColor(.5, .5, 0.5, 0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)200 / (GLfloat)200, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0., 0., 10., 
            0., 0., 0., 
            0., 1., 0.);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '+':
        glutPostRedisplay();
        break;
    case '-':
        glutPostRedisplay();
        break;
    case 'q': /* quit program */
        exit(0);
    }
}

void mouse(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if (state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if (state == GLUT_UP)
            mouseRightDown = false;
    }

    else if (button == GLUT_MIDDLE_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseMiddleDown = true;
        }
        else if (state == GLUT_UP)
            mouseMiddleDown = false;
    }
}

void mouseMotion(int x, int y)
{
    if (mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if (mouseRightDown)
    {
        cameraDistance += (y - mouseY) * 0.2f;
        mouseY = y;
    }

    glutPostRedisplay();
}