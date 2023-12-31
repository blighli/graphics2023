import math

from OpenGL.GL import *  
from OpenGL.GLUT import *  
from OpenGL.GLU import *  

import pyglet
from controls import Controls
from euclid import *
from gui import GUI
from pyglet.gl import *
from solarsystem.loader import load_bodies
from util import toGlMatrix
from util.camera import Camera, halfpi
from util.skybox import SkySphere

# Register resource locations in pyglet resource loader
pyglet.resource.path = ['resource/texture', 'resource/text']
pyglet.resource.reindex()

# Configure and setup window
config = pyglet.gl.Config(sample_buffers=1, samples=8, depth_size=24)
window = pyglet.window.Window(1280, 720, config=config, caption='Solarsystem', resizable=True, vsync=False)

fullscreen = False
draw_skybox = True

# looad the bodies from the json files
bodies = load_bodies("bodies")

# Create a new camera
camera = Camera(position=Vector3(0, 420, 0), pitch=-halfpi)

# Create model and projection matrices
model_matrix = Matrix4()
proj_matrix = None
mvp = Matrix4()

# time
timestep = 0
solarsystem_time = 0
time = solarsystem_time

# create the skyshpere
skybox = SkySphere("milkyway.jpg", 5500)


def toggle_draw_orbits():
    """
    Toggles the plotting of the orbits
    """

    for cur in bodies:
        cur.draw_orbit = not cur.draw_orbit


def toggle_draw_textures():
    """
    Toggles the drawing of the textures
    """

    global draw_skybox
    draw_skybox = not draw_skybox
    for cur in bodies:
        cur.draw_texture = not cur.draw_texture


def toggle_fullscreen(override):
    global fullscreen
    if override is not None:
        fullscreen = override
    else:
        fullscreen = not fullscreen
    window.set_fullscreen(fullscreen)

def toggle_light():
    if glIsEnabled(GL_LIGHT1):
        glDisable(GL_LIGHT1)  # 关闭光源1  
        glDisable(GL_LIGHTING)
        glDisable(GL_DEPTH_TEST)
        glDisable(GL_COLOR_MATERIAL)
    else:
        glEnable(GL_LIGHT1)  # 启用光源1  
        glEnable(GL_LIGHTING)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_COLOR_MATERIAL)
        # glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE )


controls = Controls(window, camera, bodies, callbacks={'toggle_draw_orbits': toggle_draw_orbits,
                                                       'toggle_draw_textures': toggle_draw_textures,
                                                       'toggle_fullscreen': toggle_fullscreen,
                                                       'toggle_light': toggle_light})

gui = GUI(window, controls, bodies)


@window.event
def on_resize(width, height):
    """
    Capture pyglet resize event

    :param width: New width
    :param height: New height
    """
    global proj_matrix
    # recalculate projection matrix
    proj_matrix = Matrix4.new_perspective(45, float(width) / float(height), 0.1, 64000.0)
    # set new viewport
    glViewport(0, 0, width, height)
    glMatrixMode(GL_MODELVIEW)

    # glMatrixMode(GL_PROJECTION)
    # glLoadIdentity()
    # gluPerspective(45.0, width / float(height), 0.1, 100.0)
    # glMatrixMode(GL_MODELVIEW)
    # glLoadIdentity()
    # return pyglet.event.EVENT_HANDLED


@window.event
def on_draw():
    """
    Redraw the screen
    """

    # reset window and set all needed opengl flags
    window.clear()
    glPushAttrib(GL_ENABLE_BIT)
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_BLEND)
    glEnable(GL_CULL_FACE)
    glCullFace(GL_BACK)
    glEnable(GL_COLOR_MATERIAL)
    glShadeModel(GL_SMOOTH)

    # draw skybox if requested
    if draw_skybox:
        glPushAttrib(GL_ENABLE_BIT)
        skybox_matrix = mvp.__copy__()
        skybox_matrix.translate(camera.position.x, camera.position.y, camera.position.z)
        skybox_matrix.rotate_axis(math.radians(-90), Vector3(1, 0, 0))
        glLoadMatrixd(toGlMatrix(skybox_matrix))
        skybox.draw()
        glPopAttrib()

    # loop through bodies and draw
    for planet in bodies:
        glPushAttrib(GL_ENABLE_BIT)
        planet.draw(mvp.__copy__())
        glPopAttrib()

    glPopAttrib()

    # ====== START GUI ======
    # create an orthographic projection (2d)
    glPushAttrib(GL_ENABLE_BIT)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glOrtho(0, window.width, 0, window.height, -1, 1)

    gui.draw()

    # glColor3f(1, 1, 0)
    glLineWidth(1.0)
    glBegin(GL_LINES)
    cross_len = 10
    glVertex2f(window.width / 2 - cross_len, window.height / 2)
    glVertex2f(window.width / 2 + cross_len, window.height / 2)
    glVertex2f(window.width / 2, window.height / 2 - cross_len)
    glVertex2f(window.width / 2, window.height / 2 + cross_len)
    glEnd()
    # glColor3f(1, 1, 1)

    glPopAttrib()
    # ====== STOP GUI ======


def update(dt):
    """
    Update time, Recalculate orbital positions

    :param dt: Time since last update
    """
    global solarsystem_time
    global mvp
    global time

    # recalculate solarsystem time
    timestep = 60 * 60 * 24 * 7 * controls.time_multiplier
    solarsystem_time += dt * timestep
    time += dt
    gui.update_time(timestep, solarsystem_time)

    if not controls.toggled_help_label and time >= 10:
        controls.draw_help_label = False

    # update the controls
    controls.update(dt)
    # recalculate mvp matrix
    mvp = proj_matrix * camera.view_matrix()

    # update every bodies
    for planet in bodies:
        planet.update(solarsystem_time)


# starts the application
pyglet.clock.schedule(update)



# 定义光源属性  
glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat * 4)(0, 0, 0, 1))  # 设置光源位置
glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat * 4)(0, 0, 0, 1))  # 设置光源环境光颜色  
glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat * 4)(0.8, 0.8, 0.8, 1))  # 设置光源散射光颜色  
glLightfv(GL_LIGHT1, GL_SPECULAR, (GLfloat * 4)(1, 1, 1, 1))  # 设置光源镜面光颜色  

# glEnable(GL_LIGHT1)  # 启用光源1  
# glEnable(GL_LIGHTING)
# glEnable(GL_DEPTH_TEST)
# glEnable(GL_COLOR_MATERIAL)

pyglet.app.run()
