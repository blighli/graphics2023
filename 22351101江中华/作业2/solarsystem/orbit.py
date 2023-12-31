from math import sin, cos, radians, sqrt, pi

from abc import ABCMeta, abstractmethod
from euclid import Vector3
# from util import auto_str
from util.orbitalcalculations import gravitational_constant, true_anomaly_from_eccentric, eccentric_anomaly_from_mean


class Orbit(object, metaclass=ABCMeta):
    """
    An abstract class defining the orbit of a body

    :var body: Body that this orbit belongs to
    :type body: :class:`solarsystem.body.Body`
    :var multiplier: Position multiplier
    :type multiplier: float
    """

    def __init__(self, body=None, multiplier=1):
        """
        Creates a new Orbit

        :param body: Body that this orbit belongs to
        :type body: :class:`solarsystem.body.Body`
        :param multiplier: Position multiplier
        :type multiplier: float
        """

        self.body = body
        self.multiplier = multiplier

    def post_init(self):
        """
        Calculations and stuff that should happen after everything is setup correctly
        """

        pass

    @abstractmethod
    def calculate(self, time):
        """
        Calculate current position of body with the current time and return it

        :param time: Delta Time
        :type time: float
        :return: position
        :rtype: :class:`euclid.Vector3`
        """

        pass

    def plot(self, steps):
        """
        Generator to calculate the position in orbit

        :param steps: Steps to calculate
        :return: int
        """

        step = self.orbital_period / steps
        for i in range(0, steps):
            yield self.calculate(i * step)


class CircularOrbit(Orbit):
    """
    An Orbit subclass defining a circular orbit

    :var radius: Radius of the orbit
    :type radius: float
    :var orbital_period: Time to complete one orbit in seconds
    :type orbital_period: float
    :var inclination: Inlincation to ecliptic in radians
    :type inclination: float
    :param multiplier: Position multiplier
    :type multiplier: float
    """

    def __init__(self, radius, orbital_period, inclination=0, multiplier=1):
        """
        Creates a circular orbit with the given radius, orbital_period (siderial) and inclination

        :param radius: Radois of the orbit
        :type radius: float
        :param orbital_period: Duration of one orbit in days
        :type orbital_period: float
        :param inclination: Inclination of orbit in radians
        :type: inclination: float
        """

        super().__init__(multiplier=multiplier)
        self.radius = radius
        self.orbital_period = orbital_period
        self.inclination = inclination

    def calculate(self, time):
        angle = radians(360 * ((time % self.orbital_period) / self.orbital_period))
        x = self.radius * cos(angle)
        z = self.radius * sin(angle)
        pos = Vector3(x, 0, z)
        if self.inclination != 0:
            pos = pos.rotate_around(Vector3(0, 0, 1), self.inclination)
        return pos


class EllipticOrbit(Orbit):
    """
    An elliptical orbit calculated from orbital elements

    Sources:
    http://astronomy.stackexchange.com/questions/12716/simulate-an-orbit-with-orbital-elements
    http://www.orbiter-forum.com/showthread.php?t=26682
    https://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf
    https://www.physicsforums.com/threads/calculating-elliptic-orbits-in-cartesian-coordinates.712979/

    :var apoapsis: Apoapsis in meters
    :type apoapsis: float
    :var periapsis: Periapsis in meters
    :type periapsis: float
    :var longtitude_ascending_node: Longtitude of the ascending node in radians
    :type longtitude_ascending_node: float
    :var argument_of_periapsis: Argument of the periapsis in radians
    :type argument_of_periapsis: float
    :var initial_mean_anomaly: Initial mean anomaly at J.2000 in radians
    :type initial_mean_anomaly: float
    :var inclination: Inclination of the orbit in radians
    :type inclination: float
    :var semi_major_axis: Semi-mahor axis calculated from the provided apoapsis and periapsis
    :type semi_major_axis: float
    :var eccentricity: Eccentricity calculated from the provided apoapsis and periapsis
    :type eccentricity: float
    :var orbital_period: Orbital period calculated from the semi-major axis and mass in seconds
    :type orbital_period: float
    :var mean_motion: Mean motion in radians per second
    :type mean_motion: float
    :param multiplier: Position multiplier
    :type multiplier: float
    """

    def __init__(self, apoapsis, periapsis, longtitude_ascending_node, argument_of_periapsis, inclination, initial_mean_anomaly=0, multiplier=1):
        """
        Creates a new elliptical orbit from the given parameters

        :param apoapsis: Apoapsis in meters
        :type apoapsis: float
        :param periapsis: Periapsis in meters
        :type periapsis: float
        :param longtitude_ascending_node: Longtitude of the ascending node in radians
        :type longtitude_ascending_node: float
        :param argument_of_periapsis: Argument of the periapsis in radians
        :type argument_of_periapsis: float
        :param initial_mean_anomaly: Initial mean anomaly at J.2000 in radians
        :type initial_mean_anomaly: float
        :param inclination: Inclination of the orbit in radians
        :type inclination: float
        """
        super().__init__(multiplier=multiplier)

        self.apoapsis = apoapsis
        self.periapsis = periapsis
        self.longtitude_ascending_node = longtitude_ascending_node
        self.argument_of_periapsis = argument_of_periapsis
        self.initial_mean_anomaly = initial_mean_anomaly
        self.inclination = inclination

    def post_init(self):
        self.semi_major_axis = (self.apoapsis + self.periapsis) / 2.0
        self.eccentricity = (self.apoapsis - self.periapsis) / (self.apoapsis + self.periapsis)
        self.orbital_period = 2.0 * pi * sqrt((self.semi_major_axis ** 3.0) / (gravitational_constant * (5.97237 * 10 ** 24 + 1.9884 * 10 ** 30)))
        self.mean_motion = 2.0 * pi / self.orbital_period

    def calculate(self, time):
        mean_anomaly = self.initial_mean_anomaly
        mean_anomaly += self.mean_motion * time
        true_anomaly = true_anomaly_from_eccentric(self.eccentricity, eccentric_anomaly_from_mean(self.eccentricity, mean_anomaly))
        true_anomaly = abs(true_anomaly)
        radius = self.semi_major_axis * (1.0 - self.eccentricity ** 2.0) / (1.0 + self.eccentricity * cos(true_anomaly))
        x = radius * (cos(self.longtitude_ascending_node) * cos(true_anomaly + self.argument_of_periapsis) - sin(self.longtitude_ascending_node) * sin(true_anomaly + self.argument_of_periapsis) * cos(self.inclination))
        z = radius * (sin(self.longtitude_ascending_node) * cos(true_anomaly + self.argument_of_periapsis) + cos(self.longtitude_ascending_node) * sin(true_anomaly + self.argument_of_periapsis) * cos(self.inclination))
        y = radius * sin(true_anomaly + self.argument_of_periapsis) * sin(self.inclination)
        pos = Vector3(x, y, z)
        pos *= self.multiplier
        return pos
