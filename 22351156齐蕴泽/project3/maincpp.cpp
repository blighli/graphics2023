#include <iostream>
#include <stdlib.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <GL/glut.h>
#include <math.h>
#include <Windows.h>
#include <string>
#define  GLUT_WHEEL_UP 3           
#define  GLUT_WHEEL_DOWN 4
using namespace std;
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

int mousetate = 0; 
GLfloat Oldx = 0.0; 
GLfloat Oldy = 0.0;
GLuint texture;

float xRotate = 0.0f; 
float yRotate = 0.0f;
float ty = 0.0f;
float tx = 0.0f;
float scale = 0.004;

//文件读取有关的
MyMesh mesh; 
//"dinosaur.obj";
const string  file = "./objdata/";
const string file_1 = file + "cow.obj";
const string file_2 = file + "bunny.obj";
const string file_3 = file + "dinosaur.obj";
const string file_4 = file + "mba1.obj";
const string file_5 = file + "monkey.obj";
const string file_6 = file + "porsche.obj";
//const string file_7 = "teddy.obj";
const string file_7 = file + "huangfeng.obj";
const string file_8 = file + "file.obj";
const string file_9 = file + "face.sur";
int currentfile = 1;

GLuint showFaceList, showWireList;
int showstate = 1;
bool showFace = true;
bool showWire = false;
bool showFlatlines = false;



void setLightRes() {
    //GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    GLfloat lightPosition[] = { 0.0f, 1.0f, 0.0f, 0.0f }; // 平行光源


    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING); //启用光源
    glEnable(GL_LIGHT0);   //使用指定灯光
}
void SetupRC()
{
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    GLfloat ambientLight[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

//初始化顶点和面   
void initGL()
{
    //实用显示列表
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glClearDepth(2.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST); 
    setLightRes();
    SetupRC();
    showFaceList = glGenLists(1);
    showWireList = glGenLists(1);
    //int temp = mesh.n_edges();

    // 绘制 wire 
    glNewList(showWireList, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);//灰色
    glBegin(GL_LINES);
    //边线三维图绘制
    for (MyMesh::HalfedgeIter he_it = mesh.halfedges_begin(); he_it != mesh.halfedges_end(); ++he_it) {
        glVertex3fv(mesh.point(mesh.from_vertex_handle(*he_it)).data());
        glVertex3fv(mesh.point(mesh.to_vertex_handle(*he_it)).data());
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEndList();

    // 绘制flat
    glNewList(showFaceList, GL_COMPILE);
    //三角面图绘制
    for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        glBegin(GL_TRIANGLES);
        for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
            glNormal3fv(mesh.normal(*fv_it).data());  
            glVertex3fv(mesh.point(*fv_it).data());
        }
        glEnd();
    }
    glEndList();
}


// 当窗体改变大小的时候，改变窗口大小时保持图形比例
void myReshape(GLint w, GLint h)
{
    glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h, -1.0, 1.0, -100.0, 100.0);
    else
        glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w, static_cast<GLdouble>(h) / w, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// 读取文件的函数
void readfile(string file) {
    mesh.request_vertex_normals();
    if (!mesh.has_vertex_normals())
    {
        cout << "错误：标准定点属性 “法线”不存在" << endl;
        return;
    }
    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh, file, opt))
    {
        cout << "无法读取文件:" << file << endl;
        return;
    }
    else cout << "成功读取文件:" << file << endl;
    cout << endl; 
    if (!opt.check(OpenMesh::IO::Options::VertexNormal))
    {
        mesh.request_face_normals();
        mesh.update_normals();
        mesh.release_face_normals();
    }
}

//  键盘交互 1. 切换文件 2.切换显示
void mySpecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        cout << "读取文件:" << file_1 << " 中......" << endl;
        readfile(file_1);
        scale = 0.004;
        currentfile = 1;
        initGL();
        break;
    case GLUT_KEY_F2:
        cout << "读取文件:" << file_2 << " 中......" << endl;
        readfile(file_2);
        scale = 0.003;
        currentfile = 2;
        initGL();
        break;
    case GLUT_KEY_F3:
        cout << "读取文件:" << file_3 << " 中......" << endl;
        readfile(file_3);
        scale = 0.02;
        currentfile = 3;
        initGL();
        break;
    case GLUT_KEY_F4:
        cout << "读取文件:" << file_4 << " 中......" << endl;
        readfile(file_4);
        scale = 0.005;
        currentfile = 3;
        initGL();
        break;
    case GLUT_KEY_F5:
        cout << "读取文件:" << file_5 << " 中......" << endl;
        readfile(file_5);
        scale = 0.5;
        currentfile = 5;
        initGL();
        break;
    case GLUT_KEY_F6:
        cout << "读取文件:" << file_6 << " 中......" << endl;
        readfile(file_6);
        scale = 0.02;
        currentfile = 3;
        initGL();
        break;
    case GLUT_KEY_F7:
        cout << "读取文件:" << file_7 << " 中......" << endl;
        readfile(file_7);
        scale = 0.04;
        currentfile = 3;
        initGL();
        break;
    case GLUT_KEY_F8:
        cout << "读取文件:" << file_8 << " 中......" << endl;
        readfile(file_8);
        scale = 0.04;
        currentfile = 3;
        initGL();
        break;
    case GLUT_KEY_F9:
        cout << "读取文件:" << file_9 << " 中......" << endl;
        readfile(file_9);
        scale = 0.04;
        currentfile = 3;
        initGL();
        break;
    case GLUT_KEY_INSERT:
        if (showFace == true) {
            showFace = false;
            showWire = true;

            cout << "切换显示模式为：WireFrame" << endl;
        }
        else if (showWire == true)
        {
            showWire = false;
            showFlatlines = true;
            cout << "切换显示模式为：Flatlines" << endl;
        }
        else if (showFlatlines == true) {
            showFlatlines = false;
            showFace = true;
            //DisplaySphere(0.5, "G:\\source\\OpenGL\\Cow\\checkerboard3.bmp");
            cout << "切换显示模式为：Flat" << endl;
        }
        break;
    case GLUT_KEY_LEFT:
        tx -= 0.01;
        break;
    case GLUT_KEY_RIGHT:
        tx += 0.01;
        break;
    case GLUT_KEY_UP:
        ty += 0.01;
        break;
    case GLUT_KEY_DOWN:
        ty -= 0.01;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// 鼠标交互
void myMouse(int button, int state, int x, int y)
{    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mousetate = 1;
        Oldx = x;
        Oldy = y;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        mousetate = 0;
    //滚轮事件
    if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
        //cout << "hello" << endl; 
        if (currentfile == 1)
            scale += 0.0005;
        if (currentfile == 2)
            scale += 0.001;
        if (currentfile == 5) {
            scale += 0.1;
        }
        else
            scale += 0.001;
    }
    if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {
        //cout << "good" << endl;
        if (currentfile == 1)
            scale -= 0.0005;
        if (currentfile == 2)
            scale -= 0.001;
        if (currentfile == 5) {
            scale -= 0.1;
        }
        else
            scale -= 0.001;
    }
    glutPostRedisplay();
}

// 鼠标运动时
void onMouseMove(int x, int y) {
    if (mousetate) {
        yRotate += x - Oldx;
        glutPostRedisplay();
        Oldx = x;
        xRotate += y - Oldy;
        glutPostRedisplay();
        Oldy = y;
    }
}

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(xRotate, 1.0f, 0.0f, 0.0f); 
    glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
    //glTranslatef(0.0f, 0.0f, ty);
    glTranslatef(tx, ty, 0);        
    glScalef(scale, scale, scale); 

    if (showFace)
        glCallList(showFaceList);
    if (showFlatlines) {
        glCallList(showFaceList);
        glCallList(showWireList);
    }
    if (showWire)
        glCallList(showWireList);

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("Mesh Viewer");
    glutMouseFunc(myMouse);        
    glutMotionFunc(onMouseMove);
    glutSpecialFunc(&mySpecial);
    glutReshapeFunc(&myReshape);
    glutDisplayFunc(&myDisplay);

    glutMainLoop();
    return 0;
}