#pragma once
#include "Planet.h"
#include "PlanetSatellite.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#define MAXPlanet 7
#define MAXSatellite 3
class Galaxy
{
public:
	Galaxy(int iGalaxy);
	void initGalaxy(float xStar, float yStar, float zStar);
	void setTimeStart(double timeStart);
	void setTime(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos);

	int n, nS;
	Planet P[MAXPlanet];
	PlanetSatellite S[MAXSatellite];
private:
	int numBig, iGalaxy;
	double timeStart, time0;

	float R[MAXPlanet],
		rRevolution[MAXPlanet],
		xStar, yStar, zStar,
		xRotation, yRotation, zRotation,
		xSpin, ySpin, zSpin,
		TRevolution[MAXPlanet];

	float RS[MAXSatellite],
		rSRevolution[MAXSatellite],
		xSRotation[MAXSatellite], ySRotation[MAXSatellite], zSRotation[MAXSatellite];
	int labelPlanet[MAXSatellite];
	int tex[MAXPlanet];
	int texS[MAXSatellite];
};

