from math import sqrt

from abc import ABCMeta
from euclid import Vector3
from pyglet.gl import *
from pyglet.graphics import Batch
from solarsystem.renderer import BodyRenderer, OrbitingBodyRenderer
from util.texture import Texture

plot_steps = 1024


class Body(object, metaclass=ABCMeta):
    """
    An abstract class defining an body in the solarsystem
    """

    def __init__(self, parent, name, texturename, color, radius, axial_tilt, sidereal_rotation_period, mass, renderer=BodyRenderer()):
        """
        Creates a new body with the given parameters

        :param parent: Parent body in the system
        :type parent: :class:`Body`
        :param name: Name of the body
        :type name: str
        :param texturename: Name of the texture
        :type texturename: str
        :param color: Dictionary with r, g and b values
        :type color: dict
        :param radius: Radius of the body
        :type radius: float
        :param axial_tilt: Axial Tilt in degrees
        :type axial_tilt: float
        :param sidereal_rotation_period: Rotation period (siderial) around its own axis in days
        :type sidereal_rotation_period: float
        :param mass: Mass of the body in kilograms
        :type mass: float
        """

        self.xyz = Vector3()
        self.parent = parent
        self.name = name
        self.texturename = texturename
        self.color = color
        self.radius = radius
        self.axial_tilt = axial_tilt
        self.sidereal_rotation_period = sidereal_rotation_period
        self.mass = mass
        self.renderer = renderer

        self.timefactor = 0
        self.draw_orbit = True
        self.draw_texture = True

        self.texture = Texture(texturename)
        self.sphere = gluNewQuadric()
        gluQuadricNormals(self.sphere, GLU_SMOOTH)
        gluQuadricTexture(self.sphere, GL_TRUE)

    def post_init(self):
        """
        Calculations and stuff that should happen after everything is setup correctly
        """

        pass

    def update(self, time):
        """
        Update the body (Calculate current orbit position)

        :param time: Delta Time
        :type time: float
        """

        self.timefactor = (time % self.sidereal_rotation_period) / self.sidereal_rotation_period

    def draw(self, matrix):
        """
        Draw the body

        :param matrix: Current Model-View-Projection matrix
        :type matrix: :class:`euclid.Matrix4`
        """

        self.renderer.draw(self, matrix)

    def intersects(self, ray):
        """
        Checks if this body intersecets with the ray

        :param ray: Ray to check
        :type ray: :class:`util.ray.Ray`
        :return: True if the ray intersects this object, otherwise False:
        :rtype: bool
        """

        q = ray.direction.dot(ray.origin - self.xyz) ** 2 - (ray.origin - self.xyz).dot(ray.origin - self.xyz) + self.radius ** 2
        if q < 0:
            return False
        else:
            d = -ray.direction.dot(ray.origin - self.xyz)
            d1 = d - sqrt(q)
            d2 = d + sqrt(q)
            if 0 < d1 and (d1 < d2 or d2 < 0):
                return True
            elif 0 < d2 and (d2 < d1 or d1 < 0):
                return True
            else:
                return False


class StationaryBody(Body, metaclass=ABCMeta):
    """
    A stationary body in the solarsystem (Body without an orbit)
    """

    def __init__(self, parent, name, texturename, color, radius, axial_tilt, sidereal_rotation_period, mass, xyz=Vector3()):
        """
        Creates a new body with the given parameters

        :param parent: Parent body in the system, None if it doesn't have one
        :type parent: None, :class:`Body`
        :param name: Name of the body
        :type name: str
        :param texturename: Name of the texture
        :type texturename: str
        :param color: Dictionary with r, g and b values
        :type color: dict
        :param radius: Radius of the body
        :type radius: float
        :param axial_tilt: Axial Tilt in degrees
        :type axial_tilt: float
        :param sidereal_rotation_period: Rotation period (siderial) around its own axis
        :type sidereal_rotation_period: float
        :param xyz: Position of the object, default 0, 0, 0
        :type xyz: :class:`euclid.Vector3`
        """

        super().__init__(parent, name, texturename, color, radius, axial_tilt, sidereal_rotation_period, mass)
        self.xyz = xyz


class OrbitingBody(Body, metaclass=ABCMeta):
    """
    An orbiting body in the solarsystem
    """

    def __init__(self, parent, name, texturename, color, radius, orbit, axial_tilt, sidereal_rotation_period, mass, renderer=OrbitingBodyRenderer()):
        """
        Creates a new body with the given parameters

        :param parent: Parent body in the system
        :type parent: :class:`Body`, None
        :param name: Name of the body
        :type name: str
        :param texturename: Name of the texture
        :type texturename: str
        :param color: Dictionary with r, g and b values
        :type color: dict
        :param radius: Radius of the body
        :type radius: float
        :param orbit: Orbit of the body
        :type orbit: :class:`solarsystem.orbit.Orbit`
        :param axial_tilt: Axial Tilt in degrees
        :type axial_tilt: float
        :param sidereal_rotation_period: Rotation period (siderial) around its own axis
        :type sidereal_rotation_period: float
        """

        super().__init__(parent, name, texturename, color, radius, axial_tilt, sidereal_rotation_period, mass, renderer=renderer)
        self.orbit = orbit
        self.orbit.body = self
        self.orbit_line_batch = Batch()

    def post_init(self):
        self.orbit.post_init()

        # Plot the orbit to a pyglet batch for faster drawing
        orbit_line = []
        for pos in self.orbit.plot(plot_steps):
            orbit_line.append(pos.x)
            orbit_line.append(pos.y)
            orbit_line.append(pos.z)
        self.orbit_line_batch.add(int(len(orbit_line) / 3), GL_LINE_LOOP, None, ('v3f', tuple(orbit_line)))

    def update(self, time):
        """
        Update the body (Calculate current orbit position)

        :param time: Delta Time
        :type time: float
        """

        super().update(time)
        self.xyz = self.orbit.calculate(time)
        if self.parent:
            self.xyz += self.parent.xyz
