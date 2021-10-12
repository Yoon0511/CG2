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
#define MY_HEIGHT	600
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

GLuint vertexShader;
GLuint fragmentShader;
GLuint uniformModel;
float r = 0;
float offset = 0.0f;
float trilncrment = 0.01f;

typedef struct my_Vectex {
	float x, y, z;
};
typedef struct my_Rectpos {
	float x, y;
};


class myRect {
public:
	int hex_draw_begin,tri_draw_begin;
	int hex_draw_end, tri_draw_end;
	bool hex_draw[2];
	int hex_draw_begins[2];
	int hex_draw_ends[2];
	int tri_draw_begins[2], tri_draw_ends[2];
	bool tri_draw_type;
public:
	myRect() {
		hex_draw[0] = hex_draw[1] = false;
		hex_draw_begin = hex_draw_end = 0;
		tri_draw_begin = tri_draw_end = 0;
		tri_draw_type = false;
		for (int i = 0; i < 2; ++i)
		{
			hex_draw_begins[i] = hex_draw_ends[i] = 0;
			tri_draw_begins[i] = tri_draw_ends[i] = 0;
		}
	}
	GLfloat hexahedronp[108] = { 
		  0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,-0.5,0.5, -0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5,	//��
		  0.5,0.5,0.0, 0.5,-0.5,0.0, -0.5,-0.5,0.0, -0.5,-0.5,0.0, -0.5,0.5,0.0, 0.5,0.5,0.0,	//��
		  0.5,0.5,0.5, 0.5,-0.5,0.5, 0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.5,0.5,0.0, 0.5,0.5,0.5,		//��
		 -0.5,0.5,0.5, -0.5,0.5,0.0, -0.5,-0.5,0.0, -0.5,-0.5,0.0, -0.5,-0.5,0.5, -0.5,0.5,0.5,	//��
		  0.5,0.5,0.5, 0.5,0.5,0.0, -0.5,0.5,0.0, -0.5,0.5,0.0, -0.5,0.5,0.5, 0.5,0.5,0.5,		//��
		  0.5,-0.5,0.5, -0.5,-0.5,0.5, -0.5,-0.5,0.0, -0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.5,-0.5,0.5	//�Ʒ�
	};

	const GLfloat colors[108] = { //--- �ﰢ�� ������ ����
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0,//��
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0,//��
		1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0,//��
		0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0,//��
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0//�Ʒ�
	};
	
	const GLfloat lines[12] = {
		-1.0,0.0,0.0, 1.0,0.0,0.0,
		0.0,-1.0,0.0, 0.0,1.0,0.0,
	};

	const GLfloat lines_color[12] = {
		0.0,1.0,0.0, 0.0,1.0,0.0,
		1.0,0.0,0.0, 1.0,0.0,1.0,
	};

	GLfloat tri[36] = {
		-0.5,0.0,0.0, 0.5,0.0,0.0, 0.0,0.5,0.0,  0.0,0.0,0.5, 0.5,0.0,0.0, 0.0,0.5,0.0, //�� ��
		-0.5,0.0,0.0, 0.0,0.0,0.5, 0.0,0.5,0.0,  0.0,0.0,0.5, -0.5,0.0,0.0, 0.5,0.0,0.0 //�� 
	};

	GLfloat tri_colors[36] = {
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0,  0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0,
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0,  0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0
	};

public:
	void set_draw_index(int begin, int end)
	{
		hex_draw[0] = true;
		hex_draw[1] = false;
		hex_draw_begin = begin;
		hex_draw_end = end;
	}
	void set_draw_hexindexs(int b1, int e1, int b2, int e2)
	{
		hex_draw[0] = true;
		hex_draw[1] = true;
		hex_draw_begins[0] = b1;
		hex_draw_ends[0] = e1;
		hex_draw_begins[1] = b2;
		hex_draw_ends[1] = e2;
	}

	void set_draw_tri_index(int begin, int end)
	{
		hex_draw[0] = false;
		tri_draw_type = true;
		tri_draw_begin = begin;
		tri_draw_end = end;
	}
	void set_draw_tri_indexs(int b1, int e1, int b2, int e2)
	{
		hex_draw[0] = false;
		tri_draw_type = false;
		tri_draw_begins[0] = b1;
		tri_draw_ends[0] = e1;
		tri_draw_begins[1] = b2;
		tri_draw_ends[1] = e2;
	}
};

GLuint vao[3], vbo[6];
GLuint s_program;
float rect_size = RECT_SIZE;
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
	
	//glEnable(GL_DEPTH_TEST);
	
	glUseProgram(s_program);
	//xy��
	glm::mat4 model = glm::mat4(1.0f);
	glBindVertexArray(vao[1]);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_LINES, 0, 10);

	//����ü
	set_rotate_triangle();
	if (myrect.hex_draw[0] == true)
	{
		glBindVertexArray(vao[0]);
		if (myrect.hex_draw[1] == false) //�� �� ���
		{
			glDrawArrays(GL_TRIANGLES, myrect.hex_draw_begin, myrect.hex_draw_end);
		}
		else //���� �ݴ�����
		{
			glDrawArrays(GL_TRIANGLES, myrect.hex_draw_begins[0], myrect.hex_draw_ends[0]);
			glDrawArrays(GL_TRIANGLES, myrect.hex_draw_begins[1], myrect.hex_draw_ends[1]);
		}
	}
	else
	{
		//���ü
		glBindVertexArray(vao[2]);
		if (myrect.tri_draw_type == true) // ���� ���
		{
			glDrawArrays(GL_TRIANGLES, myrect.tri_draw_begin, myrect.tri_draw_end);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, myrect.tri_draw_begins[1], myrect.tri_draw_ends[1]);
			glDrawArrays(GL_TRIANGLES, myrect.tri_draw_begins[0], myrect.tri_draw_ends[0]);
		}
	}
	
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
	//glDisable(GL_DEPTH_TEST);
}

void set_rotate_triangle()
{
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	Tx = glm::rotate(Tx, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Ty = glm::rotate(Tx, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	model = Tx * Ty;

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
}

void mytimer(int value)
{
	InitBuffer();
	glutTimerFunc(0.1f, mytimer, 0);
	glutPostRedisplay();
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
	case '1': //��
		myrect.set_draw_index(0, 6);
		break;
	case '2': //��
		myrect.set_draw_index(6, 6);
		break;
	case '3': //��
		myrect.set_draw_index(12, 6);
		break;
	case '4': //��
		myrect.set_draw_index(18, 6);
		break;
	case '5': //��
		myrect.set_draw_index(24, 6);
		break;
	case '6': //�Ʒ�
		myrect.set_draw_index(30, 6);
		break;
	case 'a': //�յ�
		myrect.set_draw_hexindexs(0, 6, 6, 6);
		break;
	case 'b': //����
		myrect.set_draw_hexindexs(12, 6, 18, 6);
		break;
	case 'c': //���Ʒ�
		myrect.set_draw_hexindexs(24, 6, 30, 6);
		break;
	case '7':
		myrect.set_draw_tri_index(0, 3);
		break;
	case '8':
		myrect.set_draw_tri_index(3, 3);
		break;
	case '9':
		myrect.set_draw_tri_index(6, 3);
		break;
	case '0':
		myrect.set_draw_tri_index(9, 3);
		break;
	case 'e':
		myrect.set_draw_tri_indexs(0, 3,9,3);
		break;
	case 'f':
		myrect.set_draw_tri_indexs(3, 3,9,3);
		break;
	case 'g':
		myrect.set_draw_tri_indexs(6, 3,9,3);
		break;
	}
	glutPostRedisplay();
}