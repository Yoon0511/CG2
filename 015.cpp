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

#define MY_WIDTH	800
#define MY_HEIGHT	800
#define RECT_SIZE	0.1

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);

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

GLuint vao[3], vbo[6];
GLuint s_program;
float rect_size = RECT_SIZE;

typedef struct my_Vectex {
	float x, y, z;
};
typedef struct my_Rectpos {
	float x, y;
};


class myRect {
public:
	float xangle, yangle;
	float movex, movey;
	GLUquadricObj* qobj;
	float glux, gluy,gluanglex,gluangley;
	bool rotate[4], glurotate[4];
	bool revolution[3],chage;
	float glury, ry;
	float rotatingspeed;
public:
	myRect() {
		for (int i = 0; i < 4; ++i)
		{
			rotate[i] = glurotate[i] = false;
		}
		for (int i = 0; i < 3; ++i)
		{
			revolution[i] = false;
		}
		chage = true;
		rotatingspeed = 1.5;
		glury = 0;
		ry = 0;
		xangle = -30.0;
		yangle = -15.0;
		movex = -0.5f;
		movey = 0.5f;
		glux = 0.5f;
		gluy = -0.5f;
		gluanglex = -30.0;
		gluangley = -15.0;
	};
	GLfloat hexahedronp[108] = {
		  0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25,	//��
		  0.25,0.25,0.0, 0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,0.25,0.0, 0.25,0.25,0.0,	//��
		  0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,0.0, 0.25,-0.25,0.0, 0.25,0.25,0.0, 0.25,0.25,0.25,		//��
		 -0.25,0.25,0.25, -0.25,0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.25, -0.25,0.25,0.25,	//��
		  0.25,0.25,0.25, 0.25,0.25,0.0, -0.25,0.25,0.0, -0.25,0.25,0.0, -0.25,0.25,0.25, 0.25,0.25,0.25,		//��
		  0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.0, -0.25,-0.25,0.0, 0.25,-0.25,0.0, 0.25,-0.25,0.25	//�Ʒ�
	};

	const GLfloat colors[108] = { //--- �ﰢ�� ������ ����
		1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, //��
		1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0,//��
		1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0,//��
		1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0,//��
		1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0,//��
		1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0//�Ʒ�
	};

	const GLfloat lines[18] = {
		-0.8,0.0,0.0, 0.8,0.0,0.0,
		0.0,0.8,0.0, 0.0,-0.8,0.0,
		-0.8,-0.8,0.0, 0.8,0.8,0.0,
	};

	const GLfloat lines_color[18] = {
		1.0,0.0,0.0, 1.0,0.0,0.0,
		0.0,1.0,0.0, 0.0,1.0,0.0,
		0.0,0.0,1.0, 0.0,0.0,1.0,
	};

	//GLfloat tri[36] = {
	//	-0.5,0.0,0.0, 0.5,0.0,0.0, 0.0,0.5,0.0,  0.0,0.0,0.5, 0.5,0.0,0.0, 0.0,0.5,0.0, //�� ��
	//	-0.5,0.0,0.0, 0.0,0.0,0.5, 0.0,0.5,0.0,  0.0,0.0,0.5, -0.5,0.0,0.0, 0.5,0.0,0.0 //�� 
	//};

	GLfloat tri[54] = {
		-0.3,0.0,0.0, 0.3,0.0,-0.3, -0.3,0.0,-0.3,  -0.3,0.0,0.0, 0.3,0.0,0.0, 0.3,0.0,-0.3, //�ٴ�
		-0.3,0.0,0.0, 0.3,0.0,0.0, 0.0,0.3,0.15,  0.3,0.0,0.0, 0.3,0.0,-0.3, 0.0,0.3,0.15, //�� ��
		-0.3,0.0,-0.3, 0.3,0.0,-0.3, 0.0,0.3,0.15,  -0.3,0.0,0.0, -0.3,0.0,-0.3, 0.0,0.3,0.15, //�� ��
	};

	GLfloat tri_colors[54] = {
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0,  1.0,0.0,1.0, 1.0,0.0,1.0, 1.0,0.0,1.0,
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0,  0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0,
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0,  0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0,
	};

public:
	void mygluDraw_Sphere()
	{
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_LINE);
		gluSphere(qobj, 0.3, 20, 20);
	}
	void mygluDraw_Cylinder()
	{
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_LINE);
		gluCylinder(qobj, 0.3, 0.3, 0.3,20,8);
	}

	void myglusetting()
	{
		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Ty = glm::mat4(1.0f);
		glm::mat4 moveMatrix = glm::mat4(1.0f);
		glm::mat4 mmoveMatrix = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		moveMatrix = glm::translate(moveMatrix, glm::vec3(glux, gluy , 0.0f));
		Tx = glm::rotate(Tx, glm::radians(gluanglex), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(gluangley), glm::vec3(0.0f, 1.0f, 0.0f));
		mmoveMatrix = glm::translate(mmoveMatrix, glm::vec3(-glux,-gluy,0.0f));

		glm::mat4 Ry = glm::mat4(1.0f);
		glm::mat4 Rx = glm::mat4(1.0f);
		Ry = glm::rotate(Ry, glm::radians(glury), glm::vec3(0.0f, 1.0f, 0.0f));
		Rx = glm::rotate(Rx, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = Rx * Ry;
		model *= moveMatrix * Tx * Ty;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	}

	void mydraw_hex()
	{
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void mydraw_tri()
	{
		glBindVertexArray(vao[2]);
		glDrawArrays(GL_TRIANGLES, 0, 18);
	}

	void myrectsetting()
	{
		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Ty = glm::mat4(1.0f);
		glm::mat4 moveMatrix = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mmoveMatrix = glm::mat4(1.0f);

		moveMatrix = glm::translate(moveMatrix, glm::vec3(movex,movey, 0.0f));
		Tx = glm::rotate(Tx, glm::radians(xangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(yangle), glm::vec3(0.0f, 1.0f, 0.0f));
		mmoveMatrix = glm::translate(mmoveMatrix, glm::vec3(-movex, -movey, 0.0f));

		glm::mat4 Ry = glm::mat4(1.0f);
		glm::mat4 Rx = glm::mat4(1.0f);
		Ry = glm::rotate(Ry, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
		Rx = glm::rotate(Rx, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = Rx * Ry;
		model *= moveMatrix * Tx * Ty;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	}

	void add_xangle(float angle)
	{
		xangle += angle;
	}

	void add_yangle(float angle)
	{
		yangle += angle;
	}

	void setrectroation(bool r1, bool r2, bool r3, bool r4)
	{
		rotate[0] = r1;
		rotate[1] = r2;
		rotate[2] = r3;
		rotate[3] = r4;
	}
	void setgluroation(bool r1, bool r2, bool r3, bool r4)
	{
		glurotate[0] = r1;
		glurotate[1] = r2;
		glurotate[2] = r3;
		glurotate[3] = r4;
	}
	void add_glu_xangle(float angle)
	{
		gluanglex += angle;
	}

	void add_glu_yangle(float angle)
	{
		gluangley += angle;
	}

	void set_revolution(bool r,bool dir)
	{
		revolution[0] = r;
		revolution[1] = r;
		revolution[2] = dir;
	}

	void myrevolution(float angle)
	{
		ry += angle;
		glury += angle;
	}
	void Rectconversion()
	{
		chage = !chage;
	}
};

myRect myrect;

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

	InitShader();
	InitBuffer();

	glutTimerFunc(0.01f, mytimer, 0);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glUseProgram(s_program);
	//xy��
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glBindVertexArray(vao[1]);
	Ty = glm::rotate(Ty, glm::radians(myrect.glury), glm::vec3(0.0f, 1.0f, 0.0f));
	model *= Ty;
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	glDrawArrays(GL_LINES, 0, 6);

	glEnable(GL_DEPTH_TEST);

	myrect.myrectsetting();
	if (myrect.chage)
	{
		myrect.mydraw_hex();
	}
	else
	{
		myrect.mydraw_tri();
	}
	
	//obj
	myrect.myglusetting();
	if (myrect.chage)
	{
		myrect.mygluDraw_Sphere();
	}
	else
	{
		myrect.mygluDraw_Cylinder();
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�

}

void set_rotate_triangle()
{
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 moveMatrix = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	moveMatrix = glm::translate(moveMatrix, glm::vec3(myrect.movex, myrect.movey, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(myrect.xangle), glm::vec3(1.0f, 0.0f, 0.0f));
	Ty = glm::rotate(Ty, glm::radians(myrect.yangle), glm::vec3(0.0f, 1.0f, 0.0f));

	model = Tx * Ty * moveMatrix;

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}
void make_vertexShaders()
{
	GLchar* vertexsource;
	vertexsource = filetobuf("002v.glsl");
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
	fragmentsource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
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
		std::cerr << "ERROR: vertex shader error\n" << errorLog << std::endl;
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
	glGenVertexArrays(3, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(6, vbo);

	//����ü vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hexahedronp), myrect.hexahedronp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����ü color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.colors), myrect.colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[1]);
	//xy�� vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines), myrect.lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//xy�� color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines_color), myrect.lines_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[2]);
	//���ü
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri), myrect.tri, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//xy�� color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_colors), myrect.tri_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
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

	uniformModel = glGetUniformLocation(s_program, "model");

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
	case 'x': // X - ���� ����
		myrect.setrectroation(true, false, false, false);
		break;
	case 'X': // X - ���� ����
		myrect.setrectroation(false, true, false, false);
		break;
	case 'y': // Y - ���� ����
		myrect.setrectroation(false, false, true, false);
		break;
	case 'Y': // Y - ���� ����
		myrect.setrectroation(false, false, false, true);
		break;
	case 'a':
		myrect.setgluroation(true, false, false, false);
		break;
	case 'A':
		myrect.setgluroation(false, true, false, false);
		break;
	case 'b':
		myrect.setgluroation(false, false, true, false);
		break;
	case 'B':
		myrect.setgluroation(false, false, false, true);
		break;
	case 'r':
		myrect.set_revolution(true, true);
		break;
	case 'R':
		myrect.set_revolution(true, false);
		break;
	case 'c':
	case 'C':
		myrect.Rectconversion();
		break;
	case 's':
	case 'S':
		myrect.setrectroation(false, false, false, false);
		myrect.setgluroation(false, false, false, false);
		myrect.set_revolution(false, false);
		break;
	}
	glutPostRedisplay();
}

GLvoid Special(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		myrect.movex -= 0.01f;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		myrect.movex += 0.01f;
	}
	else if (key == GLUT_KEY_UP)
	{
		myrect.movey += 0.01f;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		myrect.movey -= 0.01f;
	}
	glutPostRedisplay();
}

void mytimer(int value)
{
	if (myrect.rotate[0]) //�簢�� x�� ���� ���� ����
	{
		myrect.add_xangle(myrect.rotatingspeed);
	}
	if (myrect.rotate[1]) //�簢�� x�� ���� ���� ����
	{
		myrect.add_xangle(-myrect.rotatingspeed);
	}
	if (myrect.rotate[2]) //�簢�� y�� ���� ���� ����
	{
		myrect.add_yangle(myrect.rotatingspeed);
	}
	if (myrect.rotate[3]) //�簢�� y�� ���� ���� ����
	{
		myrect.add_yangle(-myrect.rotatingspeed);
	}
	//glu
	if (myrect.glurotate[0]) // X - ���� ���� ����
	{
		myrect.add_glu_xangle(myrect.rotatingspeed);
	}
	if (myrect.glurotate[1]) // X - ���� ���� ����
	{
		myrect.add_glu_xangle(-myrect.rotatingspeed);
	}
	if (myrect.glurotate[2]) // Y - ���� ���� ����
	{
		myrect.add_glu_yangle(myrect.rotatingspeed);
	}
	if (myrect.glurotate[3]) // Y - ���� ���� ����
	{
		myrect.add_glu_yangle(-myrect.rotatingspeed);
	}

	//����
	if (myrect.revolution[0] && myrect.revolution[1])
	{
		if (myrect.revolution[2])
		{
			myrect.myrevolution(0.3f);
			//myrect.ry += 0.3f;
		}
		else
		{
			myrect.myrevolution(-0.3f);
			//myrect.ry -= 0.3f;
		}
	}
	

	glutPostRedisplay();
	glutTimerFunc(1.0f, mytimer, 0);
}