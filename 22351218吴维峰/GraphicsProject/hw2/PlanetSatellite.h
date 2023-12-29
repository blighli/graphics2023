#pragma once
#include "Planet.h"
class PlanetSatellite :
    public Planet
{
public:
	void setModel1(float T,
		float rRevolution, float rSRevolution,
		float xStar, float yStar, float zStar,
		float xRotation, float yRotation, float zRotation,
		float xSRotation, float ySRotation, float zSRotation);
	void setTRevolutionTSpin(float TRevolution, float TSRevolution, float TSpin);
protected:
	float rSRevolution,
		xSRotation, ySRotation, zSRotation;
	float TSRevolution;

};

