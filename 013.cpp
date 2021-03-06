#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
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
		  0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,-0.5,0.5, -0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5,	//앞
		  0.5,0.5,0.0, 0.5,-0.5,0.0, -0.5,-0.5,0.0, -0.5,-0.5,0.0, -0.5,0.5,0.0, 0.5,0.5,0.0,	//뒤
		  0.5,0.5,0.5, 0.5,-0.5,0.5, 0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.5,0.5,0.0, 0.5,0.5,0.5,		//오
		 -0.5,0.5,0.5, -0.5,0.5,0.0, -0.5,-0.5,0.0, -0.5,-0.5,0.0, -0.5,-0.5,0.5, -0.5,0.5,0.5,	//왼
		  0.5,0.5,0.5, 0.5,0.5,0.0, -0.5,0.5,0.0, -0.5,0.5,0.0, -0.5,0.5,0.5, 0.5,0.5,0.5,		//위
		  0.5,-0.5,0.5, -0.5,-0.5,0.5, -0.5,-0.5,0.0, -0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.5,-0.5,0.5	//아래
	};

	const GLfloat colors[108] = { //--- 삼각형 꼭지점 색상
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //앞
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0,//뒤
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0,//오
		1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0,//왼
		0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0,//위
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0//아래
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
		-0.5,0.0,0.0, 0.5,0.0,0.0, 0.0,0.5,0.0,  0.0,0.0,0.5, 0.5,0.0,0.0, 0.0,0.5,0.0, //앞 오
		-0.5,0.0,0.0, 0.0,0.0,0.5, 0.0,0.5,0.0,  0.0,0.0,0.5, -0.5,0.0,0.0, 0.5,0.0,0.0 //왼 
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
	//--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
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

	InitShader();
	InitBuffer();

	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’ 로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	
	//glEnable(GL_DEPTH_TEST);
	
	glUseProgram(s_program);
	//xy축
	glm::mat4 model = glm::mat4(1.0f);
	glBindVertexArray(vao[1]);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_LINES, 0, 10);

	//육면체
	set_rotate_triangle();
	if (myrect.hex_draw[0] == true)
	{
		glBindVertexArray(vao[0]);
		if (myrect.hex_draw[1] == false) //각 면 출력
		{
			glDrawArrays(GL_TRIANGLES, myrect.hex_draw_begin, myrect.hex_draw_end);
		}
		else //서로 반대방향면
		{
			glDrawArrays(GL_TRIANGLES, myrect.hex_draw_begins[0], myrect.hex_draw_ends[0]);
			glDrawArrays(GL_TRIANGLES, myrect.hex_draw_begins[1], myrect.hex_draw_ends[1]);
		}
	}
	else
	{
		//사면체
		glBindVertexArray(vao[2]);
		if (myrect.tri_draw_type == true) // 각면 출력
		{
			glDrawArrays(GL_TRIANGLES, myrect.tri_draw_begin, myrect.tri_draw_end);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, myrect.tri_draw_begins[1], myrect.tri_draw_ends[1]);
			glDrawArrays(GL_TRIANGLES, myrect.tri_draw_begins[0], myrect.tri_draw_ends[0]);
		}
	}
	
	glutSwapBuffers(); // 화면에 출력하기
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

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
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
	GLuint ShaderProgramID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(ShaderProgramID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(ShaderProgramID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(ShaderProgramID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 프로그램에 링크하여 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}

	return ShaderProgramID;
}

void InitBuffer()
{
	//외부 움직이는 삼각형
	glGenVertexArrays(3, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(6, vbo);
	
	//육면체 vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hexahedronp), myrect.hexahedronp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//육면체 color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.colors), myrect.colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(vao[1]);
	//xy축 vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines), myrect.lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//xy축 color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines_color), myrect.lines_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[2]);
	//사면체
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri), myrect.tri, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//xy축 color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_colors), myrect.tri_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

}

void InitShader()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	uniformModel = glGetUniformLocation(s_program, "model");

	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
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
	case '1': //앞
		myrect.set_draw_index(0, 6);
		break;
	case '2': //뒤
		myrect.set_draw_index(6, 6);
		break;
	case '3': //오
		myrect.set_draw_index(12, 6);
		break;
	case '4': //왼
		myrect.set_draw_index(18, 6);
		break;
	case '5': //위
		myrect.set_draw_index(24, 6);
		break;
	case '6': //아래
		myrect.set_draw_index(30, 6);
		break;
	case 'a': //앞뒤
		myrect.set_draw_hexindexs(0, 6, 6, 6);
		break;
	case 'b': //오왼
		myrect.set_draw_hexindexs(12, 6, 18, 6);
		break;
	case 'c': //위아래
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