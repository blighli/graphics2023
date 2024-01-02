#pragma once
#include <QOpenGLWidget>

class Star
{
public:
    Star();
    Star(int tex, GLfloat r, GLfloat x, GLfloat y, GLfloat revS, GLfloat rotS, GLfloat a[], GLfloat d[], GLfloat p[], GLfloat s);
    ~Star();
    //公转
    void revolute();
    //自转
    void rotate();
    //星球半径
    float radious;
    //星球位置
    GLfloat disX;
    GLfloat disY;
    //纹理id
    int texId;
    //环境反射光
    GLfloat* ambient;
    //漫反射
    GLfloat* diffuse;
    //镜面反射
    GLfloat* specular;
    //镜面反射强度
    GLfloat shinniness;
    //公转速度
    GLfloat revSpeed;
    //自转速度
    GLfloat rotSpeed;
    //公转角度
    float revAngle;
    //自转角度
    float rotAngle;
};

