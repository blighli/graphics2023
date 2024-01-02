import pyglet
from pyglet.gl import *

class Texture(object):
    """
    A wrapper for the pyglet Image for easier use
    """

    def __init__(self, filename, mipmaps=False):
        """
        Loads and uploads the given texture to the GPU

        :param filename: Filename of the texture (Loads from pyglet resource loader)
        :type filename: str
        :param mipmaps: Enable Mipmaps (default False)
        :type mipmaps: bool
        """
        print("Loading Texture " + filename)

        self.mipmaps = mipmaps
        self.filename = filename

        self.image = pyglet.resource.image(self.filename)
        self.texture = self.image.texture
        self._verify('width')
        self._verify('height')

        if self.mipmaps:
            glGenerateMipmap(self.texture.target)

    def draw(self):
        """
        Enable and bind the texture
        """

        glEnable(self.texture.target)
        glBindTexture(self.texture.target, self.texture.id)
        if self.mipmaps:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)

    def _verify(self, dimension):
        """
        Verifies the size of the texture. Texture needs to be to the power of 2

        :param dimension: Which dimension to check (width, height)
        :type dimension: str
        :raise Exception: Image is not a power of two
        """
        value = self.texture.__getattribute__(dimension)
        while value > 1:
            div_float = float(value) / 2.0
            div_int = int(div_float)
            if not (div_float == div_int):
                raise Exception('image %s is %d, which is not a power of 2' % (
                    dimension, self.texture.__getattribute__(dimension)))
            value = div_int
