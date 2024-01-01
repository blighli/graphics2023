#include "MMGL.hpp"

MMGLVAO::MMGLVAO() {
	glGenVertexArrays(1, &vao);

}

MMGLVAO::~MMGLVAO() {
	for (int i = 0; i < vboList.size(); i++) {
		GLuint vbo = vboList[i];
		glDeleteBuffers(1, &vbo);
	}
	vboList.clear();
	if (ebo != 0) {
		glDeleteBuffers(1, &ebo);
		ebo = 0;
	}
	if (vao != 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

}

int MMGLVAO::AddVertex3D(float* data, int vertexCount, int layout) {
	glBindVertexArray(vao); // 绑定vao

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), data, GL_STATIC_DRAW);

	glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);

	glEnableVertexAttribArray(layout);

	vboList.push_back(vbo);

	glBindVertexArray(vao); // 解绑vao
	return 0;
}

int MMGLVAO::SetIndex(unsigned int * indexData, int indexCount) {
	glBindVertexArray(vao); // 绑定vao

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(float), indexData, GL_STATIC_DRAW);

	glBindVertexArray(vao); // 解绑vao

	drawTime = indexCount;
	return 0;
}

int MMGLVAO::BindVAO() {
	glBindVertexArray(vao); // 绑定vao
	return 0;
}

int MMGLVAO::Draw() {
	BindVAO();
	glDrawElements(GL_TRIANGLES, drawTime, GL_UNSIGNED_INT, 0);
	return 0;
}