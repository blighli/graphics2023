## 一、项目简介

使用OpenGL（glfw, glad)模拟太阳系（地球、月球、太阳）系统，涉及三维图形变换、坐标系统，光照模型（待添加）、键盘鼠标事件处理等内容，在main函数中封装了绝大部分的OpenGL接口，极大的减少了代码量，操作简便。

## 二、代码特点

Shader.h Camera.h与Texture.h封装着色器、摄像机与纹理类，其中纹理类实例化即完成生成纹理与绑定、设置纹理环绕方式与过滤等工作。

VertexArray，VertexBuffer与IndexBuffer封装VAO, VBO, EBO，类实例化时即自动生成顶点数组对象、顶点缓冲对象、元素缓冲对象，并提供绑定与解绑等接口。VertexBufferLayout类可自由设置顶点属性。

Sphere类获得绘制球体所需的所有顶点坐标以及索引缓冲数组，用于实例化VBO和EBO对象，可手动调节设置球体绘制的精细程度。

通过Renderer类的Draw方法实现球体绘制功能。