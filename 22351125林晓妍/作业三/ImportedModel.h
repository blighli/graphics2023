#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>

using namespace std;

class ImportedModel
{
private:
	int _numVertices;     //所有顶点坐标总数
	std::vector<glm::vec3> _vertices;    //所有顶点个数，一个顶点个数包含(x,y,z)
	std::vector<glm::vec2> _texCoords;   //纹理坐标（u，v）
	std::vector<glm::vec3> _normalVecs;     //法线
public:
	ImportedModel();
	ImportedModel(const char* filePath);
	int getNumVertices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTextureCoords();
	std::vector<glm::vec3> getNormals();
};


class ModelImporter
{
private:
	std::vector<float> _vertVals;
	std::vector<float> _triangleVerts;
	std::vector<float> _textureCoords;
	std::vector<float> _stVals;
	std::vector<float> _normals;
	std::vector<float> _normVals;
public:
	ModelImporter();
	void parseOBJ(const char* filePath);
	int getNumVertices();
	std::vector<float> getVertices();
	std::vector<float> getTextureCoordinates();
	std::vector<float> getNormals();

};

