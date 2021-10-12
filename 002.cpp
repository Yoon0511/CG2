#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>

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
void convertWINXYOpenGlXY(int x, int y, float* ox, float* oy);
void repostion_triangleVertex(int x, int y);
void init_triangleVertex();
void mytimer(int value);
void set_rotate_triangle(int index);


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
	int dir[4] = {0,1,2,3};
	float speed[4] = {0.3,0.5,0.7,0.9};
	float angle[4] = { 90,180,270,0 };
	my_Rectpos mWinpos[4];
	GLfloat triShape[4][3][3] = { //--- �ﰢ�� ��ġ ��
	{{ -0.2, 0.3, 0.0 }, { 0.2, 0.3, 0.0 }, { 0.0, 0.8, 0.0} },
	{{ -0.5, -0.2, 0.0 }, { -0.7, -0.2, 0.0 }, { -0.6, 0.2, 0.0}},
	{{ -0.5, -0.5, 0.0 }, { 0.2, -0.5, 0.0 }, { 0.0, 0.3, 0.0}},
	{{ -0.5, -0.5, 0.0 }, { 0.5, -0.5, 0.0 }, { 0.0, 0.5, 0.0}}
	};

	const GLfloat colors[4][3][3] = { //--- �ﰢ�� ������ ����
		{{ 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }},
		{{ 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }},
		{{ 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }},
		{{ 1.0, 1.0, 0.0 }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0, 0.0 }}
	};
public:
	void init_triangle()
	{
		my_Rectpos mwinPos[4] = { {200,150},{600,150},{200,450},{600,450} }; //������ ��ǥ�� ���� �ﰢ���� ����
		my_Rectpos mRectpos[4] = { 0, };
		for (int i = 0; i < 4; ++i) //�ﰢ���� ������ opengl��ǥ�� ��ȯ
		{
			float x = 0, y = 0;
			mWinpos[i].x = mwinPos[i].x;
			mWinpos[i].y = mwinPos[i].y;
			convertWINXYOpenGlXY(mwinPos[i].x, mwinPos[i].y, &x, &y);
			mRectpos[i].x = x;
			mRectpos[i].y = y;
		}
		my_Vectex mVectex = { 0,0,0 };

		for (int i = 0; i < 4; ++i)			//i��° �簢��
		{
			for (int j = 0; j < 3; ++j)		//j��° ��
			{
				switch (j)	// i��° �簢���� �߽����� ���� j��° �� xyz���
				{
				case 0:				//������
					mVectex.x = mRectpos[i].x - RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 1:				//��������
					mVectex.x = mRectpos[i].x + RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 2:				//����
					mVectex.x = mRectpos[i].x;
					mVectex.y = mRectpos[i].y + (RECT_SIZE + 0.15);
					mVectex.z = 0;
					break;
				}

				for (int k = 0; k < 3; ++k)
				{
					switch (k)
					{
					case 0:						//x
						triShape[i][j][k] = mVectex.x;
						break;
					case 1:						//y
						triShape[i][j][k] = mVectex.y;
						break;
					case 2:						//z
						triShape[i][j][k] = mVectex.z;
						break;
					}
				}
			}
		}
	}
	void moving()
	{
		update_centerpos_win();
	}
	void set_centerpos()
	{
		my_Rectpos mRectpos[4] = { 0, };
		for (int i = 0; i < 4; ++i) //�ﰢ���� ������ opengl��ǥ�� ��ȯ
		{
			float x = 0, y = 0;
			convertWINXYOpenGlXY(mWinpos[i].x, mWinpos[i].y, &x, &y);
			mRectpos[i].x = x;
			mRectpos[i].y = y;
		}
		my_Vectex mVectex = { 0,0,0 };

		for (int i = 0; i < 4; ++i)			//i��° �簢��
		{
			for (int j = 0; j < 3; ++j)		//j��° ��
			{
				switch (j)	// i��° �簢���� �߽����� ���� j��° �� xyz���
				{
				case 0:				//������
					mVectex.x = mRectpos[i].x - RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 1:				//��������
					mVectex.x = mRectpos[i].x + RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 2:				//����
					mVectex.x = mRectpos[i].x;
					mVectex.y = mRectpos[i].y + (RECT_SIZE + 0.15);
					mVectex.z = 0;
					break;
				}

				for (int k = 0; k < 3; ++k)
				{
					switch (k)
					{
					case 0:						//x
						triShape[i][j][k] = mVectex.x;
						break;
					case 1:						//y
						triShape[i][j][k] = mVectex.y;
						break;
					case 2:						//z
						triShape[i][j][k] = mVectex.z;
						break;
					}
				}
			}
		}
	}
	void update_centerpos_win()
	{
		for (int i = 0; i < 4; ++i)
		{
			switch (dir[i])
			{
			case 0: // LEFT UP
				mWinpos[i].x -= speed[i];
				mWinpos[i].y -= speed[i];
				if (mWinpos[i].y <= 20) { dir[i] = 1; angle[i] = 180; }
				if (mWinpos[i].x <= 20) { dir[i] = 2; angle[i] = 270; }
				break;
			case 1: // LEFT DOWN
				mWinpos[i].x -= speed[i];
				mWinpos[i].y += speed[i];
				if (mWinpos[i].y >= 580) { dir[i] = 0; angle[i] = 0; }
				if (mWinpos[i].x <= 10) { dir[i] = 3; angle[i] = 270; }
				break;
			case 2:	// RIGHT UP
				mWinpos[i].x += speed[i];
				mWinpos[i].y -= speed[i];
				if (mWinpos[i].y <= 20) { dir[i] = 3; angle[i] = 180; }
				if (mWinpos[i].x >= 780) { dir[i] = 0; angle[i] = 90; }
				break;
			case 3:	// RIGHT DOWN
				mWinpos[i].x += speed[i];
				mWinpos[i].y += speed[i];
				if (mWinpos[i].y >= 580) { dir[i] = 2; angle[i] = 0; }
				if (mWinpos[i].x >= 780) { dir[i] = 1; angle[i] = 90; }
				break;
			}
		}
		set_centerpos();
	}
};

GLuint vao, vbo[2];
GLuint s_program;
float rect_size = RECT_SIZE;
myRect myrect;

void main(int argc, char** argv)
{
	//--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
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
	myrect.init_triangle();
	InitBuffer();

	glutTimerFunc(0.1f, mytimer, 0);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	
	glUseProgram(s_program);
	//--- ����� VAO �ҷ�����
	glBindVertexArray(vao);
	//--- �ﰢ�� �׸���
	
	set_rotate_triangle(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	set_rotate_triangle(1);
	glDrawArrays(GL_TRIANGLES, 3, 3);
	set_rotate_triangle(2);
	glDrawArrays(GL_TRIANGLES, 6, 3);
	set_rotate_triangle(3);
	glDrawArrays(GL_TRIANGLES, 9, 3);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void set_rotate_triangle(int index)
{
	glm::mat4 model = glm::mat4(1.0f);
	float x = 0, y = 0;
	convertWINXYOpenGlXY(myrect.mWinpos[index].x, myrect.mWinpos[index].y, &x, &y);
	model = glm::translate(model, glm::vec3(x, y, 0.0f));
	model = glm::rotate(model, myrect.angle[index] * 3.14f / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-x, -y, 0.0f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
}

void mytimer(int value)
{
	myrect.moving();
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
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), myrect.triShape, GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), myrect.colors, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
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

void init_triangleVertex() 	// ������ ��ǥ�� �������� �ﰢ�� �׸���
{
	my_Rectpos mWinPos[4] = { {200,150},{600,150},{200,450},{600,450} }; //������ ��ǥ�� ���� �ﰢ���� ����
	my_Rectpos mRectpos[4] = { 0, };
	for (int i = 0; i < 4; ++i) //�ﰢ���� ������ opengl��ǥ�� ��ȯ
	{
		float x = 0, y = 0;
		myrect.mWinpos[i].x = mWinPos[i].x;
		myrect.mWinpos[i].y = mWinPos[i].y;
		convertWINXYOpenGlXY(mWinPos[i].x, mWinPos[i].y, &x, &y);
		mRectpos[i].x = x;
		mRectpos[i].y = y;
	}
	my_Vectex mVectex = { 0,0,0 };

	for (int i = 0; i < 4; ++i)			//i��° �簢��
	{
		for (int j = 0; j < 3; ++j)		//j��° ��
		{
			switch (j)	// i��° �簢���� �߽����� ���� j��° �� xyz���
			{
			case 0:				//������
				mVectex.x = mRectpos[i].x - RECT_SIZE;
				mVectex.y = mRectpos[i].y - RECT_SIZE;
				mVectex.z = 0;
				break;
			case 1:				//��������
				mVectex.x = mRectpos[i].x + RECT_SIZE;
				mVectex.y = mRectpos[i].y - RECT_SIZE;
				mVectex.z = 0;
				break;
			case 2:				//����
				mVectex.x = mRectpos[i].x;
				mVectex.y = mRectpos[i].y + (RECT_SIZE + 0.15);
				mVectex.z = 0;
				break;
			}

			for (int k = 0; k < 3; ++k)
			{
				switch (k)
				{
				case 0:						//x
					myrect.triShape[i][j][k] = mVectex.x;
					break;
				case 1:						//y
					myrect.triShape[i][j][k] = mVectex.y;
					break;
				case 2:						//z
					myrect.triShape[i][j][k] = mVectex.z;
					break;
				}
			}
		}
	}
}
