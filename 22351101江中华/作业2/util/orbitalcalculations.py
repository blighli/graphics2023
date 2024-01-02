# 轨道计算

import datetime
from math import cos, sin, pi, atan2, sqrt, fabs

tau = 2 * pi
gravitational_constant = 6.67408 * 10 ** -11

J2000 = datetime.datetime(year=2000, month=1, day=1, hour=12, minute=0, second=0)


class ConvergenceError(Exception):
    pass


def eccentric_anomaly_from_mean(eccentricity, mean_anomaly, precision=50, max_iterations=1000):
    """
    Convert mean anomaly to eccentric anomaly.
    Source: https://github.com/skyfielders/python-skyfield/blob/master/skyfield/keplerianlib.py

    :param e: Eccentricity
    :type e: float
    :param M: Mean anomaly in radians
    :type M: float
    :param precision: Precision of the calculation
    :type precision: int
    :param max_iterations: Max iterations
    :type max_iterations: int
    """

    # calculate the delta
    delta = 10 ** -precision

    # normalize the mean anomaly
    m = mean_anomaly % tau

    # set up the first guess
    eccentric_anomaly = tau
    if eccentricity < 0.8:
        eccentric_anomaly = m

    # do the initial test
    test = eccentric_anomaly - eccentricity * sin(m) - m

    count = 0
    while (fabs(test) > delta) and (count < max_iterations):
        # calculate the next guess for an eccentric anomaly
        eccentric_anomaly = (eccentric_anomaly - test / (1.0 - eccentricity * cos(eccentric_anomaly)))

        # try it
        test = eccentric_anomaly - eccentricity * sin(eccentric_anomaly) - m

        # count the runs, so we don't go forever
        count += 1

    # convert to degrees
    return eccentric_anomaly


def true_anomaly_from_eccentric(e, E):
    """
    Convert eccentric anomaly to true anomaly.

    :param E: Eccentric anomaly in radians
    :type E: float
    :param e: Eccentricity
    :type e: float
    """

    return 2 * atan2(sqrt(1.0 + e) * sin(E / 2.0), sqrt(1.0 - e) * cos(E / 2.0))
