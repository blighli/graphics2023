# -*- coding: utf-8 -*-
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import numpy as np
from PIL import Image as Image


def read_texture(filename):
    img = Image.open(filename)
    img_data = np.flipud(np.array(img, np.int8))
    textID = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, textID)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.size[0], img.size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, img_data)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    return textID


def getposture():
    dist = np.sqrt(np.power((EYE - LOOK_AT), 2).sum())
    if dist > 0:
        phi = np.arcsin((EYE[2] - LOOK_AT[2]) / dist)
        theta = np.arctan2(EYE[1] - LOOK_AT[1], EYE[0] - LOOK_AT[0])
    else:
        phi = 0.0
        theta = 0.0
    return dist, phi, theta


def draw():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    if WIN_W > WIN_H:
        if IS_PERSPECTIVE:
            glFrustum(VIEW[0] * WIN_W / WIN_H, VIEW[1] * WIN_W / WIN_H, VIEW[2], VIEW[3], VIEW[4], VIEW[5])
        else:
            glOrtho(VIEW[0] * WIN_W / WIN_H, VIEW[1] * WIN_W / WIN_H, VIEW[2], VIEW[3], VIEW[4], VIEW[5])
    else:
        if IS_PERSPECTIVE:
            glFrustum(VIEW[0], VIEW[1], VIEW[2] * WIN_H / WIN_W, VIEW[3] * WIN_H / WIN_W, VIEW[4], VIEW[5])
        else:
            glOrtho(VIEW[0], VIEW[1], VIEW[2] * WIN_H / WIN_W, VIEW[3] * WIN_H / WIN_W, VIEW[4], VIEW[5])

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    gluLookAt(
        EYE[0], EYE[1], EYE[2],
        LOOK_AT[0], LOOK_AT[1], LOOK_AT[2],
        EYE_UP[0], EYE_UP[1], EYE_UP[2]
    )

    glViewport(0, 0, WIN_W, WIN_H)

    tex = read_texture('2k_earth_daymap.jpg')
    qobj = gluNewQuadric()
    gluQuadricTexture(qobj, GL_TRUE)
    glEnable(GL_TEXTURE_2D)
    glBindTexture(GL_TEXTURE_2D, tex)
    gluSphere(qobj, 1, 32, 32)
    glDisable(GL_TEXTURE_2D)
    gluDeleteQuadric(qobj)

    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightfv(GL_LIGHT0, GL_POSITION, (1, 1, 1, 0))
    glLightfv(GL_LIGHT0, GL_AMBIENT, (0.4, 0.4, 0.4, 1.0))
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (1.2, 1.2, 1.2, 1.0))
    glLightfv(GL_LIGHT0, GL_SPECULAR, (1.5, 1.5, 1.5, 1.0))

    glutSwapBuffers()
    glutPostRedisplay()


MOUSE_X, MOUSE_Y = 0, 0
LEFT_IS_DOWNED = False


def mouseclick(button, state, x, y):
    global MOUSE_X, MOUSE_Y, LEFT_IS_DOWNED
    MOUSE_X, MOUSE_Y = x, y
    if button == GLUT_LEFT_BUTTON:
        LEFT_IS_DOWNED = state == GLUT_DOWN
        glutPostRedisplay()


def mousemotion(x, y):
    global MOUSE_X, MOUSE_Y, LEFT_IS_DOWNED
    DIST, PHI, THETA = getposture()
    if LEFT_IS_DOWNED:
        dx = x - MOUSE_X
        dy = y - MOUSE_Y
        MOUSE_X, MOUSE_Y = x, y

        PHI += 2 * np.pi * dy / WIN_H
        PHI = min(np.pi * 0.499, PHI)
        PHI = max(np.pi * -0.499, PHI)
        THETA -= 2 * np.pi * dx / WIN_W
        THETA %= 2 * np.pi
        r = DIST * np.cos(PHI)

        EYE[2] = DIST * np.sin(PHI)
        EYE[1] = r * np.sin(THETA)
        EYE[0] = r * np.cos(THETA)

        glutPostRedisplay()


if __name__ == "__main__":
    IS_PERSPECTIVE = True
    VIEW = np.array([-0.8, 0.8, -0.8, 0.8, 1.0, 20.0])
    EYE = np.array([2.0, 2.0, 2.0])
    LOOK_AT = np.array([0.0, 0.0, 0.0])
    EYE_UP = np.array([0.0, 0.0, 1.0])
    WIN_W, WIN_H = 640, 480

    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH)

    glutInitWindowSize(WIN_W, WIN_H)
    glutInitWindowPosition(300, 200)
    glutCreateWindow('hw2')

    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)

    glutDisplayFunc(draw)
    glutMouseFunc(mouseclick)
    glutMotionFunc(mousemotion)

    glutMainLoop()
