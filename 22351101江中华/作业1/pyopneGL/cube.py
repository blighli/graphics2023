import pygame  # 导入 Pygame 库，用于创建游戏窗口和处理事件
from pygame.locals import *  # 导入 Pygame 的本地模块，包含常用的变量和函数
from OpenGL.GL import *  # 导入 OpenGL 的核心功能
from OpenGL.GLUT import *  # 导入 OpenGL 的实用工具库
from OpenGL.GLU import *  # 导入 OpenGL 的实用工具库

# 定义立方体的顶点坐标
vertices = (
    (1, -1, -1), (1, 1, -1),  # 前面的两个顶点
    (-1, 1, -1), (-1, -1, -1),  # 左面的两个顶点
    (1, -1, 1), (1, 1, 1),  # 后面的两个顶点
    (-1, -1, 1), (-1, 1, 1)  # 右面的两个顶点
)

# 定义立方体的面
faces = (
    (0, 1, 2, 3),  # 前面的四个顶点
    (3, 2, 7, 6),  # 左面的四个顶点
    (6, 7, 5, 4),  # 后面的四个顶点
    (4, 5, 1, 0),  # 右面的四个顶点
    (1, 5, 7, 2),  # 上面的四个顶点
    (4, 0, 3, 6)  # 下面的四个顶点
)

# 定义面的颜色
colors = (
    (1, 0, 0),  # 红色
    (0, 1, 0),  # 绿色
    (0, 0, 1),  # 蓝色
    (1, 1, 0),  # 黄色
    (1, 0, 1),  # 紫色
    (0, 1, 1),  # 青色
    (1, 1, 1),  # 白色
    (0, 0, 0)  # 黑色
)

def Cube():
    """
    绘制立方体
    """
    glBegin(GL_QUADS)  # 开始绘制四边形
    for face in faces:
        x = 0
        for vertex in face:
            x += 1
            glColor3fv(colors[x])  # 设置顶点颜色
            glVertex3fv(vertices[vertex])  # 设置顶点坐标
    glEnd()  # 结束绘制四边形

def main():
    """
    主函数
    """
    pygame.init()  # 初始化 Pygame
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)  # 创建窗口
    pygame.display.set_caption("jiang22351101")
    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)  # 设置透视参数
    glTranslatef(0.0, 0.0, -5)  # 平移视图

    # Enable depth testing
    glEnable(GL_DEPTH_TEST)  # 启用深度测试

    while True:  # 主循环
        for event in pygame.event.get():  # 处理事件
            if event.type == pygame.QUIT:  # 如果是退出事件，则退出程序
                pygame.quit()
                quit()

        glRotatef(1, 3, 1, 1)  # 旋转立方体
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)  # 清除屏幕和深度缓冲
        Cube()  # 绘制立方体
        pygame.display.flip()  # 刷新屏幕
        pygame.time.wait(10)  # 稍微等待一下，减少 CPU 占用

main()  # 调用主函数，启动程序