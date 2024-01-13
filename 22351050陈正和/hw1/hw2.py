import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
from PIL import Image


def textureLoader(imagePath):
    img = Image.open(imagePath)
    try:
        imgData = img.tobytes("raw", "RGBA", 0, -1)
    except SystemError:
        imgData = img.tobytes("raw", "RGB", 0, -1)
    imgWidth, imgHeight = img.size

    texID = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texID)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData)

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    return texID


def setupGL():
    glEnable(GL_TEXTURE_2D)
    glEnable(GL_DEPTH_TEST)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glShadeModel(GL_SMOOTH)
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightfv(GL_LIGHT0, GL_POSITION, [1.0, 1.0, 1.0, 0.0])
    glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])

    glMatrixMode(GL_PROJECTION)
    gluPerspective(45, 1.33, 2, 100)
    glMatrixMode(GL_MODELVIEW)


def renderSphere():
    sphere = gluNewQuadric()
    gluQuadricTexture(sphere, GL_TRUE)
    gluSphere(sphere, 1, 32, 32)
    gluDeleteQuadric(sphere)


def drawScene():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glBindTexture(GL_TEXTURE_2D, tex)
    glLoadIdentity()
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0)
    renderSphere()
    glfw.swap_buffers(win)


if not glfw.init():
    raise Exception("Failed to initialize GLFW")

win = glfw.create_window(800, 600, "Textured Sphere", None, None)
if not win:
    glfw.terminate()
    raise Exception("Failed to create GLFW window")

glfw.make_context_current(win)
setupGL()
tex = textureLoader("./R.jpg")

while not glfw.window_should_close(win):
    glfw.poll_events()
    drawScene()

glfw.terminate()
