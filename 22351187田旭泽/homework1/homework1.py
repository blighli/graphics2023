import pygame  # 导入 Pygame 库，用于创建游戏窗口和处理事件
from pygame.locals import *  # 导入 Pygame 的本地模块，包含常用的变量和函数

from OpenGL.GL import *  # 导入 OpenGL 的核心功能
from OpenGL.GLUT import *  # 导入 OpenGL 的实用工具库
from OpenGL.GLU import *  # 导入 OpenGL 的实用工具库

# 定义三角形的顶点
vertices = [
    [0, 1, 0],  # 顶点0
    [-1, -1, 0],  # 顶点1
    [1, -1, 0]  # 顶点2
]

# 定义三角形的颜色
colors = [
    [1, 0, 0],  # 红色
    [0, 1, 0],  # 绿色
    [0, 0, 1]  # 蓝色
]

def Triangle():
    """
    绘制三角形
    """
    glBegin(GL_TRIANGLES)  # 开始绘制三角形
    for i, vertex in enumerate(vertices):
        glColor3fv(colors[i])  # 设置颜色
        glVertex3fv(vertex)  # 设置顶点
    glEnd()  # 结束绘制三角形

def main():
    """
    主函数
    """
    pygame.init()  # 初始化 Pygame
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF|OPENGL)  # 创建窗口

    gluPerspective(45, (display[0]/display[1]), 0.1, 50.0)  # 设置透视参数
    glTranslatef(0.0, 0.0, -5)  # 平移视图

    while True:  # 主循环
        for event in pygame.event.get():  # 处理事件
            if event.type == pygame.QUIT:  # 如果是退出事件，则退出程序
                pygame.quit()
                quit()

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)  # 清除屏幕和深度缓冲

        Triangle()  # 绘制三角形

        pygame.display.flip()  # 刷新屏幕
        pygame.time.wait(10)  # 稍微等待一下，减少 CPU 占用

main()  # 调用主函数，启动程序