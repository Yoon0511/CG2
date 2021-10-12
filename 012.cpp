#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
#include <time.h>
#include <random>

#define MY_WIDTH	800
#define MY_HEIGHT	600
#define RECT_SIZE	0.05

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
void MouseMove(int x, int y);
void Mouse(int button, int state, int x, int y);

GLuint vertexShader;
GLuint fragmentShader;
GLuint uniformModel;


typedef struct my_Vectex {
	float x, y, z;
};
typedef struct my_Rectpos {
	float x, y;
};
//GLuint vindex[] = { 0,1,2,2,3,0 };

GLuint vindex[2][6] = { { 0,1,2,2,3,0 },{ 0,1,2,2,3,0 } };

class myRect {
public:
	bool showeraser = false;
	int selectindex = 0;
	my_Rectpos mClickpos;
	my_Rectpos mWinpos;
	my_Rectpos eraser_ver[4];
	GLfloat triShape[100][4][3] = {
	};

	GLfloat colors[100][4][3] = {
	};

	GLfloat myEraser[4][3] = {
	};
	GLfloat myEraserColor[4][3] = {
		{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}
	};

public:
	void init()
	{
		for (int i = 0; i < 100; ++i)
		{
			float x = rand() % (MY_WIDTH-200) + 100;
			float y = rand() % (MY_HEIGHT-200) + 50;
			float mx = 0, my = 0;
			convertWINXYOpenGlXY(x, y, &mx, &my);
			setRect(i, mx, my);
		}
		
		setEraser(0, 0);
	}
	void Erasing(float x, float y)
	{
		EraserDrag(x, y);
		collider_check();
	}

	void EraserDrag(float x, float y)
	{
		setEraser(x, y);
	}
	void setRect(int i, float x, float y)
	{
		for (int j = 0; j < 4; ++j)
		{
			switch (j)
			{
			case 0:
				triShape[i][j][0] = x - RECT_SIZE;
				triShape[i][j][1] = y - RECT_SIZE;
				break;
			case 1:
				triShape[i][j][0] = x + RECT_SIZE;
				triShape[i][j][1] = y - RECT_SIZE;
				break;
			case 2:
				triShape[i][j][0] = x + RECT_SIZE;
				triShape[i][j][1] = y + RECT_SIZE;
				break;
			case 3:
				triShape[i][j][0] = x - RECT_SIZE;
				triShape[i][j][1] = y + RECT_SIZE;
				break;
			}
			triShape[i][j][2] = 0;


			for (int k = 0; k < 3; ++k)
			{
				float color = (float)(rand() % 255) / 255;
				colors[i][j][k] = color;
			}
		}
	}

	void setEraser(float x, float y)
	{
		for (int j = 0; j < 4; ++j)
		{
			switch (j)
			{
			case 0:
				eraser_ver[j].x = myEraser[j][0] = x - RECT_SIZE * 2;
				eraser_ver[j].y = myEraser[j][1] = y - RECT_SIZE * 2;
				break;
			case 1:
				eraser_ver[j].x = myEraser[j][0] = x + RECT_SIZE * 2;
				eraser_ver[j].y = myEraser[j][1] = y - RECT_SIZE * 2;
				break;
			case 2:
				eraser_ver[j].x = myEraser[j][0] = x + RECT_SIZE * 2;
				eraser_ver[j].y = myEraser[j][1] = y + RECT_SIZE * 2;
				break;
			case 3:
				eraser_ver[j].x = myEraser[j][0] = x - RECT_SIZE * 2;
				eraser_ver[j].y = myEraser[j][1] = y + RECT_SIZE * 2;
				break;
			}
			myEraser[j][2] = 0;
		}
	}
	
	void collider_check()
	{
		my_Rectpos v[4];
		for (int i = 0; i < 100; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				v[j].x = triShape[i][j][0];
				v[j].y = triShape[i][j][1];
			}
			if (myIntersectRect(v))
			{
				setRect(i, 50, 50);
			}
		}
	}

	bool myIntersectRect(my_Rectpos v[4])
	{
		//0 = 왼쪽 아래 2 = 오른쪽 위
		my_Rectpos vrect[4];
		my_Rectpos veraser[4];
		vrect[0].x = v[0].x; //left
		vrect[1].y = v[2].y; //top
		vrect[2].x = v[2].x; //right
		vrect[3].y = v[0].y; //bottom
		veraser[0].x = eraser_ver[0].x; //left
		veraser[1].y = eraser_ver[2].y; //top
		veraser[2].x = eraser_ver[2].x; //right
		veraser[3].y = eraser_ver[0].y; //bottom

		if (veraser[0].x < vrect[2].x &&
			veraser[1].y > vrect[3].y &&
			veraser[2].x > vrect[0].x &&
			veraser[3].y < vrect[1].y)
		{
			return true;
		}

		return false;
	}
};

GLuint vao[2], vbo[4],EBO;
GLuint s_program;
float rect_size = RECT_SIZE;
myRect myrect;
bool MouseMove_check = false;

void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(MY_WIDTH, MY_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	myrect.init();

	InitShader();
	InitBuffer();

	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(s_program);

	glBindVertexArray(vao[0]);
	glDrawArrays(GL_QUADS, 0, 404);

	if (myrect.showeraser)
	{
		glBindVertexArray(vao[1]);
		glDrawArrays(GL_QUADS, 0, 4);
	}

	glutSwapBuffers();

}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
	GLchar* vertexsource;
	vertexsource = filetobuf("vertex.glsl");
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
	fragmentsource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
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
	GLuint ShaderProgramID = glCreateProgram();
	glAttachShader(ShaderProgramID, vertexShader);
	glAttachShader(ShaderProgramID, fragmentShader);
	glLinkProgram(ShaderProgramID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}

	return ShaderProgramID;
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao[0]);
	glBindVertexArray(vao[0]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (100 * 4 * 3)*sizeof(GLfloat), myrect.triShape, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (100 * 4 * 3) * sizeof(GLfloat), myrect.colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &vao[1]);
	glBindVertexArray(vao[1]);
	glGenBuffers(1, &vbo[3]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, (4 * 3) * sizeof(GLfloat), myrect.myEraser, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vbo[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, (4 * 3) * sizeof(GLfloat), myrect.myEraserColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShaders();

	s_program = glCreateProgram();
	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	uniformModel = glGetUniformLocation(s_program, "model");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

void MouseMove(int x, int y)
{
	if (MouseMove_check)
	{
		float mx = 0, my = 0;
		convertWINXYOpenGlXY(x, y, &mx, &my);
		//myrect.EraserDrag(mx, my);
		myrect.Erasing(mx, my);
	}
	InitBuffer();
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float mx = 0, my = 0;
		convertWINXYOpenGlXY(x, y, &mx, &my);
		myrect.showeraser = true;
		MouseMove_check = true;
		myrect.Erasing(mx, my);
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		myrect.showeraser = false;
		MouseMove_check = false;
	}
	InitBuffer();
	glutPostRedisplay();
}