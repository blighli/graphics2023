from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import glfw

if __name__ == "__main__":
    glfw.init()
    window = glfw.create_window(640, 480, "Hello World", None, None)
    glfw.make_context_current(window)
    while not glfw.window_should_close(window):
        glClear(GL_COLOR_BUFFER_BIT)
        glfw.swap_buffers(window)
        glfw.poll_events()
    glfw.terminate()
