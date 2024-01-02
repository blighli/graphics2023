#pragma once
#include "Planet.h"
class PlanetSatellite :
    public Planet
{
public:
	void setModel1(float T,//已过的时间
		float rRevolution, float rSRevolution,//所绕星公转半径,卫星公转半径
		float xStar, float yStar, float zStar,//公转所绕恒星坐标
		float xRotation, float yRotation, float zRotation,//公转所绕恒星转轴
		float xSRotation, float ySRotation, float zSRotation);//自转轴
	void setTRevolutionTSpin(float TRevolution, float TSRevolution, float TSpin);//公转自转周期
protected:
	float rSRevolution,//卫星公转半径
		xSRotation, ySRotation, zSRotation;//卫星公转自转转轴
	float TSRevolution;//卫星公转周期

};

