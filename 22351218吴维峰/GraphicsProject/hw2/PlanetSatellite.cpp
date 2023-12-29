#include "PlanetSatellite.h"
void PlanetSatellite::setModel1(float T,
	float rRevolution, float rSRevolution,
	float xStar, float yStar, float zStar,
	float xRotation, float yRotation, float zRotation,
	float xSRotation, float ySRotation, float zSRotation) {
    float angleRevolution, angleSpin, angleSRevolution;
    angleRevolution = 360.0 * T / TRevolution;
    angleSpin = 360.0 * T / TSpin;
    angleSRevolution = 360.0 * T / TSRevolution;

    this->xStar = xStar;
    this->yStar = yStar;
    this->zStar = zStar;

    model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(xStar, yStar, zStar));
    model = glm::rotate(model, glm::radians(angleRevolution),
        glm::normalize(glm::vec3(xRotation, yRotation, zRotation)));
    model = glm::translate(model, glm::vec3(rRevolution, 0.0, 0.0));

    model = glm::rotate(model, glm::radians(angleSRevolution),
        glm::normalize(glm::vec3(xSRotation, ySRotation, zSRotation)));
    model = glm::translate(model, glm::vec3(rSRevolution, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(angleSpin),
        glm::normalize(glm::vec3(xSRotation, ySRotation, zSRotation)));
    
}
void PlanetSatellite::setTRevolutionTSpin(
    float TRevolution, float TSRevolution, float TSpin) {
    this->TRevolution = TRevolution;
    this->TSRevolution = TSRevolution;
    this->TSpin = TSpin;
}



