from pyglet.gl import *
from util.texture import Texture

class SkySphere():
    """
    A SkyBox that maps the texture to a sphere
    """

    def __init__(self, filename, radius):
        """
        Creates a new SkyShpere

        :param filename: Filename of the texture
        :type filename: str
        :param radius: Radius of the skysphere
        :type radius: float
        """

        self.radius = radius
        self.texture = Texture(filename)
        self.sphere = gluNewQuadric()
        gluQuadricNormals(self.sphere, GLU_SMOOTH)
        gluQuadricTexture(self.sphere, GL_TRUE)

    def draw(self):
        """
        Draws the SkySphere
        """

        glDisable(GL_DEPTH_TEST)
        glDisable(GL_BLEND)
        glDisable(GL_CULL_FACE)
        self.texture.draw()
        gluSphere(self.sphere, self.radius, 50, 50)
