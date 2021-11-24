#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
#include <random>
#include <time.h>
#include <math.h>
#include <list>
using namespace std;

#define MY_WIDTH	800
#define MY_HEIGHT	800
#define RECT_SIZE	0.1

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);

typedef struct my_Vectex {
	float x, y, z;
};
typedef struct my_Rectpos {
	float x, y;
};
GLuint shaderID;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
void InitShader();
void set_rotate_triangle();
void convertWINXYOpenGlXY(int x, int y, float* ox, float* oy);
void repostion_triangleVertex(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Special(int key, int x, int y);
GLuint vertexShader;
GLuint fragmentShader;
GLuint uniformModel;
float r = 0;
float offset = 0.0f;
float trilncrment = 0.01f;
void mytimer(int value);
GLuint vao[16], vbo[30];
GLuint s_program;
float rect_size = RECT_SIZE;

float SphereObj[1000][18];
int Shpherindex = 0;
float light_angle = 0;

int ReadObj(FILE* objFile)
{
	char count[100], bind[100];
	int vertexNum = 0;
	int faceNum = 0;

	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		//printf("%s\n", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexNum += 1;
		else if (count[0] == 'f' && count[1] == '\0')
			faceNum += 1;
		memset(count, '\0', sizeof(count));
	}
	//printf("vertexNum:%d faceNum:%d\n", vertexNum, faceNum);
	vertexNum += 1;
	glm::vec3* vertex;
	glm::vec3* normal;
	glm::uvec3* v, * t, * n;
	int vertIndex = 1;
	int normIndex = 1;
	int faceIndex = 0;
	v = (glm::uvec3*)malloc(sizeof(glm::vec3) * faceNum);
	t = (glm::uvec3*)malloc(sizeof(glm::vec3) * faceNum);
	n = (glm::uvec3*)malloc(sizeof(glm::vec3) * faceNum);
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNum);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNum);
	rewind(objFile);
	while (!feof(objFile)) {
		fscanf(objFile, "%s", bind);
		//printf("%s        count:%d\n", bind, a++);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(objFile, "%f %f %f",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);
			//printf("vertex[%d].x=%f\n", vertIndex, vertex[vertIndex].x);
			vertIndex++;
		}
		else if (bind[0] == 'v' && bind[1] == 'n') {
			fscanf(objFile, "%f %f %f",
				&normal[normIndex].x, &normal[normIndex].y, &normal[normIndex].z);
			//printf("normal[%d].x=%f\n", normIndex, normal[normIndex].x);
			normIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&v[faceIndex].x, &t[faceIndex].x, &n[faceIndex].x, &v[faceIndex].y, &t[faceIndex].y, &n[faceIndex].y, &v[faceIndex].z, &t[faceIndex].z, &n[faceIndex].z);
			//printf("%d/%d/%d %d/%d/%d %d/%d/%d\n", v[faceIndex].x, t[faceIndex].x, n[faceIndex].x, v[faceIndex].y, t[faceIndex].y, n[faceIndex].y, v[faceIndex].z, t[faceIndex].z, n[faceIndex].z);
			//printf("faceIndex:%d\n", faceIndex);
			faceIndex++;
		}
	}

	//printf("VertexN:%d , faceIndexN:%d\n", vertIndex, faceIndex);

	for (int i = 0; i < faceIndex; i++) {
		SphereObj[i][0] = vertex[v[i].x].x;
		SphereObj[i][1] = vertex[v[i].x].y;
		SphereObj[i][2] = vertex[v[i].x].z;
		SphereObj[i][3] = normal[n[i].x].x;
		SphereObj[i][4] = normal[n[i].x].y;
		SphereObj[i][5] = normal[n[i].x].z;

		SphereObj[i][6] = vertex[v[i].y].x;
		SphereObj[i][7] = vertex[v[i].y].y;
		SphereObj[i][8] = vertex[v[i].y].z;
		SphereObj[i][9] = normal[n[i].y].x;
		SphereObj[i][10] = normal[n[i].y].y;
		SphereObj[i][11] = normal[n[i].y].z;

		SphereObj[i][12] = vertex[v[i].z].x;
		SphereObj[i][13] = vertex[v[i].z].y;
		SphereObj[i][14] = vertex[v[i].z].z;
		SphereObj[i][15] = normal[n[i].z].x;
		SphereObj[i][16] = normal[n[i].z].y;
		SphereObj[i][17] = normal[n[i].z].z;
	}
	//for (int i = 0; i < faceIndex; i++) {
	//    printf("faceIndex:%d\nver:%f %f %f\nnor:%f %f %f\n\n", i, SphereObj[i][0], SphereObj[i][1], SphereObj[i][2], SphereObj[i][3], SphereObj[i][4], SphereObj[i][5]);
	//}
	return faceIndex;
}


void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(MY_WIDTH, MY_HEIGHT); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	FILE* sphere = fopen("sphere.obj", "rb");
	if (sphere == NULL) {
		printf("File open failed...");
		exit(1);
	}
	Shpherindex = ReadObj(sphere);
	fclose(sphere);

	InitShader();
	InitBuffer();

	glutTimerFunc(1.0f, mytimer, 0);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(0, 0, 0, 0); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glUseProgram(s_program);

	glEnable(GL_DEPTH_TEST);

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� ��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	//����
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -7.0)); //--- ������ �ణ �ڷ� �̷���
	projection = glm::rotate(projection, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	projection = glm::rotate(projection, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	//xy��
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Rm = glm::mat4(1.0f);
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 mo = glm::mat4(1.0f);
	glm::mat4 moveMatrix = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 init = glm::mat4(1.0f);

	model = init;

	moveMatrix = glm::translate(moveMatrix, glm::vec3(0,0,0));
	Ty = glm::rotate(Ty, glm::radians(light_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	int lightmodelLocation = glGetUniformLocation(s_program, "lightmodel");
	model = Ty;
	glUniformMatrix4fv(lightmodelLocation, 1, GL_FALSE, glm::value_ptr(model));

	int lightPosLocation = glGetUniformLocation(s_program, "lightPos");
	glUniform3f(lightPosLocation, 0.0f, 0.0f, 15.0f);

	int lightColorLocation = glGetUniformLocation(s_program, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	int objColorLocation = glGetUniformLocation(s_program, "objectColor");
	glUniform3f(objColorLocation, 1.0, 0.0, 0.0);

	glBindVertexArray(vao[12]);
	
	model = init;
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	for (int i = 0; i < Shpherindex; ++i)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}

	glUniform3f(objColorLocation, 0.0, 1.0, 0.0);
	model = init;
	model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
	model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	for (int i = 0; i < Shpherindex; ++i)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}


	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}
void make_vertexShaders()
{
	GLchar* vertexsource;
	vertexsource = filetobuf("light_vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader error\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentsource;
	fragmentsource = filetobuf("light_fragment.glsl"); // �����׼��̴� �о����
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader error\n" << errorLog << std::endl;
		return;
	}

}

GLuint make_shaderProgram()
{
	GLuint ShaderProgramID = glCreateProgram(); //--- ���̴� ���α׷� �����
	glAttachShader(ShaderProgramID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(ShaderProgramID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(ShaderProgramID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ���α׷��� ��ũ�Ͽ� ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}

	return ShaderProgramID;
}

void InitBuffer()
{
	//�ܺ� �����̴� �ﰢ��
	glGenVertexArrays(14, vao);
	glBindVertexArray(vao[0]); //�ﰢ�� ��

	glGenBuffers(27, vbo);

	glBindVertexArray(vao[12]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[24]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SphereObj), SphereObj, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void InitShader()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	uniformModel = glGetUniformLocation(s_program, "transform");

	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program ����ϱ�
	glUseProgram(s_program);

}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

void convertWINXYOpenGlXY(int x, int y, float* ox, float* oy)
{
	int w = MY_WIDTH;
	int h = MY_HEIGHT;

	*ox = (float)((x - (float)(w / 2.0)) * (float)(1.0 / (float)(w / 2.0)));
	*oy = -(float)((y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0)));
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		light_angle += 5.0f;
		break;
	}
	glutPostRedisplay();
}

GLvoid Special(int key, int x, int y)
{

	glutPostRedisplay();
}

void mytimer(int value)
{

	glutPostRedisplay();
	glutTimerFunc(10.0f, mytimer, 0);
}