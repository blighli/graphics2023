#include "MyOpenGLWidget.h"

MyOpenGLWidget::MyOpenGLWidget(QWidget*parent)
	: QOpenGLWidget(parent)
{

}

MyOpenGLWidget::~MyOpenGLWidget()
{}

void MyOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f, 
		0.0f, 0.5f, 0.0f
	};

	// 生成一个VAO
	glGenVertexArrays(1, &VAO);

	// 利用VBO来完成CPU到GPU的数据传输
	GLuint VBO;
	// 生成一个VBO
	glGenBuffers(1, &VBO);
	
	// VAO 绑定, 所有跟 GL_ARRAY_BUFFER 有关系的操作状态，它都会记录下来
	glBindVertexArray(VAO);
	// VBO 和 GPU缓存绑定
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 数据 放在 GPU缓存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// 清理 VAO 和 VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 定义着色器
	// 顶点着色器
	QOpenGLShader vertexShader(QOpenGLShader::Vertex);
	vertexShader.compileSourceFile("shader/triangle.vert");
	// 片段着色器
	QOpenGLShader fragmentShader(QOpenGLShader::Fragment);
	fragmentShader.compileSourceFile("shader/triangle.frag");
	// 添加到着色器，并链接
	shaderProgram.addShader(&vertexShader);
	shaderProgram.addShader(&fragmentShader);
	shaderProgram.link();

	time.start();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
}

void MyOpenGLWidget::paintGL()
{
	// 清除颜色并绘制背景
	glClearColor(qAbs(qSin((float)time.elapsed() / 1000)), 0.4, 0.6, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	shaderProgram.bind();
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	GLuint loc = shaderProgram.uniformLocation("outColor");
	shaderProgram.setUniformValue(loc, QVector4D(0.4, qAbs(qSin((float)time.elapsed() / 1000)), 0.3, 1));

	update();
}
