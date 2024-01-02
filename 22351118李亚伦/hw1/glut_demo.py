from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


def display():
    glClear(GL_COLOR_BUFFER_BIT)
    glBegin(GL_POLYGON)
    glVertex2f(-0.5, -0.5)
    glVertex2f(0.5, -0.5)
    glVertex2f(0.5, 0.5)
    glVertex2f(-0.5, 0.5)
    glEnd()
    glFlush()


# 主函数
if __name__ == "__main__":
    glutInit()
    glutCreateWindow("Glut Demo")
    glutDisplayFunc(display)
    glutMainLoop()
