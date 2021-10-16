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
GLuint vao[4], vbo[8];
GLuint s_program;
float rect_size = RECT_SIZE;

class myRect {
public:
	GLUquadricObj* qobj;
	float xangle, yangle;
	float movex, movey,movez,scale_x,scale_y,scale_z,scalebyzero;
	float glux, gluy, gluz,gluscale_x, gluscale_y,gluscale_z, gluscalebyzero;
	float gluanglex, gluangley;
	float movespeed;
	float prev_x, prev_y,prev_z;
	bool moving;
	int move_index;

public:
	myRect() {
		movespeed = 0.05;
		xangle = 10.0;
		yangle = -45.0;
		movex = -0.5f;
		movey = 0.5f;
		movez = 0.0f;
		glux = 0.5f;
		gluy = -0.7f;
		gluz = 0.0f;
		gluanglex = -30.0;
		gluangley = -15.0;
		scale_x = scale_y = scale_z = gluscale_x = gluscale_y = gluscale_z = -0.3;
		scalebyzero = gluscalebyzero = 1;
		prev_x = prev_z = prev_y = 0;
		move_index = 0;
		moving = false;
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

	/*const GLfloat lines[18] = {
		-0.8,0.0,0.0, 0.8,0.0,0.0,
		0.0,0.8,0.0, 0.0,-0.8,0.0,
		0.5,-0.5,0.5, -0.5,0.5,-0.5,
	};*/

	const GLfloat lines[18] = {
		-0.8,0.0,0.0, 0.8,0.0,0.0,
		0.0,0.8,0.0, 0.0,-0.8,0.0,
		0.0,0.0,0.8, 0.0,0.0,-0.8,
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

	GLfloat move_lines[7560];
	GLfloat move_lines_color[3] = {
		0.5,0.5,0.5
	};

public:
	void mygluDraw_Sphere()
	{
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_LINE);
		gluSphere(qobj, 0.3, 20, 20);
	}
	
	void myglusetting()
	{
		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Ty = glm::mat4(1.0f);
		glm::mat4 moveMatrix = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);
		glm::mat4 scalezero = glm::mat4(1.0f);

		moveMatrix = glm::translate(moveMatrix, glm::vec3(glux, gluy, gluz));
		Tx = glm::rotate(Tx, glm::radians(gluanglex), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(gluangley), glm::vec3(0.0f, 1.0f, 0.0f));
		scale = glm::scale(scale, glm::vec3(gluscale_x, gluscale_y, gluscale_z));
		scalezero = glm::scale(scalezero, glm::vec3(gluscalebyzero, gluscalebyzero, gluscalebyzero));

		//model = scalezero * moveMatrix * Tx * Ty * scale; //����z���̵�
		model = scalezero * Tx * Ty * moveMatrix * scale; //����z�� �̵�

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	}

	void mydraw_hex()
	{
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void myrectsetting()
	{
		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Ty = glm::mat4(1.0f);
		glm::mat4 moveMatrix = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);
		glm::mat4 scalezero = glm::mat4(1.0f);


		moveMatrix = glm::translate(moveMatrix, glm::vec3(movex, movey, movez));
		Tx = glm::rotate(Tx, glm::radians(xangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(yangle), glm::vec3(0.0f, 1.0f, 0.0f));
		scale = glm::scale(scale, glm::vec3(scale_x, scale_y, scale_z));
		scalezero = glm::scale(scalezero, glm::vec3(scalebyzero, scalebyzero, scalebyzero));

		//model = scalezero * moveMatrix * Tx * Ty * scale; //����z���̵�
		model = scalezero * Tx * Ty * moveMatrix *scale; //����z�� �̵�

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	}

	void hex_move(float x, float y, float z)
	{
		movex += x;
		movey += y;
		movez += z;
	}

	void glu_move(float x, float y, float z)
	{
		glux += x;
		gluy += y;
		gluz += z;
	}

	void hex_scale(float x, float y, float z)
	{
		scale_x += x;
		scale_y += y;
		scale_z += z;
	}

	void glu_scale(float x, float y, float z)
	{
		gluscale_x += x;
		gluscale_y += y;
		gluscale_z += z;
	}

	void hex_scalebyzero(float scale)
	{
		scalebyzero += scale;
	}

	void glu_scalebyzero(float scale)
	{
		gluscalebyzero += scale;
	}

	void reset()
	{
		xangle = 10.0;
		yangle = -45.0;
		movex = -0.5f;
		movey = 0.5f;
		movez = 0.0f;
		glux = 0.5f;
		gluy = -0.7f;
		gluz = 0.0f;
		gluanglex = -30.0;
		gluangley = -15.0;
		scale_x = scale_y = scale_z = gluscale_x = gluscale_y = gluscale_z = -0.3;
		scalebyzero = gluscalebyzero = 1;
		prev_x = prev_z = prev_y = 0;
		move_index = 0;
		moving = false;
	}

	void setline()
	{
		float rad = 0;
		//glBegin(GL_LINE_STRIP);
		//2520
		//7,560
		float dx[2528];
		float dz[2528];

		for (int i = 0; i < 360 * 7; i++) {
			double angle, x, y;
			angle = i * 3.141592 / 180;
			rad += 0.0004f;
			x = rad * cos(angle);
			y = rad * sin(angle);
			dx[i] = x;
			dz[i] = y;
			//glVertex3f(x + -0.5, 0.7,y);
		}
		//glEnd();
		//glFinish();

		int x = 0, y = 0, z = 0;
		for (int i = 0; i < 7560; ++i)
		{
			int k = i % 3;
			switch (k)
			{
			case 0:
				move_lines[i] = dx[x];
				x++;
				break;
			case 1:
				move_lines[i] = 0.0f;
				break;
			case 2:
				move_lines[i] = dz[z];
				z++;
				break;
			}
		}

		/*int c = 0;
		for (int i = 0; i < 7560; ++i)
		{
			printf("%d %f ", i,move_lines[i]);
			c++;
			if(c == 3)
			{
				c = 0;
				printf("\n");
			}
		}*/
	}

	void hex_moving()
	{
		if (moving)
		{
			prev_x = movex;
			prev_z = movez;
			prev_z = movey;
			//int i = 0;
			//while (true)
			//{
			//	i = i++ % 7558; //7560 - 2
			//	movex = move_lines[i];
			//	movez = move_lines[i + 2];
			//}
		}
		else
		{
			movex = - movex + prev_x;
			movez = - movez + prev_z;
			movey = - movey + prev_y;
		}
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

	myrect.setline();

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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glUseProgram(s_program);
	glEnable(GL_DEPTH_TEST);

	//xy��
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 moveMatrix = glm::mat4(1.0f);
	moveMatrix = glm::translate(moveMatrix, glm::vec3(0, 0, 0));
	Ty = glm::rotate(Ty, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = Tx * Ty * moveMatrix;
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_LINES, 0, 6);

	Ty = glm::rotate(Ty, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = Tx * Ty * moveMatrix;
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(vao[3]);
	glDrawArrays(GL_LINE_STRIP, 0, 2520);

	//�簢��
	myrect.myrectsetting();
	myrect.mydraw_hex();

	//obj
	myrect.myglusetting();
	myrect.mygluDraw_Sphere();
	
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
	glGenVertexArrays(4, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(8, vbo);

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
	//���ü color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_colors), myrect.tri_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//ȸ�� line
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.move_lines), myrect.move_lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//ȸ�� line color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.move_lines_color), myrect.move_lines_color, GL_STATIC_DRAW);
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
	case 'w':
		myrect.hex_move(0, myrect.movespeed, 0);
		break;
	case 'a':
		myrect.hex_move(-myrect.movespeed,0, 0);
		break;
	case 's':
		myrect.hex_move(0, -myrect.movespeed, 0);
		break;
	case 'd':
		myrect.hex_move(myrect.movespeed, 0, 0);
		break;
	case 'q':
		myrect.hex_move(0, 0, myrect.movespeed);
		break;
	case 'e':
		myrect.hex_move(0, 0, -myrect.movespeed);
		break;
	case 'i':
		myrect.glu_move(0, myrect.movespeed, 0);
		break;
	case 'j':
		myrect.glu_move(-myrect.movespeed, 0, 0);
		break;
	case 'k':
		myrect.glu_move(0, -myrect.movespeed, 0);
		break;
	case 'l':
		myrect.glu_move(myrect.movespeed,0,0);
		break;
	case 'u':
		myrect.glu_move(0, 0, myrect.movespeed);
		break;
	case 'o':
		myrect.glu_move(0, 0, -myrect.movespeed);
		break;
	case '1':
		myrect.hex_scale(0.01f, 0.01f, 0.01f);
		break;
	case '2':
		myrect.hex_scale(-0.01f, -0.01f, -0.01f);
		break;
	case '3':
		myrect.glu_scale(0.01f, 0.01f, 0.01f);
		break;
	case '4':
		myrect.glu_scale(-0.01f, -0.01f, -0.01f);
		break;
	case '5':
		myrect.hex_scalebyzero(0.01f);
		break;
	case '6':
		myrect.hex_scalebyzero(-0.01f);
		break;
	case '7':
		myrect.glu_scalebyzero(0.01f);
		break;
	case '8':
		myrect.glu_scalebyzero(-0.01f);
		break;
	case 'z':
		myrect.hex_move(-myrect.movespeed, 0, 0);
		myrect.glu_move(-myrect.movespeed, 0, 0);
		break;
	case 'x':
		myrect.hex_move(myrect.movespeed, 0, 0);
		myrect.glu_move(myrect.movespeed, 0, 0);
		break;
	case 'c':
		myrect.hex_move(0, 0, myrect.movespeed);
		myrect.glu_move(0, 0, myrect.movespeed);
		break;
	case 'v':
		myrect.hex_move(0, 0, -myrect.movespeed);
		myrect.glu_move(0, 0, -myrect.movespeed);
		break;
	case 'g':
		myrect.reset();
		break;
	case 'r':
		myrect.moving = !myrect.moving;
		myrect.hex_moving();
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
	if (myrect.moving)
	{
		//myrect.move_index = myrect.move_index % 7558; //7560 - 2 , 2520 - 2
		myrect.movex = myrect.move_lines[myrect.move_index];
		myrect.movey = 0;
		myrect.movez = myrect.move_lines[myrect.move_index + 2];
		myrect.move_index += 3;

		if (myrect.move_index >= 7558)
		{
			myrect.move_index = 0;
		/*	myrect.movex = -myrect.movex;
			myrect.movey = -myrect.movex;
			myrect.movez = -myrect.movex;*/
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1.0f, mytimer, 0);
}