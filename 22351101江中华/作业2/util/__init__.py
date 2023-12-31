import ctypes
import time as time_
from math import pi

import pyglet
from pyglet.gl import GLdouble
from pyglet.text import decode_html

lightfv = ctypes.c_float * 4
dts = 24 * 60 * 60
halfpi = pi / 2.0


def load_string(filename):
    """
    Loads the given file with the pyglet resource loader

    :param filename: name of the file that should be loaded
    :return: content of the file
    """

    with pyglet.resource.file(filename) as file:
        string = file.read().decode("utf-8")
    return string


def load_html(filename):
    """
    Loads the HTML-file with the pyglet resource loader

    :param filename: name of the file that should be loaded
    :return: content of the file
    """

    return decode_html(load_string(filename))


def auto_str(cls):
    """
    Adds an automatically generated __str__ method to the class that is annotated with it.

    :param cls: Class to annotate
    :return: Class that was annotated
    """

    def __str__(self):
        return '%s(%s)' % (
            type(self).__name__,
            ', '.join('%s=%s' % item for item in vars(self).items())
        )

    cls.__str__ = __str__
    return cls


def millis():
    """
    Get the current time in milliseconds since epoch

    :return: Current time in milliseconds since epoch
    :rtype: int
    """

    return int(round(time_.time() * 1000))


def toGlMatrix(matrix4):
    """
    Converts the given matrix to be used by OpenGL

    :param matrix4: Matrix to convert
    :type matrix4: :class:`euclid.Matrix4`
    :return: Matrix to be used by OpenGL
    :rtype: No Idea
    """
    return (GLdouble * 16)(*matrix4)
