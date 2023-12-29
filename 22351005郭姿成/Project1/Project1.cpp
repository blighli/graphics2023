#include <gl/glut.h>
#include <iostream>
#include <vector>

//绕每个轴旋转的角度
GLfloat angle_x = 30;
GLfloat angle_y = 30;
GLfloat angle_z = 30;

//点结构体
struct Point {
	GLfloat x, y, z;//位置
	GLfloat r, g, b;//颜色
	Point() = default;
};

//定义一个Point数组为面，每个面包括四个点
using face = std::vector<Point>;
//faces数组中存了所有面
std::vector<face> Faces;

//立方体顶点
GLfloat vertexes[8][3] = { { 1.0, 1.0, 1.0},
						   { 1.0,-1.0, 1.0},
						   {-1.0,-1.0, 1.0},
						   {-1.0, 1.0, 1.0},
						   { 1.0, 1.0,-1.0},
						   { 1.0,-1.0,-1.0},
						   {-1.0,-1.0,-1.0},
						   {-1.0, 1.0,-1.0} };
//立方体六个面
int facesId[6][4] = { { 0, 1, 2, 3},
					{ 4, 5, 6, 7},
					{ 0, 4, 7, 3},
					{ 1, 5, 6, 2},
					{ 0, 4, 5, 1},
					{ 3, 7, 6, 2} };
//六个面分别的颜色
GLfloat facesColor[6][3] = { { 1.0, 1.0, 0.0},
						   { 1.0, 0.0, 1.0},
						   { 0.0, 1.0, 1.0},
						   { 1.0, 0.0, 0.0},
						   { 0.0, 1.0, 0.0},
						   { 0.0, 0.0, 1.0} };


//画一个面
void drawFace(face& points)
{
	glBegin(GL_POLYGON);
	for (auto& pt : points)
	{
		glColor3f(pt.r, pt.g, pt.b);
		glVertex3f(pt.x, pt.y, pt.z);
	}
	glEnd();
}

//当窗口发生变化（如改变大小）时自动调用
void mydisplay()
{
	//清除颜色缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto& tmp_face : Faces)
	{
		drawFace(tmp_face);
	}
	//修改视角
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle_x, 1, 0, 0);
	glRotatef(angle_y, 0, 1, 0);
	glRotatef(angle_z, 0, 0, 1);
	//使用DOUBLE_BUFFER后，使用以下代码来交换前后台内存
	glutSwapBuffers();
}

//初始化函数，一般包括视角等
void init()
{
	//全屏颜色变成黑色
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//开启深度，阻挡后面的元素
	glEnable(GL_DEPTH_TEST);
	//改变投影视图，
	glMatrixMode(GL_PROJECTION);
	//opengl是一个状态机，要先清空之前的变换矩阵数据，所以每次视角操作时都要先变为单位矩阵
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
	//修改视角
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle_x, 1, 0, 0);
	glRotatef(angle_y, 0, 1, 0);
	glRotatef(angle_z, 0, 0, 1);
}

//初始化六个面数组
void initVertexes()
{
	for (int i = 0; i < 6; i++)
	{
		face tmp_face;
		for (int j = 0; j < 4; j++)
		{
			Point pt;
			pt.x = vertexes[facesId[i][j]][0];
			pt.y = vertexes[facesId[i][j]][1];
			pt.z = vertexes[facesId[i][j]][2];
			pt.r = facesColor[i][0];
			pt.g = facesColor[i][1];
			pt.b = facesColor[i][2];
			tmp_face.emplace_back(pt);
		}
		Faces.push_back(tmp_face);
	}
}

//旋转函数
void rotate(int x)
{
	angle_x += 1;
	if (angle_x >= 360)angle_x = 0;
	//立刻绘制
	glutPostRedisplay();
	//设置新定时器
	glutTimerFunc(10, rotate, 0);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//displayMode，增加GLUT_DEPTH使得深度
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//设置窗口名
	glutCreateWindow("Cubic");

	//初始化六个面
	initVertexes();

	//设置定时函数
	glutTimerFunc(10, rotate, 0);
	//绑定display函数
	glutDisplayFunc(mydisplay);
	//设定opengl初始状态
	init();

	//开启窗口循环
	glutMainLoop();
	return 0;
}
