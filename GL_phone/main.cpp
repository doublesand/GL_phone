#include "include/Angel.h"
#include "include/TriMesh.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <iostream>

using namespace std;

//������Ҫ�ı�ʶ��
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

//ר�Ŷ�ȡOFF�ļ�����
TriMesh* mesh = new TriMesh();
//��Դλ��
vec3 lightPos(0.0, 0.0, 2.0);

//�����ϵ�����������ǽ����յģ����漰�ӽǱ任
namespace Camera
{
    mat4 modelMatrix(1.0);
    mat4 viewMatrix(1.0);
    mat4 projMatrix(1.0);
}

// ��ʼ������
void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //��ɫ����

	programID = InitShader("vshader_vert.glsl", "fshader_vert.glsl");  //������ɫ��

	// �Ӷ�����ɫ����ƬԪ��ɫ���л�ȡ������λ��
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vNormalID = glGetAttribLocation(programID, "vNormal");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	modelViewProjMatrixID = glGetUniformLocation(programID, "modelViewProjMatrix");
	lightPosID = glGetUniformLocation(programID, "lightPos");

	// ����mesh���ȡ�ⲿ��άģ�ͣ����Ƿ�����˰�
	mesh->read_off("sphere_coarse.off");

	vector<vec3f> vs = mesh->v();  //����
	vector<vec3i> fs = mesh->f();  //Ƭ��
	vector<vec3f> ns;              //���㷨��������

	// ������ģ����ÿ������ķ�����������λ��-�۾�λ��
	for (int i = 0; i < vs.size(); ++i) {
		ns.push_back(vs[i] - vec3(0.0, 0.0, 0.0));
	}

	// ����VAO�������������
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ����VBO�����㻺����󣩣����󶨶�������
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// ����VBO�����󶨷���������
	glGenBuffers(1, &vertexNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(vec3f), ns.data(), GL_STATIC_DRAW);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	// OpenGL��Ӧ״̬����
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);  
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

// ��Ⱦ
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// ��������۲�����ͶӰ���󣬲����붥����ɫ��
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mat4 modelViewProjMatrix = Camera::projMatrix * modelViewMatrix;

	// ��ͶӰ�仯��������任��������ɫ����
	// ע�⣺Ϊ�˷�����㣬��Դλ���������������ϵ��
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, modelViewProjMatrix);

	// ����Դλ�ô��붥����ɫ��
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

// �������ô���
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);  //�Զ��������ڴ�С
}

// �����Ӧ����

void mouse(int x, int y)
{
	// �������ƹ�Դ��λ��lightPos����ʵʱ���¹���Ч��
	// ��������ϵ��ͼ������ϵת�������������Ӿ���֮���ת��
	float half_winx = 500.0 / 2.0;   //���е�
	float half_winy = 500.0 / 2.0;
	float lx = float(x - half_winx) / half_winx;
	float ly = float(500.0 - y - half_winy) / half_winy;
	lightPos = vec3(lx, ly, 2.0);
}

// ������Ӧ����
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:	// ESC�� �� 'q' ���˳���Ϸ
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

//�ص��ػ溯��
void idle(void)
{
	glutPostRedisplay();
}

//���������ɾ��û�õĻ���֮���
void clean()
{
	glDeleteBuffers(1, &vertexBufferID); //ɾ������
	glDeleteProgram(programID);          //ɾ�������Ƭ����ɫ��
	glDeleteVertexArrays(1, &vertexArrayID); //ɾ����������

	if (mesh) {  //ɾ��OFF�����
		delete mesh;
		mesh = NULL;
	}
}

//������
int main(int argc, char **argv)
{
	glutInit(&argc, argv); //��ʼ��
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("������ɫ��");

	glewInit();   //glew��ʼ��
	init();       //���ݳ�ʼ��

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMotionFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}