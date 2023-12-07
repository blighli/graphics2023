import glfw
from OpenGL.GL import *
from OpenGL.GLUT import *

def draw_triangle():
    glBegin(GL_TRIANGLES)
    glColor3f(1.0, 0.0, 0.0)  # 设置颜色为红色
    glVertex2f(0.0, 1.0)     # 第一个顶点
    glColor3f(0.0, 1.0, 0.0)  # 设置颜色为绿色
    glVertex2f(-1.0, -1.0)   # 第二个顶点
    glColor3f(0.0, 0.0, 1.0)  # 设置颜色为蓝色
    glVertex2f(1.0, -1.0)    # 第三个顶点
    glEnd()

def display():
    glClear(GL_COLOR_BUFFER_BIT)
    draw_triangle()
    glFlush()

def main():
    # 初始化glfw
    if not glfw.init():
        return

    # 创建窗口
    window = glfw.create_window(800, 600, "Simple OpenGL", None, None)

    if not window:
        glfw.terminate()
        return

    # 设置窗口为当前上下文
    glfw.make_context_current(window)

    # 设置绘制函数
    glfw.set_key_callback(window, key_callback)

    while not glfw.window_should_close(window):
        display()
        glfw.swap_buffers(window)
        glfw.poll_events()

    glfw.terminate()

def key_callback(window, key, scancode, action, mods):
    if key == glfw.KEY_ESCAPE and action == glfw.PRESS:
        glfw.set_window_should_close(window, True)

if __name__ == "__main__":
    main()
