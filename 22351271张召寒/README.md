# 22351271张召寒

## 作业1

* 使用premake5配置vs2019项目，使用glad+glfw库，双击`GenerateProject.bat`文件来构建项目，需要注意的是，要在非中文路径下构建，否则premake会报错

## 作业2

* 使用premake5配置vs2022项目，使用glad+glfw+glm库双击`GenerateProject.bat`文件来构建项目，需要注意的是，要在非中文路径下构建，否则premake会报错

* 实现的特性：
  
  * 太阳作为光照，使用的是sun的顶点着色器和片段着色器
  
  * 使用太阳、地球和月亮图片进行纹理映射
  
  * 地球和月亮使用light顶点着色器和片段着色器，实现Phong光照模型
