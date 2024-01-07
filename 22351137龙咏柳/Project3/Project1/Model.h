#include <vector>

class Imported
{
private:
    int numVertices{};//存放点的个数
    std::vector<glm::vec3> vertices;//存放顶点集
    std::vector<glm::vec3> normalVecs;//存放法向集
    std::vector<float> vertVals;//存放读入的点的值
    std::vector<float> triangleVerts;//存放按面的索引排序后的顶点集
    std::vector<float> fnormals;//存放按面的索引排序后的法向集
    std::vector<float> normVals;//存放法向的值
public:
    Imported();
    Imported(const char *filePath);//文件名构造，传输本地模型
    int getNumVertices() const;//返回顶点个数的函数
    std::vector<glm::vec3> getVertices();//返回顶点集vertices的函数
    std::vector<glm::vec3> getNormals();//返回法向集normalVecs的函数
};
