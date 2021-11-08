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
void mytimer(int value);
GLuint vertexShader;
GLuint fragmentShader;
GLuint uniformModel;
GLuint vao[403], vbo[810];
GLuint s_program;
void my_draw(int type);

class Mymountain {
public:
	GLfloat mountains[108] = {
		  0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25,				//��
		  0.25,0.25,-0.25, 0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,0.25,-0.25, 0.25,0.25,-0.25,		//��
		  0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,0.25,-0.25, 0.25,0.25,0.25,				//��
		 -0.25,0.25,0.25, -0.25,0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,0.25, -0.25,0.25,0.25,		//��
		  0.25,0.25,0.25, 0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,0.25, 0.25,0.25,0.25,				//��
		  0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,-0.25,0.25		//�Ʒ�
	};

	const GLfloat mountains_colors[108] = { //--- �ﰢ�� ������ ����
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, //��
		1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, //��
		0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, //��
		0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5  //�Ʒ�
	};

	const GLfloat mountains_base_colors[108] = { //--- �ﰢ�� ������ ����
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, //��
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, //��
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, //��
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, //��
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, //��
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0  //�Ʒ�
	};

	const GLfloat mountains_player_colors[108] = { //--- �ﰢ�� ������ ����
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0  //�Ʒ�
	};

	bool projection_type;
	float mountains_scale_height[400] = { 0, };
	float mountains_scale_max_height[400] = { 0, };
	float mountains_speed[400] = { 0, };
	bool mountains_scale_height_check[400] = { false, };
	bool mountains_move;
	int mountain_index;
	float cameraRx, cameraRy,camera_x,camera_y, camera_z;
	float c_offsetx, c_offsety, c_offsetz;
	float projection_z;
	int mx, my;
	bool make_maze;
	bool player;
	float player_x, player_y,player_z,player_angle;
	int player_dir;
	glm::vec3 target;
public:
	Mymountain()
	{
		init();
	}
	void init()
	{
		input();
		projection_type = true;
		mountains_move = false;
		make_maze = false;
		mountain_index = 0;
		cameraRx = cameraRy = 0.0f;
		projection_z = -2.0f;
		player = false;
		//player_x = -0.8f;
		//player_z = -1.0f + (((my + 2) / 2) * (0.15f + 0.01f));
		camera_x = 1.96f;
		camera_y = 0.03f;
		camera_z = 0;
		player_x = -0.9f;
		player_z = 0.19f;
		player_y = -0.2f;
		player_angle = 0.0f;
		player_dir = 0;
		c_offsetx = 0;
		c_offsety = 0;
		c_offsety = 0;
		target = glm::vec3(1.0f);
		for (int i = 0; i < mx * my; ++i)
		{
			mountains_scale_height[i] = 0.3f;
			mountains_scale_max_height[i] = (float)((rand() % 30 + 8) / 10);
			mountains_speed[i] = (float)((float)(rand() % 4 + 2) / 100);
		}
	}

	void mountains_scale_height_move()
	{
		for (int i = 0; i < mx * my; ++i)
		{
			if (mountains_scale_height_check[i] == false)
			{
				mountains_scale_height[i] += mountains_speed[i];
				if (mountains_scale_height[i] >= mountains_scale_max_height[i])
				{
					mountains_scale_height_check[i] = true;
				}
			}
			else
			{
				mountains_scale_height[i] -= mountains_speed[i];
				if (mountains_scale_height[i] <= 0.3f)
				{
					mountains_scale_height_check[i] = false;
				}
			}
		}
	}

	void mountains_add_speed(float speed)
	{
		for (int i = 0; i < mx * my; ++i)
		{
			if (mountains_speed[i] >= 0.015f && mountains_speed[i] <= 0.06f)
			{
				mountains_speed[i] += speed;
			}
		}
	}

	void mountains_move_stop()
	{
		mountains_move = false;
		mountains_set_height(0.6f);
	}

	void mountains_set_height(float height)
	{
		for (int i = 0; i < mx * my; ++i)
		{
			mountains_scale_height[i] = height;
		}
	}

	void input()
	{
		cout << "����ü�� ���� ���� �� �Է� :";
		cin >> mx >> my;
		cin.clear();
		cout << "-----------------------command-----------------------" << endl;
		cout << "����					- o/p (����/����), z/Z (���������� z���̵�)" << endl;
		cout << "����ü					- m/M (�̵�/����)" << endl;
		cout << "ī�޶�ȸ��				- x/X,y/Y (���ǹ���/���� ����) " << endl;
		cout << "�̷�����				- r " << endl;
		cout << "��ü������				- t " << endl;
		cout << "����ü�����ױ���ȭ		- r " << endl;
		cout << "��ü�̵�				- w/a/s/d " << endl;
		cout << "����ü�ӵ���������		- +/- " << endl;
		cout << "ī�޶������ȯ(�̱���)	- 1/3 " << endl;
		cout << "�ʱ�ȭ					- c " << endl;
		cout << "���α׷�����			- q " << endl;
	}
};


Mymountain mountain;
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

	glutTimerFunc(1.0f, mytimer, 0);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}
float a = 0.1f;
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glUseProgram(s_program);

	//glutSwapBuffers(); // ȭ�鿡 ����ϱ�
	glViewport(0, 0, MY_WIDTH, MY_HEIGHT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glEnable(GL_DEPTH_TEST);
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.

	my_draw(0);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�

	
	glViewport(500, 600, MY_WIDTH / 3, MY_HEIGHT / 3);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glEnable(GL_DEPTH_TEST);// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	my_draw(1);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�

}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	
}
void make_vertexShaders()
{
	GLchar* vertexsource;
	vertexsource = filetobuf("h_vertex.glsl");
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
	glGenVertexArrays(4, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(8, vbo);

	//����ü vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains), mountain.mountains, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����ü color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains_colors), mountain.mountains_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[401]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains), mountain.mountains, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����ü color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains_base_colors), mountain.mountains_base_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[403]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains), mountain.mountains, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����ü color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains_player_colors), mountain.mountains_player_colors, GL_STATIC_DRAW);
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
	int angle = 0;
	switch (key)
	{
	case 'o': //��������
		mountain.projection_type = true;
		break;
	case 'p': //����
		mountain.projection_type = false;
		break;
	case 'm': //����ü ���Ʒ� ������
		mountain.mountains_move = !mountain.mountains_move;
		break;
	case 'x':
		mountain.cameraRx += 1.0f;
		break;
	case 'X':
		mountain.cameraRx -= 1.0f;
		break;
	case 'y':
		mountain.cameraRy += 1.0f;
		break;
	case 'Y':
		mountain.cameraRy -= 1.0f;
		break;
	case 'z':
		mountain.projection_z += 0.1f;
		break;
	case 'Z':
		mountain.projection_z -= 0.1f;
		break;
	case '+':
		mountain.mountains_add_speed(0.005);
		break;
	case '-':
		mountain.mountains_add_speed(-0.005);
		break;
	case 'v':
		if (mountain.mountains_move)
		{
			mountain.mountains_move_stop();
		}
		else
		{
			mountain.mountains_move = true;
		}
		break;
	case 'r':
		mountain.make_maze = true;
		break;
	case 't':
		mountain.player = true;
		break;
	case 'w':
		switch (mountain.player_dir)
		{
		case 0:
			mountain.player_x += 0.01f;
			break;
		case 1:
			mountain.player_z -= 0.01f;
			break;
		case 2:
			mountain.player_x -= 0.01f;
			break;
		case 3:
			mountain.player_z += 0.01f;
			break;
		}
		break;
	case 'a':
		mountain.player_angle += 90.0f;
		mountain.player_angle = (int)mountain.player_angle % 360;
		mountain.player_dir++;
		mountain.player_dir = mountain.player_dir % 4;
		break;
	case 's':
		switch (mountain.player_dir)
		{
		case 0:
			mountain.player_x -= 0.01f;
			break;
		case 1:
			mountain.player_z += 0.01f;
			break;
		case 2:
			mountain.player_x += 0.01f;
			break;
		case 3:
			mountain.player_z -= 0.01f;
			break;
		}
		break;
	case 'd':
		mountain.player_angle -= 90.0f;
		mountain.player_angle = (int)mountain.player_angle % 360;
		mountain.player_dir--;
		if (mountain.player_dir < 0)
		{
			mountain.player_dir = 3;
		}
		break;
	case 'c':
		mountain.init();
		break;
	case 'q':
		exit(0);
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
	if (mountain.mountains_move)
	{
		mountain.mountains_scale_height_move();
	}

	glutPostRedisplay();
	glutTimerFunc(10.0f, mytimer, 0);
}

void my_draw(int type)
{
	
	//ī�޶�
	if (type == 0)
	{
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.7f, 2.0f); //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		view = glm::rotate(view, glm::radians(mountain.cameraRx), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(mountain.cameraRy), glm::vec3(0.0f, 1.0f, 0.0f));

		unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	}
	else
	{
		glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 0.1f); //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

		unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	}

	//����
	glm::mat4 projection = glm::mat4(1.0f);
	if (mountain.projection_type)
	{
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 5.0f, -5.0f);
	}
	else
	{
		projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 500.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, mountain.projection_z)); //--- ������ �ణ �ڷ� �̷���
	}
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	glBindVertexArray(vao[0]);
	float ox = -1.0f;
	float oz = -1.0f;
	float offset_x = 0.15f;
	float offset_z = 0.15f;
	int mx = mountain.mx;
	int my = mountain.my;

	for (int z = 0; z < my; ++z)
	{
		for (int x = 0; x < mx; ++x)
		{
			float height = mountain.mountains_scale_height[mountain.mountain_index];
			mountain.mountain_index++;
			mountain.mountain_index = mountain.mountain_index % (int)(mx * my);
			if (mountain.make_maze)
			{
				if (z != my / 2 && x != mx / 2 && z != (my / 2) + 1 && x != (mx / 2) + 1)
				{
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(ox + (x * offset_x), 0.0f, oz + (z * offset_z)));
					model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
					model = glm::scale(model, glm::vec3(0.3f, height, 0.3f));
					model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
			else
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(ox + (x * offset_x), 0.0f, oz + (z * offset_z)));
				model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
				model = glm::scale(model, glm::vec3(0.3f, height, 0.3f));
				model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}

	//�ٴ� 400 : 5.95 = 100 : ?
	glBindVertexArray(vao[401]);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(ox, -0.27f, oz));
	model = glm::translate(model, glm::vec3(-0.29f, 0.0f, -0.29f));
	model = glm::scale(model, glm::vec3(5.95f, 0.1f, 5.95f));
	model = glm::translate(model, glm::vec3(0.29f, 0.0f, 0.29f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);


	if (mountain.player)
	{
		glBindVertexArray(vao[403]);
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(ox + 0.2f + mountain.player_x, -0.2f, oz + (((my+2) / 2) * (offset_z + 0.01f))+ mountain.player_z));

		model = glm::translate(model, glm::vec3(mountain.player_x, mountain.player_y, mountain.player_z));

		model = glm::translate(model, glm::vec3(0.0, 0.25, 0.0));
		model = glm::rotate(model, glm::radians(mountain.player_angle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0, -0.25, 0.0));

		model = glm::scale(model, glm::vec3(0.2f, 0.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}