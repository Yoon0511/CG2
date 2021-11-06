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
GLuint vao[400], vbo[800];
GLuint s_program;

class Mymountain {
public:
	GLfloat mountains[108] = {
		  0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25,				//앞
		  0.25,0.25,-0.25, 0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,0.25,-0.25, 0.25,0.25,-0.25,		//뒤
		  0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,0.25,-0.25, 0.25,0.25,0.25,				//오
		 -0.25,0.25,0.25, -0.25,0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,0.25, -0.25,0.25,0.25,		//왼
		  0.25,0.25,0.25, 0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,0.25, 0.25,0.25,0.25,				//위
		  0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,-0.25,0.25		//아래
	};

	const GLfloat mountains_colors[108] = { //--- 삼각형 꼭지점 색상
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //앞
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //뒤
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, //오
		1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, //왼
		0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, //위
		0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5  //아래
	};
	bool projection_type;
	float mountains_scale_height[400] = { 0, };
	float mountains_scale_max_height[400] = { 0, };
	float mountains_speed[400] = { 0, };
	bool mountains_scale_height_check[400] = { false, };
	bool mountains_move;
	int mountain_index;
	float cameraRx, cameraRy;
public:
	Mymountain()
	{
		projection_type = true;
		mountains_move = false;
		mountain_index = 0;
		cameraRx = cameraRy = 0.0f;
		init();
	}
	void init()
	{
		for (int i = 0; i < 400; ++i)
		{
			mountains_scale_height[i] = 0.3f;
			mountains_scale_max_height[i] = (float)((rand() % 30 + 8) / 10);
			mountains_speed[i] = (float)((float)(rand() % 4 + 2) / 100);
		}
	}

	void mountains_scale_height_move()
	{
		for (int i = 0; i < 400; ++i)
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
};


Mymountain mountain;
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

	glutTimerFunc(1.0f, mytimer, 0);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}
float a = 0.1f;
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’ 로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	glUseProgram(s_program);
	glEnable(GL_DEPTH_TEST);

	//카메라
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.7f, 2.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	view = glm::rotate(view, glm::radians(mountain.cameraRx), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, glm::radians(mountain.cameraRy), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- 뷰잉 변환 설정

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	//투영
	glm::mat4 projection = glm::mat4(1.0f);
	if (mountain.projection_type)
	{
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 5.0f, -5.0f);
	}
	else
	{
		projection = glm::perspective(glm::radians(100.0f), 1.0f, 0.1f, 500.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0)); //--- 공간을 약간 뒤로 미뤄줌
	}
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	glBindVertexArray(vao[0]);
	float ox = -1.0f;
	float oz = -1.0f;
	float offset_x = 0.15f;
	float offset_z = 0.15f;
	for (int z = 0; z < 20; ++z)
	{
		for (int x = 0; x < 20; ++x)
		{
			float height = mountain.mountains_scale_height[mountain.mountain_index];
			mountain.mountain_index++;
			mountain.mountain_index = mountain.mountain_index % 400;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(ox + (x * offset_x), 0.0f, oz + (z * offset_z)));
			model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
			model = glm::scale(model, glm::vec3(0.3f, height, 0.3f));
			model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
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
	glGenVertexArrays(4, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(8, vbo);

	//육면체 vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains), mountain.mountains, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//육면체 color
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mountain.mountains_colors), mountain.mountains_colors, GL_STATIC_DRAW);
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
	case 'o': //직각투영
		mountain.projection_type = true;
		break;
	case 'p': //원근
		mountain.projection_type = false;
		break;
	case 'm': //육면체 위아래 움직임
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