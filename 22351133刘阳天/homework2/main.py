import glfw
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from PIL import Image

# 纹理加载函数


def loadTexture(imageName):
    # 使用Pillow库打开图像文件
    im = Image.open(imageName)
    try:
        # 尝试获取图像的尺寸和数据（带透明通道）
        ix, iy, image = im.size[0], im.size[1], im.tobytes(
            "raw", "RGBX", 0, -1)
    except SystemError:
        # 如果上面的方法失败，不使用透明通道
        ix, iy, image = im.size[0], im.size[1], im.tobytes("raw", "RGB", 0, -1)

    # 创建一个新的纹理ID
    ID = glGenTextures(1)
    # 绑定新创建的纹理ID
    glBindTexture(GL_TEXTURE_2D, ID)
    # 将图像数据加载到纹理中
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ix, iy,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image)
    # 设置纹理参数
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    return ID


def init():
    # 启用纹理映射
    glEnable(GL_TEXTURE_2D)
    # 启用深度测试，用于隐藏被遮挡的部分
    glEnable(GL_DEPTH_TEST)
    # 设置清除颜色为黑色
    glClearColor(0.0, 0.0, 0.0, 0.0)
    # 设置阴影平滑模式
    glShadeModel(GL_SMOOTH)
    # 启用光照
    glEnable(GL_LIGHTING)
    # 启用一个光源
    glEnable(GL_LIGHT0)
    # 设置光源的位置和漫反射颜色
    glLightfv(GL_LIGHT0, GL_POSITION, [0.0, 1.0, 1.0, 0.0])
    glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
    # 设置投影模式
    glMatrixMode(GL_PROJECTION)
    # 设置视角，透视投影参数
    gluPerspective(45, 1.33, 2, 100)
    # 切换到模型视图矩阵
    glMatrixMode(GL_MODELVIEW)


def draw_sphere():
    # 创建一个二次曲面对象
    qobj = gluNewQuadric()
    # 指定二次曲面的纹理坐标生成
    gluQuadricTexture(qobj, GL_TRUE)
    # 绘制球体
    gluSphere(qobj, 1, 32, 32)
    # 删除二次曲面对象
    gluDeleteQuadric(qobj)


def draw():
    # 清除颜色和深度缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    # 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture)
    # 重置视图
    glLoadIdentity()
    # 设置摄像机位置
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0)
    # 绘制球体
    draw_sphere()
    # 交换缓冲区
    glfw.swap_buffers(window)


# 初始化 GLFW
if not glfw.init():
    raise Exception("GLFW 无法初始化")

# 创建窗口
window = glfw.create_window(
    800, 600, "OpenGL Sphere with Texture and Light", None, None)
if not window:
    glfw.terminate()
    raise Exception("GLFW 窗口无法创建")

# 设置当前上下文
glfw.make_context_current(window)

# 初始化 OpenGL 环境
init()
# 加载纹理
texture = loadTexture("OIP1.jpg")

# 主循环
while not glfw.window_should_close(window):
    # 处理事件
    glfw.poll_events()
    # 绘制
    draw()

# 结束，释放资源
glfw.terminate()
