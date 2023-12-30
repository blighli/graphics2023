#include "star.h"

Star::Star()
{
}

Star::Star(int tex, GLfloat r, GLfloat x, GLfloat y, GLfloat revS, GLfloat rotS, GLfloat a[], GLfloat d[], GLfloat p[], GLfloat s)
{
    this->texId = tex;
    this->radious = r;
    this->disX = x;
    this->disY = y;
    this->ambient = a;
    this->diffuse = d;
    this->specular = p;
    this->shinniness = s;
    this->revSpeed = revS;
    this->rotSpeed = rotS;
    this->revAngle = 0.0;
    this->rotAngle = 0.0;

}

Star::~Star()
{}

void Star::revolute()
{
    this->revAngle = this->revAngle + this->revSpeed < 360 ? this->revAngle + this->revSpeed : 0;
}
void Star::rotate()
{
    this->rotAngle = this->rotAngle + this->rotSpeed < 360 ? this->rotAngle + this->rotSpeed : 0;
}