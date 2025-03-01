#include "draw.h"
#include <GL/glew.h>

void draw_triangle(void) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0); glVertex2f(-0.6, -0.75);
    glColor3f(0, 1, 0); glVertex2f(0.6, -0.75);
    glColor3f(0, 0, 1); glVertex2f(0, 0.75);
    glEnd();
}