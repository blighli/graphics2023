/*#include "IntersectEvents.h"
#include <stdio.h>


void CastRay(int x, int y)
{
	GLint    viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetIntegerv(GL_VIEWPORT, viewport); // 得到的是最后一个设置视口的参数
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	int winX = x;
	int winY = 600 - y;
	GLdouble posX, posY, posZ;
	//获取像素对应的前裁剪面的点坐标
	bool bResult = gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ);
	FPoint3 nearPoint;
	nearPoint.x = posX; nearPoint.y = posY; nearPoint.z = posZ;
	glm::vec3 O(0.0,0.0,5.0);
	glm::vec3 D(posX-O.x,posY-O.y,posZ-O.z);
}
float IntersectSphere(glm::vec3 O, glm::vec3 D, glm::vec3 S, float R)
{
	glm::vec3 OS = O - S;

	float a = glm::dot(D, D);
	float b = 2 * glm::dot(D, OS);
	float c = glm::dot(OS, OS) - ( R * R);
	float discriminant = b * b - 4 * a * c;
	if (discriminant > 0)
	{
		float d = glm::sqrt(discriminant);
		float t_0 = (-b + d) / (2 * a);
		float t_1 = (-b - d) / (2 * a);

		if (t_0 < 0)
			return INF;
		else if (t_1 < 0)
			return t_0;
		else 
			return t_1;
	}
	else return INF;

}*/