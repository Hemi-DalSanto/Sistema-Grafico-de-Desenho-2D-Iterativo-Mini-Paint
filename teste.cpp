#include <GL/glut.h>
#include "Framebuffer.h"

Framebuffer fb(800, 600, {255, 255, 255});

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(fb.get_width(), fb.get_height(), GL_RGB, GL_UNSIGNED_BYTE, fb.get_data());
    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'n' || key == 'N') {
        fb.clear_canvas();
        glutPostRedisplay();
    }
    if (key == 's' || key == 'S') {
        fb.export_ppm("desenho.ppm");
    }
}

int main(int argc, char** argv) {
    fb.put_pixel(400, 300, {255, 0, 0});

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(fb.get_width(), fb.get_height());
    glutCreateWindow("Mini Paint 2D - Motor do Framebuffer");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
