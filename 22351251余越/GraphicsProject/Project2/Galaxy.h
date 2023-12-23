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
	Planet P[MAXPlanet];//行星对象
	PlanetSatellite S[MAXSatellite];//卫星对象
private:
	int numBig, iGalaxy;
	double timeStart, time0;

	float R[MAXPlanet],//各行星半径
		rRevolution[MAXPlanet],//各行星公转半径
		xStar, yStar, zStar,//公转所绕恒星坐标
		xRotation, yRotation, zRotation,//公转轴
		xSpin, ySpin, zSpin,//自转轴
		TRevolution[MAXPlanet];//公转周期

	float RS[MAXSatellite],//各卫星半径
		rSRevolution[MAXSatellite],//各卫星公转半径
		xSRotation[MAXSatellite], ySRotation[MAXSatellite], zSRotation[MAXSatellite];//转轴
	int labelPlanet[MAXSatellite];//公转所绕星标号

	int tex[MAXPlanet];//各行星纹理标号
	int texS[MAXSatellite];//各卫星纹理标号
};

