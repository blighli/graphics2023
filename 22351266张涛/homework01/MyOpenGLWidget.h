#pragma once

#include <QWidget>
#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglshaderprogram.h>
#include <QElapsedTimer>
#include "ui_MyOpenGLWidget.h"

class MyOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	MyOpenGLWidget(QWidget *parent = nullptr);
	~MyOpenGLWidget();

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

private:
	Ui::MyOpenGLWidgetClass ui;
	
	// VAO是一个记忆机
	GLuint VAO;
	QOpenGLShaderProgram shaderProgram;
	QElapsedTimer time;
};
