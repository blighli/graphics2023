// Arc_circle.cpp : 定义控制台应用程序的入口点。
//

#define NDEBUG
#include<math.h>
#include<GL/glut.h>

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

const double PI = 3.1415926;
GLdouble dpt;

void init(void) {
	glClearColor(0.3, 0.5, 0.3, 1.0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 400.0, 0.0, 400.0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
}


void DrawCircle(GLfloat PX, GLfloat PY, GLfloat Radius, int red, int green, int blue, int shape)
{
	GLfloat x, y, ta, te, dt = 0.01;
	if (Radius < 100)
	{
		if ((red*green*blue - 0.5)*(Radius - 20) > 0)
		{
			PX = 200 - 72.0f*cos(dpt);
			PY = 200 - 72.0f*sin(dpt);
		}
		else
		{
			PX = 200 + 72.0f*cos(dpt);
			PY = 200 + 72.0f*sin(dpt);
		}
	}
	switch (shape)
	{
	case -1:
			ta = -PI + dpt;
			te = 0 + dpt;
			break;
	case 0:
		ta = 0;
		te = 2 * PI;
		break;
	case 1:
		ta = 0 + dpt;
		te = PI + dpt;
		break;
	}
	glColor3f(red, green, blue);
	glBegin(GL_POLYGON);
	while (ta < te)
	{
		x = PX + Radius*cos(ta);
		y = PY + Radius*sin(ta);
		glVertex2f(x, y);
		ta += dt;
	}
	glEnd();
	glFlush();
}

void Circle_Drawing()
{
	glClear(GL_COLOR_BUFFER_BIT);
	DrawCircle(200, 200, 144, 0, 0, 0, 1);
	DrawCircle(200, 200, 144, 1, 1, 1, -1);
	DrawCircle(128, 200, 72, 1, 1, 1, 0);
	DrawCircle(272, 200, 72, 0, 0, 0, 0);
	DrawCircle(128, 200, 8, 0, 0, 0, 0);
	DrawCircle(272, 200, 8, 1, 1, 1, 0);
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	Circle_Drawing();
	dpt -= 0.05f;
	glutSwapBuffers();
}

void ChangeSize(GLsizei w, GLsizei h)
{
	GLdouble windowWidth;
	GLdouble windowHeight;
	if (h == 0)
	{
		h = 1;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
	{
		windowHeight = 400.0*h / w;
		windowWidth = 400.0f;
		glViewport(0, (h - w) / 2, w, h);
	}
	else
	{
		windowHeight = 400.f;
		windowWidth = 400.0*h / w;
		glViewport((w - h) / 2, 0, w, h);
	}
	glOrtho(0.0f, windowWidth, 0.0f, windowHeight, 1.0f, -1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(25, TimerFunction, value);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(420, 120);
	glutInitWindowSize(500, 500);
	glutCreateWindow("太极GIF");
	init();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(25, TimerFunction, 1);
	glutMainLoop();
	return 0;
}