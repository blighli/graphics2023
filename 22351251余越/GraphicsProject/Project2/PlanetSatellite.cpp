#include "PlanetSatellite.h"
void PlanetSatellite::setModel1(float T,//已过的时间,星球半径
	float rRevolution, float rSRevolution,//所绕星公转半径,卫星公转半径
	float xStar, float yStar, float zStar,//公转所绕恒星坐标
	float xRotation, float yRotation, float zRotation,//公转所绕恒星转轴
	float xSRotation, float ySRotation, float zSRotation) {//卫星转轴
    float angleRevolution, angleSpin, angleSRevolution;
    angleRevolution = 360.0 * T / TRevolution;
    angleSpin = 360.0 * T / TSpin;
    angleSRevolution = 360.0 * T / TSRevolution;

    this->xStar = xStar;
    this->yStar = yStar;
    this->zStar = zStar;

    model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(xStar, yStar, zStar));//恒星坐标
    model = glm::rotate(model, glm::radians(angleRevolution),
        glm::normalize(glm::vec3(xRotation, yRotation, zRotation)));//所绕行星公转
    model = glm::translate(model, glm::vec3(rRevolution, 0.0, 0.0));

    model = glm::rotate(model, glm::radians(angleSRevolution),
        glm::normalize(glm::vec3(xSRotation, ySRotation, zSRotation)));//卫星公转
    model = glm::translate(model, glm::vec3(rSRevolution, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(angleSpin),//卫星自转
        glm::normalize(glm::vec3(xSRotation, ySRotation, zSRotation)));
    //model = glm::scale(model, glm::vec3(R, R, R));
}
void PlanetSatellite::setTRevolutionTSpin(
    float TRevolution, float TSRevolution, float TSpin) {
    this->TRevolution = TRevolution;
    this->TSRevolution = TSRevolution;
    this->TSpin = TSpin;
}



