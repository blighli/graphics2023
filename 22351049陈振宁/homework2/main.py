import glfw
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from PIL import Image

def init():
    glEnable(GL_TEXTURE_2D)
    glEnable(GL_DEPTH_TEST)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glShadeModel(GL_SMOOTH)
    # Sunlight source
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightfv(GL_LIGHT0, GL_POSITION, [0.0, 1.0, 1.0, 0.0])
    glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
    glMatrixMode(GL_PROJECTION)
    gluPerspective(45, 1.33, 2, 100)
    glMatrixMode(GL_MODELVIEW)

# 纹理加载函数
def loadTexture(imageName):
    im = Image.open(imageName)
    try:
        ix, iy, image = im.size[0], im.size[1], im.tobytes("raw", "RGBX", 0, -1)
    except SystemError:
        ix, iy, image = im.size[0], im.size[1], im.tobytes("raw", "RGB", 0, -1)
    # create texture
    ID = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, ID)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, image)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    return ID


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

# initialize GLFW
if not glfw.init():
    raise Exception("GLFW can't initialize")

# create window
window = glfw.create_window(800, 600, "OpenGL Sphere with Texture and Light", None, None)
if not window:
    glfw.terminate()
    raise Exception("GLFW window can't be created")

glfw.make_context_current(window)

# initialize OpenGL environment
init()
texture = loadTexture("sand.jpg")
#texture = loadTexture("texture1.jpg")
#texture = loadTexture("texture2.jpg")

# main cycle
while not glfw.window_should_close(window):
    glfw.poll_events()
    draw()

glfw.terminate()