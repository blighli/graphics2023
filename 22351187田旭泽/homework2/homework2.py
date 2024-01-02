import glfw
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from PIL import Image
import numpy as np

# 纹理加载函数
def loadTexture(imageName):
    im = Image.open(imageName)
    try:
        ix, iy, image = im.size[0], im.size[1], im.tobytes("raw", "RGBX", 0, -1)
    except SystemError:
        ix, iy, image = im.size[0], im.size[1], im.tobytes("raw", "RGB", 0, -1)
    # 创建纹理
    ID = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, ID)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, image)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    return ID

def init():
    glEnable(GL_TEXTURE_2D)
    glEnable(GL_DEPTH_TEST)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glShadeModel(GL_SMOOTH)
    # 太阳光源
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightfv(GL_LIGHT0, GL_POSITION, [0.0, 1.0, 1.0, 0.0])
    glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
    glMatrixMode(GL_PROJECTION)
    gluPerspective(45, 1.33, 2, 100)
    glMatrixMode(GL_MODELVIEW)

def draw_sphere():
    qobj = gluNewQuadric()
    gluQuadricTexture(qobj, GL_TRUE)
    gluSphere(qobj, 1, 32, 32)
    gluDeleteQuadric(qobj)

def draw():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glBindTexture(GL_TEXTURE_2D, texture)
    glLoadIdentity()
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0)
    draw_sphere()
    glfw.swap_buffers(window)

# 初始化 GLFW
if not glfw.init():
    raise Exception("GLFW 无法初始化")

# 创建窗口
window = glfw.create_window(800, 600, "OpenGL Sphere with Texture and Light", None, None)
if not window:
    glfw.terminate()
    raise Exception("GLFW 窗口无法创建")

glfw.make_context_current(window)

# 初始化 OpenGL 环境
init()
texture = loadTexture("graphics2023/22351187田旭泽/homework2/earth1.jpg")

# 主循环
while not glfw.window_should_close(window):
    glfw.poll_events()
    draw()

glfw.terminate()