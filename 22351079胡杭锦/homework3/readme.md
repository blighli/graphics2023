# homework3

## 作业要求

1. 支持三维模型显示、至少一种（STL、OBJ、3DS）
2. 支持多个光源的光照效果，使用着色器渲染
3. 支持多种视点浏览方式（以模型为中心的平移旋转和缩放，以视点为中心的场景漫游）

## 完成情况

实现以上要求，绘制塔楼场景，场景中使用了一个点光源和一个定向光源，使用assimp导入OBJ格式3D素材  
使用cmake链接库的方式配置opengl环境  
采用核心模式完成程序编写任务  

## 其他说明

**操作**：使用鼠标控制相机视角转动，键盘wasd控制相机位置，q,e控制摄像机升降，滑轮控制缩放，esc退出  
**目录结构**：debug目录下包含有可执行文件，include文件夹下为使用的头文件，lib目录下为使用的库文件，shader目录下是我使用的shader代码，img目录下为纹理，model目录下包含了使用的模型  
**参考**：部分代码学习自learnopengl
**素材使用**：3D模型来自<https://free3d.com/> 开源模型

## 截图如下

![img_3.png](img_3.png)