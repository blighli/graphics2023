#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

using namespace std;

vector<string> Stringsplit(string str, const const char split)
{
    vector<string> res;
    istringstream iss(str);	// 输入流
    string token;			// 接收缓冲区
    while (getline(iss, token, split))	// 以split为分隔符
    {
        res.push_back(token);
    }
    return res;
}


class ObjLoader {
public:
    ObjLoader(string filename) {
        std::ifstream file(filename);
        std::string line;
        while (getline(file, line))
        {
            if (line.substr(0, 2) == "vt")
            {
                vector<GLfloat> uv;
                GLfloat x, y;
                std::istringstream s(line.substr(2));
                s >> x; s >> y;
                uv.push_back(x);
                uv.push_back(y);
                vNormals.push_back(uv);
            }
            else if (line.substr(0, 2) == "vn")
            {
                vector<GLfloat> normal;
                GLfloat x, y, z;
                std::istringstream s(line.substr(2));
                s >> x; s >> y; s >> z;
                normal.push_back(x);
                normal.push_back(y);
                normal.push_back(z);
                vNormals.push_back(normal);
            }
            else if (line.substr(0, 1) == "v")
            {
                vector<GLfloat> Point;
                GLfloat x, y, z;
                std::istringstream s(line.substr(2));
                s >> x; s >> y; s >> z;
                Point.push_back(x);
                Point.push_back(y);
                Point.push_back(z);
                vSets.push_back(Point);

            }
            else if (line.substr(0, 1) == "f")
            {
                vector<GLint> vIndexSets;
                GLint u, v, w;
                string t;
                vector<string> vs1 = Stringsplit(line.substr(2), ' ');
                u = stoi(Stringsplit(vs1[0], '/')[0]);
                vIndexSets.push_back(u - 1);
                v = stoi(Stringsplit(vs1[1], '/')[0]);
                vIndexSets.push_back(v - 1);
                w = stoi(Stringsplit(vs1[2], '/')[0]);
                vIndexSets.push_back(w - 1);
                fSets.push_back(vIndexSets);
            }
            else if (line.substr(0, 1) == "#")
            {

            }
            else
            {

            }
        }
        std::cout << "Mesh included" << std::endl;
        file.close();
    }
    vector<vector<GLfloat>> getvSets() {
        return this->vSets;
    }
    vector<vector<GLint>> getfSets() {
        return this->fSets;
    }
    vector<vector<GLfloat>>vNormals;
    vector<vector<GLfloat>>vuvs;
    vector<vector<GLfloat>>vSets;//存放顶点(x,y,z)坐标
    vector<vector<GLint>>fSets;//存放面的三个顶点索引
private:
};
