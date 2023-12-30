#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <GL/GLU.h>

#include "star.h"
#include "ui_MainWidget.h"

class MainWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	MainWidget(QWidget *parent = nullptr);
	~MainWidget();

    void eyeXup();
    void eyeXdown();
    void eyeZup();
    void eyeZdown();
protected:
    //设置渲染环境
    void initializeGL();
    //绘制窗口
    void paintGL();
    //响应窗口的大小变化
    void resizeGL(int width, int height);
    //加载纹理
    void loadGLTextures(QString filename, int id);
    //绘制星球
    void drawStar(Star* s);

    GLfloat xRot, yRot, zRot;
    //纹理存储数组
    GLuint texture[13];
    QImage qtexture[13];
    //立方体在X轴和Y轴上旋转的速度
    GLfloat xSpeed, ySpeed;

    GLUquadricObj* mySphere;

    Star* sky;
    Star* sun;
    Star* mercury;
    Star* venus;
    Star* earth;
    Star* mars;
    Star* jupiter;
    Star* saturn;
public:
    GLfloat eyeX;
    GLfloat eyeY;
    GLfloat eyeZ;

	Ui::MainWidgetClass ui;
};
