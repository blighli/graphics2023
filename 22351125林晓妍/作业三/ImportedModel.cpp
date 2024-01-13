#include "ImportedModel.h"
#include <sstream>


ImportedModel::ImportedModel()
{

}

ImportedModel::ImportedModel(const char* filePath)
{
	ModelImporter modelImporter = ModelImporter();
	modelImporter.parseOBJ(filePath);
	_numVertices = modelImporter.getNumVertices();
	vector<float> verts = modelImporter.getVertices();
	vector<float> tcs = modelImporter.getTextureCoordinates();
	vector<float> normals = modelImporter.getNormals();

	for (int i=0; i<_numVertices; i++)
	{
		_vertices.push_back(glm::vec3(verts[i * 3 + 0], verts[i * 3 + 1], verts[i * 3 + 2]));
		_texCoords.push_back(glm::vec2(tcs[i * 2 + 0], tcs[i * 2 + 1]));
		_normalVecs.push_back(glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
}

int ImportedModel::getNumVertices()
{
	return _numVertices;
}

std::vector<glm::vec3> ImportedModel::getVertices()
{
	return _vertices;
}

std::vector<glm::vec2> ImportedModel::getTextureCoords()
{
	return _texCoords;
}

std::vector<glm::vec3> ImportedModel::getNormals()
{
	return _normalVecs;
}

/// <summary>
/// ModelImporter implement
/// </summary>

ModelImporter::ModelImporter()
{

}

void ModelImporter::parseOBJ(const char* filePath)
{
	float x = 0.f, y = 0.f, z = 0.f;
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0)    //注意v后面有空格
		{
			std::stringstream ss(line.erase(0, 1));
			ss >> x >> y >> z;
			//ss >> x; ss >> y; ss >> z;
			_vertVals.push_back(x);
			_vertVals.push_back(y);
			_vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x >> y;
			_stVals.push_back(x);
			_stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x >> y >> z;
			_normVals.push_back(x);
			_normVals.push_back(y);
			_normVals.push_back(z);
		}
		if (line.compare(0, 1, "f") == 0)  //原书有误
		{
			string oneCorner, v, t, n;
			std::stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++)
			{
				getline(ss, oneCorner, ' ');
				//getline(ss, oneCorner, " ");
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertRef = (stoi(v) - 1) * 3;   //为什么要 -1？
				int tcRef = (stoi(t) - 1) * 2;
				int normRef = (stoi(n) - 1) * 3;

				_triangleVerts.push_back(_vertVals[vertRef]);
				_triangleVerts.push_back(_vertVals[vertRef + 1]);
				_triangleVerts.push_back(_vertVals[vertRef + 2]);

				_textureCoords.push_back(_stVals[tcRef]);
				_textureCoords.push_back(_stVals[tcRef + 1]);

				_normals.push_back(_normVals[normRef]);
				_normals.push_back(_normVals[normRef + 1]);
				_normals.push_back(_normVals[normRef + 2]);
			}
			 
		}
	}
}

int ModelImporter::getNumVertices()
{
	return (_triangleVerts.size() / 3);
}

std::vector<float> ModelImporter::getVertices()
{
	return _triangleVerts;
}

std::vector<float> ModelImporter::getTextureCoordinates()
{
	return _textureCoords;
}

std::vector<float> ModelImporter::getNormals()
{
	return _normals;
}
