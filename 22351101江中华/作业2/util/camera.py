from math import sin, cos

from euclid import *
from util import halfpi
from util.ray import Ray

class Camera(object):
    """
    A simple camera using pitch and yaw

    :var position: Initial position
    :type position: :class:`euclid.Vector3`
    :var yaw: Initial yaw
    :type yaw: float
    :var pitch: Initial pitch
    :type pitch: float
    """

    def __init__(self, position=Vector3(), yaw=0.0, pitch=0.0):
        """
        Initializes the camera

        :param position: Initial position
        :type position: :class:`euclid.Vector3`
        :param yaw: Initial yaw
        :type yaw: float
        :param pitch: Initial pitch
        :type pitch: float
        """

        self.position = position
        self._yaw = yaw
        self._pitch = pitch

    def forwards(self, distance):
        """
        Moves the camera forwards by the given distance

        :param distance: Distance to move
        :type distance: float
        """

        self.position.x -= distance * sin(self._yaw)
        self.position.z -= distance * cos(self._yaw)

    def backwards(self, distance):
        """
        Moves the camera backwards by the given distance

        :param distance: Distance to move
        :type distance: float
        """

        self.position.x += distance * sin(self._yaw)
        self.position.z += distance * cos(self._yaw)

    def left(self, distance):
        """
        Moves the camera left by the given distance

        :param distance: Distance to move
        :type distance: float
        """

        self.position.x += distance * sin(self._yaw - 90)
        self.position.z += distance * cos(self._yaw - 90)

    def right(self, distance):
        """
        Moves the camera right by the given distance

        :param distance: Distance to move
        :type distance: float
        """

        self.position.x += distance * sin(self._yaw + 90)
        self.position.z += distance * cos(self._yaw + 90)

    def up(self, distance):
        """
        Moves the camera up by the given distance

        :param distance: Distance to move
        :type distance: float
        """

        self.position.y += distance

    def down(self, distance):
        """
        Moves the camera down by the given distance

        :param distance: Distance to move
        :type distance: float
        """

        self.position.y -= distance

    def pitch(self, angle):
        """
        Adds the given pitch to the camera

        :param angle: Angle to move
        :type distance: float
        """

        self._pitch += angle

        if self._pitch > halfpi:
            self._pitch = halfpi
        if self._pitch < -halfpi:
            self._pitch = -halfpi

    def yaw(self, angle):
        """
        Adds the given yaw to the camera

        :param angle: Angle to move
        :type distance: float
        """

        self._yaw += angle

    def view_matrix(self):
        """
        Calculates the view matrix

        :return: Calculated view matrix
        :rtype: :class:`euclid.Matrix4`
        """

        matrix = Matrix4()
        matrix.rotate_axis(-self._pitch, Vector3(1, 0, 0))
        matrix.rotate_axis(-self._yaw, Vector3(0, 1, 0))
        matrix.translate(-self.position.x, -self.position.y, -self.position.z)
        return matrix

    def create_ray(self):
        mat = self.view_matrix()
        return Ray(self.position, -Vector3(mat[2], mat[6], mat[10]).normalize())
