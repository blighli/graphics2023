from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import math

def display():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()

    glBegin(GL_TRIANGLES)
    glColor3f(1.0, 0.0, 0.0)  # 红色

    glVertex2f(250, 400)  # 顶点1
    glVertex2f(150, 200)  # 顶点2
    glVertex2f(350, 200)  # 顶点3

    glEnd()

    glutSwapBuffers()


def idle():
    display()


def initRendering():
    glEnable(GL_DEPTH_TEST)
    # glEnable(GL_LIGHTING)
    # glEnable(GL_LIGHT0)


def reshape(w, h):
    glViewport(0, 0, w, h)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, w, 0.0, h, 0.0, 1.0)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()


w, h = 500, 500
u, v = 100, 100

glutInit()
glutInitDisplayMode(GLUT_RGB |
                    GLUT_DOUBLE |
                    GLUT_DEPTH)

glutInitWindowSize(w, h)
glutInitWindowPosition(u, v)
glutCreateWindow("red triangle")

initRendering()

glutReshapeFunc(reshape, w, h)
glutDisplayFunc(display)
glutIdleFunc(idle)
glutMainLoop()