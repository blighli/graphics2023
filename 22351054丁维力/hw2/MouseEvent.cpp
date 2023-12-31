#include "MouseEvent.h"
#include <stdio.h>

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };
const float MINSCALE = { 0.05f };

float	Scale;					// scaling factor

const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

int		ActiveButton;			// current button that is down
int		Xmouse, Ymouse;			// mouse values

float	Xrot, Yrot;				// rotation angles in degrees
float	Xscreen, Yscreen;
//int showTimes;
glm::vec3 v(2.f, 2.f, 2.f);
void MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	// get the proper button bit mask:
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		
		b = LEFT;		break;
	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;
	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;
	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}
	// button down sets the bit, up clears the bit:
	if (state == GLUT_DOWN)
	{
		Xscreen = x;
		Yscreen = y;
		Xmouse = x;
		Ymouse = y;
		//CastRay(x,y);
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}
// called when the mouse moves while a button is down:
void MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;
	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT*dy);
		Yrot += (ANGFACT*dx);
	}
	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}
	Xmouse = x;			// new current position
	Ymouse = y;

	//glutSetWindow(MainWindow);
	glutPostRedisplay();
}
// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
void Reset()
{
	ActiveButton = 0;
	//AxesOn = 1;
	//DebugOn = 0;
	//DepthBufferOn = 1;
	//DepthFightingOn = 0;
	//DepthCueOn = 0;
	Scale = 1.0;
	//WhichColor = WHITE;
	//WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}

/*void CastRay(int x, int y)
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
	glm::vec3 O(0.0, 0.0, 5.0);
	glm::vec3 D(posX - O.x, posY - O.y, posZ - O.z);
	if (IntersectSphere(O, D, glm::vec3(0.0,0.0,0.0), 1.0)<INF)
	{
		showTimes = 1000;
 	}
}
float IntersectSphere(glm::vec3 O, glm::vec3 D, glm::vec3 S, float R)
{
	glm::vec3 OS = O - S;

	float a = glm::dot(D, D);
	float b = 2 * glm::dot(D, OS);
	float c = glm::dot(OS, OS) - (R * R);
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