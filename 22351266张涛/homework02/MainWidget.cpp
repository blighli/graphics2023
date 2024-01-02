#include "MainWidget.h"
#include <QTimer>

MainWidget::MainWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	ui.setupUi(this);

	xRot = yRot = zRot = 0.0;
	xSpeed = ySpeed = 0.0;
	mySphere = gluNewQuadric();

	eyeX = 0;
	eyeY = 0;
	eyeZ = 150.0;

	//夜空参数设置
	GLfloat  sky_ambient[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  sky_diffuse[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  sky_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  sky_shininess = 20.0;
	GLfloat  sky_radious = 290.0;
	sky = new Star(0, sky_radious, 0, 0, 0, 0, sky_ambient, sky_diffuse, sky_specular, sky_shininess);

	//太阳参数设置
	GLfloat  sun_ambient[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  sun_diffuse[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  sun_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  sun_shininess = 20.0;
	GLfloat  sun_radious = 10.0;
	GLfloat sun_rotSpeed = (GLfloat)360 / 58 / 100;
	sun = new Star(1, sun_radious, 0, 0, 0, sun_rotSpeed, sun_ambient, sun_diffuse, sun_specular, sun_shininess);

	//水星
	GLfloat  mercury_ambient[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  mercury_diffuse[] = { 0.5,0.5,0.5,1.0 };
	GLfloat  mercury_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  mercury_shininess = 20.0;
	GLfloat  mercury_radious = 0.7;
	GLfloat mecury_revSpeed = (GLfloat)360 / 88;
	GLfloat mecury_rotSpeed = (GLfloat)360 / 58 / 100;
	mercury = new Star(2, mercury_radious, 15.2, 0, mecury_revSpeed, mecury_rotSpeed, mercury_ambient, mercury_diffuse, mercury_specular, mercury_shininess);

	//金星
	GLfloat  venus_ambient[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  venus_diffuse[] = { 0.8,0.8,0.8,1.0 };
	GLfloat  venus_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  venus_shininess = 20.0;
	GLfloat  venus_radious = 1.24;
	GLfloat venus_revSpeed = (GLfloat)360 / 224;
	GLfloat venus_rotSpeed = (GLfloat)360 / 243 / 100;
	venus = new Star(3, venus_radious, 19.2, 0, venus_revSpeed, venus_rotSpeed, venus_ambient, venus_diffuse, venus_specular, venus_shininess);

	//地球
	GLfloat  earth_ambient[] = { 0.1,0.1,0.1,1.0 };
	GLfloat  earth_diffuse[] = { 0.4,0.4,0.8,1.0 };
	GLfloat  earth_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  earth_shininess = 20.0;
	GLfloat  earth_radious = 1.24;
	GLfloat earth_revSpeed = (GLfloat)360 / 365;
	GLfloat earth_rotSpeed = (GLfloat)360 / 1 / 100;
	earth = new Star(4, earth_radious, 26, 0, earth_revSpeed, earth_rotSpeed, earth_ambient, earth_diffuse, earth_specular, earth_shininess);

	//火星
	GLfloat  mars_ambient[] = { 0.1,0.1,0.1,1.0 };
	GLfloat  mars_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat  mars_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  mars_shininess = 20.0;
	GLfloat  mars_radious = 1.0;
	GLfloat mars_revSpeed = (GLfloat)360 / 687;
	GLfloat mars_rotSpeed = (GLfloat)360 / 1 / 100;
	mars = new Star(5, mars_radious, 31, 0, mars_revSpeed, mars_rotSpeed, mars_ambient, mars_diffuse, mars_specular, mars_shininess);

	//木星
	GLfloat  jupiter_ambient[] = { 0.0, 0.0, 0.0,1.0 };
	GLfloat  jupiter_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat  jupiter_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  jupiter_shininess = 20.0;
	GLfloat  jupiter_radious = 4.0;
	GLfloat jupiter_revSpeed = (GLfloat)360 / 4329;
	GLfloat jupiter_rotSpeed = (GLfloat)360 / 0.3 / 100;
	jupiter = new Star(6, jupiter_radious, 43, 0, jupiter_revSpeed, jupiter_rotSpeed, jupiter_ambient, jupiter_diffuse, jupiter_specular, jupiter_shininess);

	//土星
	GLfloat  saturn_ambient[] = { 0.0, 0.0, 0.0,1.0 };
	GLfloat  saturn_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat  saturn_specular[] = { 0.0,0.0,0.0,1.0 };
	GLfloat  saturn_shininess = 20.0;
	GLfloat  saturn_radious = 3.5;
	GLfloat saturn_revSpeed = (GLfloat)360 / 10768;
	GLfloat saturn_rotSpeed = (GLfloat)360 / 1.4 / 100;
	saturn = new Star(7, saturn_radious, 56.5, 0, saturn_revSpeed, saturn_rotSpeed, saturn_ambient, saturn_diffuse, saturn_specular, saturn_shininess);
}

MainWidget::~MainWidget()
{}

void MainWidget::loadGLTextures(QString filename, int id)
{
	qtexture[id].load(filename);
	qtexture[id] = qtexture[id].convertToFormat(QImage::Format_RGBA8888, Qt::AutoColor);
}

void MainWidget::drawStar(Star* s)
{
	glPushMatrix();
	//指定纹理
	glTexImage2D(GL_TEXTURE_2D, 0, 3, qtexture[s->texId].width(), qtexture[s->texId].height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, qtexture[s->texId].bits());
	//纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//公转
	glRotatef(s->revAngle, 0.0, 0.0, 1.0);
	glTranslatef(s->disX, s->disY, 0.0);
	//自转
	glRotatef(s->rotAngle, 0.0, 0.0, 1.0);
	gluSphere(mySphere, s->radious, 32, 16);
	//设置材质属性
	glMaterialfv(GL_BACK, GL_AMBIENT, s->ambient);
	glMaterialfv(GL_BACK, GL_DIFFUSE, s->diffuse);
	glMaterialfv(GL_BACK, GL_SPECULAR, s->specular);
	glMaterialf(GL_BACK, GL_SHININESS, s->shinniness);
	glPopMatrix();
}


void MainWidget::initializeGL()
{
	initializeOpenGLFunctions();

	//载入纹理
	loadGLTextures(":/texture/Texture/sun.jpg", sun->texId);
	loadGLTextures(":/texture/Texture/mercury.jpg", mercury->texId);
	loadGLTextures(":/texture/Texture/venus.jpg", venus->texId);
	loadGLTextures(":/texture/Texture/earth.jpg", earth->texId);
	loadGLTextures(":/texture/Texture/mars.jpg", mars->texId);
	loadGLTextures(":/texture/Texture/saturn.jpg", saturn->texId);
	loadGLTextures(":/texture/Texture/jupiter.jpg", jupiter->texId);
	loadGLTextures(":/texture/Texture/sky.jpg", sky->texId);

	// 启用阴影平滑
	glShadeModel(GL_SMOOTH);
	// 黑色背景
	glClearColor(0.0, 0.0, 0.0, 0.0);

	GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //光源的位置在世界坐标系圆心，齐次坐标形式
	GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBA模式的环境光，为0
	GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBA模式的漫反射光，全白光
	GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  //RGBA模式下的镜面光 ，全白光
	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

	//开启灯光
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// 设置深度缓存
	glClearDepth(1.0);
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	//启用纹理
	glEnable(GL_TEXTURE_2D);
	// 所作深度测试的类型
	glDepthFunc(GL_LEQUAL);
	// 告诉系统对透视进行修正
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// 使用平滑法线
	gluQuadricNormals(mySphere, GL_SMOOTH);
	// 使用纹理
	gluQuadricTexture(mySphere, GL_TRUE);
}

void MainWidget::paintGL()
{

	// 清除屏幕和深度缓存
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, texture[sky->texId]);
	drawStar(sky);

	glBindTexture(GL_TEXTURE_2D, texture[sun->texId]);
	drawStar(sun);

	glBindTexture(GL_TEXTURE_2D, texture[mercury->texId]);
	drawStar(mercury);

	glBindTexture(GL_TEXTURE_2D, texture[venus->texId]);
	drawStar(venus);

	glBindTexture(GL_TEXTURE_2D, texture[earth->texId]);
	drawStar(earth);

	glBindTexture(GL_TEXTURE_2D, texture[mars->texId]);
	drawStar(mars);

	glBindTexture(GL_TEXTURE_2D, texture[jupiter->texId]);
	drawStar(jupiter);

	glBindTexture(GL_TEXTURE_2D, texture[saturn->texId]);
	drawStar(saturn);

	//旋转速度
	yRot += 0.4;
	sun->rotate();
	mercury->revolute();
	mercury->rotate();
	venus->revolute();
	venus->rotate();
	earth->revolute();
	earth->rotate();
	mars->revolute();
	mars->rotate();
	jupiter->revolute();
	jupiter->rotate();
	saturn->revolute();
	saturn->rotate();
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0);
	glFlush();

	update();
}

// 重置OpenGL窗口大小
void MainWidget::resizeGL(int width, int height)
{
	// 防止窗口大小变为0
	if (height == 0)height = 1;
	// 重置当前的视口
	glViewport(0, 0, (GLint)width, (GLint)height);
	// 选择投影矩阵
	glMatrixMode(GL_PROJECTION);
	// 重置投影矩阵
	glLoadIdentity();
	// 设置视口的大小
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 600.0);
	// 选择模型观察矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MainWidget::eyeXup()
{
	eyeX += 1;
}

void MainWidget::eyeXdown()
{
	eyeX -= 1;
}

void MainWidget::eyeZup()
{
	eyeZ += 1;
}

void MainWidget::eyeZdown()
{
	eyeZ -= 1;
}