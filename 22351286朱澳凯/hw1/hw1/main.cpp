#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

double rtri = 60.0;
double rquad = 60.0;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// 清除屏幕及深度缓存
	glMatrixMode(GL_MODELVIEW);//模型视图矩阵4x4
	glLoadIdentity();							// 重置当前的模型观察矩阵
	//glutSolidSphere(1.0, 400, 16);  //繪製一個球體
	/*当您调用glLoadIdentity()之后，您实际上将当前点移到了屏幕中心，X坐标轴从左至右，Y坐标轴从下至上，Z坐标轴从里至外。OpenGL屏幕中心的坐标值是X和Y轴上的0.0f点。中心左面的坐标值是负值，右面是正值。移向屏幕顶端是正值，移向屏幕底端是负值。移入屏幕深处是负值，移出屏幕则是正值。
	 glTranslatef(x, y, z)沿着 X, Y 和 Z 轴移动。根据前面的次序，下面的代码沿着X轴左移1.5个单位，Y轴不动(0.0f)，最后移入屏幕6.0f个单位。注意在glTranslatef(x, y, z)中当您移动的时候，您并不是相对屏幕中心移动，而是相对与当前所在的屏幕位置。*/

	glPushMatrix();
	glTranslatef(-2.5f, 0.0f, -6.0f);						// 左移 1.5 单位，并移入屏幕 6.0
	//glRotatef(rtri, 0.0f, 1.0f, 0.0f);				// 绕Y轴旋转三角形

	glBegin(GL_TRIANGLES);							// 绘制三角形
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0f, 1.0f, 0.0f);					// 上顶点
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(-0.5f, 0.0f, 0.0f);					// 左下
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.5f, 0.0f, 0.0f);					// 右下
	glEnd();								// 三角形绘制结束
	glPopMatrix();

	//绘制四棱锥
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef(4.0f, -0.5f, -6.0f);
	glRotatef(rtri, 0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	//繪製上頂點 左側面
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	//右側面
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	//右后側面
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);

	//左后側面
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glEnd();
	glPopMatrix();
	//绘制茶壶
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -8.0f);
	glutWireTeapot(2.0f);
	glPopMatrix();

	//設置當前使用的顏色為白色
	glColor3f(1.0, 1.0, 1.0);
	glFlush();

	rtri += 0.3;
	rquad += 0.2;
}

void myReshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//设置相机
	glMatrixMode(GL_PROJECTION);//投影矩阵4x4
	glLoadIdentity();
	gluPerspective(60.0f, (GLdouble)w / (GLdouble)h, 1.0f, 800.0f);
}
/*当您调用glLoadIdentity()之后，您实际上将当前点移到了屏幕中心，X坐标轴从左至右，Y坐标轴从下至上，Z坐标轴从里至外。OpenGL屏幕中心的坐标值是X和Y轴上的0.0f点。中心左面的坐标值是负值，右面是正值。移向屏幕顶端是正值，移向屏幕底端是负值。移入屏幕深处是负值，移出屏幕则是正值。
 glTranslatef(x, y, z)沿着 X, Y 和 Z 轴移动。注意在glTranslatef(x, y, z)中当您移动的时候，您并不是相对屏幕中心移动，而是相对与当前所在的屏幕位置（视景体）。*/


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("第一个OpenGL程序");

	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}