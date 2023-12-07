from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *


def display():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()

    glBegin(GL_QUADS)
    glColor3f(1.0, 0.0, 0.0)
    glVertex2f(200, 200)
    glColor3f(0.0, 1.0, 0.0)
    glVertex2f(200, 300)
    glColor3f(0.0, 0.0, 1.0)
    glVertex2f(300, 300)
    glColor3f(1.0, 0.0, 1.0)
    glVertex2f(300, 200)
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
glutCreateWindow("hw01")

initRendering()

glutReshapeFunc(reshape, w, h)
glutDisplayFunc(display)
glutIdleFunc(idle)
glutMainLoop()