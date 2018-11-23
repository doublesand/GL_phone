#include "include/Angel.h"
#include "include/TriMesh.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <iostream>

using namespace std;

//各种需要的标识符
GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexNormalID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint vNormalID;
GLuint modelViewMatrixID;
GLuint modelViewProjMatrixID;

GLuint lightPosID;

//专门读取OFF文件的类
TriMesh* mesh = new TriMesh();
//光源位置
vec3 lightPos(0.0, 0.0, 2.0);

//照相机系数，由于这是讲光照的，不涉及视角变换
namespace Camera
{
    mat4 modelMatrix(1.0);
    mat4 viewMatrix(1.0);
    mat4 projMatrix(1.0);
}

// 初始化函数
void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //黑色背景

	programID = InitShader("vshader_vert.glsl", "fshader_vert.glsl");  //读入着色器

	// 从顶点着色器和片元着色器中获取变量的位置
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vNormalID = glGetAttribLocation(programID, "vNormal");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	modelViewProjMatrixID = glGetUniformLocation(programID, "modelViewProjMatrix");
	lightPosID = glGetUniformLocation(programID, "lightPos");

	// 利用mesh类读取外部三维模型，真是方便多了啊
	mesh->read_off("sphere_coarse.off");

	vector<vec3f> vs = mesh->v();  //顶点
	vector<vec3i> fs = mesh->f();  //片面
	vector<vec3f> ns;              //顶点法向量数组

	// 计算球模型在每个顶点的法向量，顶点位置-眼睛位置
	for (int i = 0; i < vs.size(); ++i) {
		ns.push_back(vs[i] - vec3(0.0, 0.0, 0.0));
	}

	// 生成VAO（顶点数组对象）
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 生成VBO（顶点缓存对象），并绑定顶点数据
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// 生成VBO，并绑定法向量数据
	glGenBuffers(1, &vertexNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(vec3f), ns.data(), GL_STATIC_DRAW);

	// 生成VBO，并绑定顶点索引
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	// OpenGL相应状态设置
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);  
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

// 渲染
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// 计算相机观察矩阵和投影矩阵，并传入顶点着色器
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mat4 modelViewProjMatrix = Camera::projMatrix * modelViewMatrix;

	// 将投影变化矩阵、相机变换矩阵传入着色器中
	// 注意：为了方便计算，光源位置设置在相机坐标系中
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, modelViewProjMatrix);

	// 将光源位置传入顶点着色器
	glUniform3fv(lightPosID, 1, lightPos);

	glEnableVertexAttribArray(vPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		vPositionID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(vNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glVertexAttribPointer(
		vNormalID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);  //自动调整窗口大小
}

// 鼠标响应函数

void mouse(int x, int y)
{
	// 用鼠标控制光源的位置lightPos，以实时更新光照效果
	// 窗口坐标系与图像坐标系转换，像素体与视景体之间的转换
	float half_winx = 500.0 / 2.0;   //求中点
	float half_winy = 500.0 / 2.0;
	float lx = float(x - half_winx) / half_winx;
	float ly = float(500.0 - y - half_winy) / half_winy;
	lightPos = vec3(lx, ly, 2.0);
}

// 键盘响应函数
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:	// ESC键 和 'q' 键退出游戏
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

//回调重绘函数
void idle(void)
{
	glutPostRedisplay();
}

//清除函数，删除没用的缓存之类的
void clean()
{
	glDeleteBuffers(1, &vertexBufferID); //删除缓存
	glDeleteProgram(programID);          //删除顶点和片面着色器
	glDeleteVertexArrays(1, &vertexArrayID); //删除顶点数组

	if (mesh) {  //删除OFF类对象
		delete mesh;
		mesh = NULL;
	}
}

//主函数
int main(int argc, char **argv)
{
	glutInit(&argc, argv); //初始化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("顶点着色器");

	glewInit();   //glew初始化
	init();       //数据初始化

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMotionFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}