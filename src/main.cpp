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
#include "light.h"

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
    LightType AMBIANT = AMBIENT; LightType DIRECTIONAL = DIRECTIONAL;
    Light ambientLight(AMBIENT, Vector3(8, 5, 2), Vector3(.1,.1,1)*0.5);
    Light diffuseLight(DIRECTIONAL, Vector3(8, 5, 2), Vector3(.0,1,.0)*0.8);


    Material material(0.2, 0.3);
    Sphere sphere(Vector3(0, 0, 0), 2, material);

    for (int y = 0; y < screenHeight; ++y)
    {
        for (int x = 0; x < screenWidth; ++x)
        {
            Vector3 origin(0,0,10);
            Vector3 direction = getRayDirection(x, y);
            Ray ray(origin, direction);

            if(sphere.intersect(ray) != nullptr) {
                Vector3* intersection = sphere.intersect(ray);
                Vector3 finalColor(0,0,0);

                if(ambientLight.type == AMBIENT) {
                    finalColor += ambientLight.intensity * sphere.material.Ka;
                }

                if(diffuseLight.type == POINT) {
                    Vector3 Vi = (diffuseLight.position - *intersection).normalize();
                    Vector3 N = sphere.normalAt(intersection);
                    double cosTheta = std::max(N.dot(Vi), 0.0f);
                    Vector3 Id = diffuseLight.intensity * sphere.material.Kd * cosTheta;
                    finalColor += Id;
                }

                glPointSize(2.0f);
                glBegin(GL_POINTS);
                glColor3d(finalColor.x, finalColor.y, finalColor.z);
                glVertex3d(intersection->x, intersection->y, intersection->z); 
                glEnd();

                delete intersection;
            }
        }
    }

    glPopMatrix();
    glFlush();
}

Vector3 getRayDirection(int x, int y) {
    // Convertir les angles de degrés en radians pour les fonctions trigonométriques
    float radFov = fov * M_PI / 180.0;

    // Calcul de la hauteur et de la largeur de la vue
    float viewHeight = 2.0 * tan(radFov / 2.0);
    float viewWidth = aspectRatio * viewHeight;

    // Normalisation des coordonnées x et y
    float normalizedX = (x + 0.5) / screenWidth * viewWidth - viewWidth / 2.0;
    float normalizedY = -(y + 0.5) / screenHeight * viewHeight + viewHeight / 2.0;

    // Création de la direction du rayon et normalisation
    Vector3 rayDirection(normalizedX, normalizedY, -1.0);
    return rayDirection.normalize(); // Assurez-vous que la méthode normalize() ne modifie pas l'état de l'objet rayDirection
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