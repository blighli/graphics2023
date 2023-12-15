from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import glfw
import math

def display():
    glClearColor(1, 1, 1, 1)
    glClear(GL_COLOR_BUFFER_BIT)
    glBegin(GL_POLYGON)
    glColor3f(1.0, 102/256, 0.0)
    glVertex2f(-0.5, -0.5)
    glVertex2f(0.5, -0.5)
    glVertex2f(0.5, 0.5)
    glVertex2f(-0.5, 0.5)
    glEnd()
    glFlush()

if __name__ == "__main__":
    glfw.init()
    window = glfw.create_window(640, 480, "Hello World", None, None)
    glfw.make_context_current(window)
    while not glfw.window_should_close(window):
        display()
        glfw.swap_buffers(window)
        glfw.poll_events()
    glfw.terminate()
