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
	GLfloat triShape[4][3][3] = { //--- 삼각형 위치 값
	{{ -0.2, 0.3, 0.0 }, { 0.2, 0.3, 0.0 }, { 0.0, 0.8, 0.0} },
	{{ -0.5, -0.2, 0.0 }, { -0.7, -0.2, 0.0 }, { -0.6, 0.2, 0.0}},
	{{ -0.5, -0.5, 0.0 }, { 0.2, -0.5, 0.0 }, { 0.0, 0.3, 0.0}},
	{{ -0.5, -0.5, 0.0 }, { 0.5, -0.5, 0.0 }, { 0.0, 0.5, 0.0}}
	};

	const GLfloat colors[4][3][3] = { //--- 삼각형 꼭지점 색상
		{{ 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }},
		{{ 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }},
		{{ 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }},
		{{ 1.0, 1.0, 0.0 }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0, 0.0 }}
	};
public:
	void init_triangle()
	{
		my_Rectpos mwinPos[4] = { {200,150},{600,150},{200,450},{600,450} }; //윈도우 좌표계 기준 삼각형의 중점
		my_Rectpos mRectpos[4] = { 0, };
		for (int i = 0; i < 4; ++i) //삼각형의 중점을 opengl좌표로 변환
		{
			float x = 0, y = 0;
			mWinpos[i].x = mwinPos[i].x;
			mWinpos[i].y = mwinPos[i].y;
			convertWINXYOpenGlXY(mwinPos[i].x, mwinPos[i].y, &x, &y);
			mRectpos[i].x = x;
			mRectpos[i].y = y;
		}
		my_Vectex mVectex = { 0,0,0 };

		for (int i = 0; i < 4; ++i)			//i번째 사각형
		{
			for (int j = 0; j < 3; ++j)		//j번째 점
			{
				switch (j)	// i번째 사각형의 중심으로 부터 j번째 점 xyz계산
				{
				case 0:				//왼쪽점
					mVectex.x = mRectpos[i].x - RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 1:				//오른쪽점
					mVectex.x = mRectpos[i].x + RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 2:				//윗점
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
		for (int i = 0; i < 4; ++i) //삼각형의 중점을 opengl좌표로 변환
		{
			float x = 0, y = 0;
			convertWINXYOpenGlXY(mWinpos[i].x, mWinpos[i].y, &x, &y);
			mRectpos[i].x = x;
			mRectpos[i].y = y;
		}
		my_Vectex mVectex = { 0,0,0 };

		for (int i = 0; i < 4; ++i)			//i번째 사각형
		{
			for (int j = 0; j < 3; ++j)		//j번째 점
			{
				switch (j)	// i번째 사각형의 중심으로 부터 j번째 점 xyz계산
				{
				case 0:				//왼쪽점
					mVectex.x = mRectpos[i].x - RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 1:				//오른쪽점
					mVectex.x = mRectpos[i].x + RECT_SIZE;
					mVectex.y = mRectpos[i].y - RECT_SIZE;
					mVectex.z = 0;
					break;
				case 2:				//윗점
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

	
	InitShader();
	myrect.init_triangle();
	InitBuffer();

	glutTimerFunc(0.1f, mytimer, 0);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’ 로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	
	glUseProgram(s_program);
	//--- 사용할 VAO 불러오기
	glBindVertexArray(vao);
	//--- 삼각형 그리기
	
	set_rotate_triangle(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	set_rotate_triangle(1);
	glDrawArrays(GL_TRIANGLES, 3, 3);
	set_rotate_triangle(2);
	glDrawArrays(GL_TRIANGLES, 6, 3);
	set_rotate_triangle(3);
	glDrawArrays(GL_TRIANGLES, 9, 3);

	glutSwapBuffers(); // 화면에 출력하기
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
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기
	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), myrect.triShape, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), myrect.colors, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
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

void init_triangleVertex() 	// 윈도우 좌표계 기준으로 삼각형 그리기
{
	my_Rectpos mWinPos[4] = { {200,150},{600,150},{200,450},{600,450} }; //윈도우 좌표계 기준 삼각형의 중점
	my_Rectpos mRectpos[4] = { 0, };
	for (int i = 0; i < 4; ++i) //삼각형의 중점을 opengl좌표로 변환
	{
		float x = 0, y = 0;
		myrect.mWinpos[i].x = mWinPos[i].x;
		myrect.mWinpos[i].y = mWinPos[i].y;
		convertWINXYOpenGlXY(mWinPos[i].x, mWinPos[i].y, &x, &y);
		mRectpos[i].x = x;
		mRectpos[i].y = y;
	}
	my_Vectex mVectex = { 0,0,0 };

	for (int i = 0; i < 4; ++i)			//i번째 사각형
	{
		for (int j = 0; j < 3; ++j)		//j번째 점
		{
			switch (j)	// i번째 사각형의 중심으로 부터 j번째 점 xyz계산
			{
			case 0:				//왼쪽점
				mVectex.x = mRectpos[i].x - RECT_SIZE;
				mVectex.y = mRectpos[i].y - RECT_SIZE;
				mVectex.z = 0;
				break;
			case 1:				//오른쪽점
				mVectex.x = mRectpos[i].x + RECT_SIZE;
				mVectex.y = mRectpos[i].y - RECT_SIZE;
				mVectex.z = 0;
				break;
			case 2:				//윗점
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
