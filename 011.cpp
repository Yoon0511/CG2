#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
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

class myRect {
public:
	bool outdrag = false;
	bool indrag = false;
	int selectindex = 0;
	my_Rectpos mClickpos;
	my_Rectpos mWinpos;
	my_Rectpos mvertex[4];
	GLfloat triShape[1][4][3] = {
	{{ -0.5, -0.5, 0.0 }, { 0.5, -0.5, 0.0 }, { 0.5, 0.5, 0.0},{-0.5,0.5,0.0}}
	};

	const GLfloat colors[1][4][3] = {
		{{ 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 },{0.0,0.0,0.0}
		}
	};
public:
	void init()
	{
		mWinpos.x = mWinpos.y = 0;
		for (int i = 0; i < 4; ++i)
		{
			mvertex[i].x = triShape[0][i][0];
			mvertex[i].y = triShape[0][i][1];
		}
	}

	bool ismousepointerinrect(float x, float y) //삼각형 안에 클릭했는지 체크하는 함수
	{
		int count = 0;
		for (int i = 0;i<4;++i)
		{
			int j = (i + 1) % 4;
			if ((mvertex[i].y > y) != (mvertex[j].y > y))
			{
				float atx = ((mvertex[j].x - mvertex[i].x) * (y - mvertex[i].y)) / (mvertex[j].y - mvertex[i].y) + mvertex[i].x;
				if (x < atx) count++;
			}
		}
		
		return count % 2 > 0;
	}

	bool get_min_dis_index(float x,float y)
	{
		int index = 0;
		float mindis = 0.02f;
		for (int i = 0; i < 4; ++i)
		{
			float dis = getdistance(x, y, mvertex[i].x, mvertex[i].y);
			if (abs(dis) < mindis)
			{
				mindis = dis;
				index = i;
				selectindex = i;
				return true;
			}
		}
		return false;
	}

	float getdistance(float x1, float y1,float x2,float y2)
	{
		return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
	}

	void vertexDrag(float x, float y)
	{
		triShape[0][selectindex][0] = x;
		triShape[0][selectindex][1] = y;
		mvertex[selectindex].x = triShape[0][selectindex][0];
		mvertex[selectindex].y = triShape[0][selectindex][1];
		InitBuffer();
	}

	void setdragvertex(float x,float y)
	{
		selectindex = get_min_dis_index(x, y);
	}

	void setClickpos(float x, float y)
	{
		mClickpos.x = x;
		mClickpos.y = y;
	}

	void rectDrag(float x, float y)
	{
		my_Rectpos movepos;
		movepos.x = (x - mClickpos.x) / (MY_WIDTH / 4);
		movepos.y = (y - mClickpos.y) / (MY_HEIGHT / 4);
		for (int i = 0; i < 4; ++i)
		{
			mvertex[i].x += movepos.x;
			mvertex[i].y += movepos.y;
			triShape[0][i][0] = mvertex[i].x;
			triShape[0][i][1] = mvertex[i].y;
		}
		InitBuffer();
	}
};

GLuint vao, vbo[2];
GLuint s_program;
float rect_size = RECT_SIZE;
myRect myrect;
bool MouseMove_check = false;

void main(int argc, char** argv)
{
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
	glBindVertexArray(vao);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

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
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), myrect.triShape, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), myrect.colors, GL_STATIC_DRAW);
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
		if (myrect.outdrag)
		{
			myrect.vertexDrag(mx, my);
		}
		if (myrect.indrag)
		{
			myrect.rectDrag(mx, my);
		}
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float mx = 0, my = 0;
		convertWINXYOpenGlXY(x, y, &mx, &my);
		if (myrect.ismousepointerinrect(mx, my))//내부클릭
		{
			myrect.setClickpos(mx,my);
			myrect.indrag = true;
			MouseMove_check = true;
		}
		else //외부클릭
		{
			if (myrect.get_min_dis_index(mx, my))
			{
				myrect.outdrag = true;
				MouseMove_check = true;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		myrect.outdrag = false;
		myrect.indrag = false;
		MouseMove_check = false;
	}

	glutPostRedisplay();
}