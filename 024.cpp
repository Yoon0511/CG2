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
GLuint vertexShader;
GLuint fragmentShader;
GLuint uniformModel;
float r = 0;
float offset = 0.0f;
float trilncrment = 0.01f;
void mytimer(int value);
GLuint vao[16], vbo[30];
GLuint s_program;
float rect_size = RECT_SIZE;

float SphereObj[1000][18];
int Shpherindex = 0;
int ReadObj(FILE* objFile)
{
	char count[100], bind[100];
	int vertexNum = 0;
	int faceNum = 0;

	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		printf("%s\n", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexNum += 1;
		else if (count[0] == 'f' && count[1] == '\0')
			faceNum += 1;
		memset(count, '\0', sizeof(count));
	}
	//printf("vertexNum:%d faceNum:%d\n", vertexNum, faceNum);
	vertexNum += 1;
	glm::vec3* vertex;
	glm::vec3* normal;
	glm::uvec3* v, * t, * n;
	int vertIndex = 1;
	int normIndex = 1;
	int faceIndex = 0;
	v = (glm::uvec3*)malloc(sizeof(glm::vec3) * faceNum);
	t = (glm::uvec3*)malloc(sizeof(glm::vec3) * faceNum);
	n = (glm::uvec3*)malloc(sizeof(glm::vec3) * faceNum);
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNum);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3) * vertexNum);
	rewind(objFile);
	while (!feof(objFile)) {
		fscanf(objFile, "%s", bind);
		//printf("%s        count:%d\n", bind, a++);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(objFile, "%f %f %f",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);
			//printf("vertex[%d].x=%f\n", vertIndex, vertex[vertIndex].x);
			vertIndex++;
		}
		else if (bind[0] == 'v' && bind[1] == 'n') {
			fscanf(objFile, "%f %f %f",
				&normal[normIndex].x, &normal[normIndex].y, &normal[normIndex].z);
			//printf("normal[%d].x=%f\n", normIndex, normal[normIndex].x);
			normIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&v[faceIndex].x, &t[faceIndex].x, &n[faceIndex].x, &v[faceIndex].y, &t[faceIndex].y, &n[faceIndex].y, &v[faceIndex].z, &t[faceIndex].z, &n[faceIndex].z);
			//printf("%d/%d/%d %d/%d/%d %d/%d/%d\n", v[faceIndex].x, t[faceIndex].x, n[faceIndex].x, v[faceIndex].y, t[faceIndex].y, n[faceIndex].y, v[faceIndex].z, t[faceIndex].z, n[faceIndex].z);
			//printf("faceIndex:%d\n", faceIndex);
			faceIndex++;
		}
	}

	//printf("VertexN:%d , faceIndexN:%d\n", vertIndex, faceIndex);

	for (int i = 0; i < faceIndex; i++) {
		SphereObj[i][0] = vertex[v[i].x].x;
		SphereObj[i][1] = vertex[v[i].x].y;
		SphereObj[i][2] = vertex[v[i].x].z;
		SphereObj[i][3] = normal[n[i].x].x;
		SphereObj[i][4] = normal[n[i].x].y;
		SphereObj[i][5] = normal[n[i].x].z;

		SphereObj[i][6] = vertex[v[i].y].x;
		SphereObj[i][7] = vertex[v[i].y].y;
		SphereObj[i][8] = vertex[v[i].y].z;
		SphereObj[i][9] = normal[n[i].y].x;
		SphereObj[i][10] = normal[n[i].y].y;
		SphereObj[i][11] = normal[n[i].y].z;

		SphereObj[i][12] = vertex[v[i].z].x;
		SphereObj[i][13] = vertex[v[i].z].y;
		SphereObj[i][14] = vertex[v[i].z].z;
		SphereObj[i][15] = normal[n[i].z].x;
		SphereObj[i][16] = normal[n[i].z].y;
		SphereObj[i][17] = normal[n[i].z].z;
	}
	//for (int i = 0; i < faceIndex; i++) {
	//    printf("faceIndex:%d\nver:%f %f %f\nnor:%f %f %f\n\n", i, SphereObj[i][0], SphereObj[i][1], SphereObj[i][2], SphereObj[i][3], SphereObj[i][4], SphereObj[i][5]);
	//}
	return faceIndex;
}
class myRect {
public:
	GLUquadricObj* qobj;
	float xangle, yangle;
	float movex, movey, movez, scale_x, scale_y, scale_z, scalebyzero;
	float glux, gluy, gluz, gluscale_x, gluscale_y, gluscale_z, gluscalebyzero;
	float gluanglex, gluangley;
	float movespeed;
	float prev_x, prev_y, prev_z;
	bool moving, depth_test, tri_animation, tri_animation_ing, revolution, hex_up_animation, hex_front_animation, hex_front_animation_ing, hex_slied, hex_slied_ing;
	int move_index;
	float tri_angle, revolutiony, hex_up_anglex, hex_front_anglex, hex_slied_y;
	bool projection, projection_type;
	bool change_rect;
	float light_x, light_y, light_z;
	float light_rect_x, light_rect_y, light_rect_z;
	float light_angle;
	bool lighton;
public:
	myRect() {
		revolutiony = 0.0f;
		tri_angle = 0.0f;
		movespeed = 0.05;
		xangle = 0.0;
		yangle = 0.0f;
		movex = -0.0f;
		movey = 0.0f;
		movez = 0.0f;
		glux = 0.0f;
		gluy = -0.7f;
		gluz = 0.0f;
		gluanglex = 0;
		gluangley = 0;
		scale_x = scale_y = scale_z = gluscale_x = gluscale_y = gluscale_z = 1;
		scalebyzero = gluscalebyzero = 1;
		prev_x = prev_z = prev_y = 0;
		move_index = 0;
		moving = false;
		depth_test = true;
		tri_animation = false;
		tri_animation_ing = false;
		revolution = false;
		hex_up_anglex = hex_slied_y = 0;
		hex_up_animation = hex_front_animation = hex_front_animation_ing = hex_slied = hex_slied_ing = false;
		change_rect = false;
		light_x = 0.0f;
		light_y = 0.0f;
		light_z = 0.0f;
		light_rect_x = 0.0f;
		light_rect_y = 0.0f;
		light_rect_z = 0.0f;
		light_angle = 0.0f;
		lighton = true;
	};
	GLfloat hexahedronp[108] = {
		  0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25,		//앞
		  0.25,0.25,0.0, 0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,0.25,0.0, 0.25,0.25,0.0,			//뒤
		  0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,0.0, 0.25,-0.25,0.0, 0.25,0.25,0.0, 0.25,0.25,0.25,			//오
		 -0.25,0.25,0.25, -0.25,0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.25, -0.25,0.25,0.25,		//왼
		  0.25,0.25,0.25, 0.25,0.25,0.0, -0.25,0.25,0.0, -0.25,0.25,0.0, -0.25,0.25,0.25, 0.25,0.25,0.25,			//위
		  0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.0, -0.25,-0.25,0.0, 0.25,-0.25,0.0, 0.25,-0.25,0.25		//아래
	};
	GLfloat hex_front[18] = { 0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25 };		// 앞
	GLfloat hex_back[18] = { 0.25,0.25,-0.25, 0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,0.25,-0.25, 0.25,0.25,-0.25 };	// 뒤
	GLfloat hex_right[18] = { 0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,0.25,-0.25, 0.25,0.25,0.25 };		// 오
	GLfloat hex_left[18] = { -0.25,0.25,0.25, -0.25,0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,0.25, -0.25,0.25,0.25 };	// 왼
	GLfloat hex_up[18] = { 0.25,0.25,0.25, 0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,0.25, 0.25,0.25,0.25 };			// 위
	GLfloat hex_down[18] = { 0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,-0.25,0.25 };	// 아래

	GLfloat hex_front_color[18] = { 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0 };					//앞 001
	GLfloat hex_back_color[18] = { 0.0,0.0,-1.0, 0.0,0.0,-1.0, 0.0,0.0,-1.0, 0.0,0.0,-1.0, 0.0,0.0,-1.0, 0.0,0.0,-1.0 };			//뒤 00-1
	GLfloat hex_right_color[18] = { 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0 };					//오 100
	GLfloat hex_left_color[18] = { -1.0,0.0,0.0, -1.0,0.0,0.0, -1.0,0.0,0.0, -1.0,0.0,0.0, -1.0,0.0,0.0, -1.0,0.0,0.0 };			//왼 -100
	GLfloat hex_up_color[18] = { 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0 };					//위 010
	GLfloat hex_down_color[18] = { 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0 };			//아래 0-10

	//const GLfloat colors[108] = { //--- 삼각형 꼭지점 색상
	//	1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //앞
	//	0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //뒤
	//	0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, //오
	//	1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, //왼
	//	0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, //위
	//	0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0  //아래
	//};

	const GLfloat colors[108] = { //--- 삼각형 꼭지점 색상
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //앞
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //뒤
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //오
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //왼
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //위
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0  //아래
	};
	/*const GLfloat lines[18] = {
		-0.8,0.0,0.0, 0.8,0.0,0.0,
		0.0,0.8,0.0, 0.0,-0.8,0.0,
		0.0,-0.0,0.0, -0.0,0.0,-0.0,
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
	//	-0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0,  0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0, //앞 오
	//	-0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0,  0.0,0.0,0.0, -0.0,0.0,0.0, 0.0,0.0,0.0 //왼 
	//};

	GLfloat tri[54] = {
		-0.3,0.0,0.0, 0.3,0.0,-0.3, -0.3,0.0,-0.3,  -0.3,0.0,0.0, 0.3,0.0,0.0, 0.3,0.0,-0.3, //바닥
		-0.3,0.0,0.0, 0.3,0.0,0.0, 0.0,0.3,0.15,  0.3,0.0,0.0, 0.3,0.0,-0.3, 0.0,0.3,0.15, //뒤 오
		-0.3,0.0,-0.3, 0.3,0.0,-0.3, 0.0,0.3,0.15,  -0.3,0.0,0.0, -0.3,0.0,-0.3, 0.0,0.3,0.15, //앞 왼
	};

	GLfloat tri_pos_front[3][3] = { 0.0,0.2,0.0, -0.25,-0.25,0.25, 0.25,-0.25,0.25 };
	GLfloat tri_pos_back[3][3] = { 0.0,0.2,0.0, 0.25,-0.25,-0.25, -0.25,-0.25,-0.25 };
	GLfloat tri_pos_right[3][3] = { 0.0,0.2,0.0, 0.25,-0.25,0.25, 0.25,-0.25,-0.25 };
	GLfloat tri_pos_left[3][3] = { 0.0,0.2,0.0, -0.25,-0.25,-0.25, -0.25,-0.25,0.25 };
	GLfloat tri_pos_floor[6][3] = { 0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,-0.25,-0.25,-0.25,-0.25,0.25,-0.25,-0.25,0.25,-0.25,0.25 };

	GLfloat tri_color_front[3][3] = { 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0 };          //011
	GLfloat tri_color_back[3][3] = { 0.0,1.0,-1.0, 0.0,1.0,-1.0, 0.0,1.0,-1.0 };			//01-1
	GLfloat tri_color_right[3][3] = { 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0 };			//110
	GLfloat tri_color_left[3][3] = { -1.0,1.0,0.0, -1.0,1.0,0.0, -1.0,1.0,0.0 };			//-110
	GLfloat tri_color_floor[6][3] = { 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0, 0.0,-1.0,0.0 }; //0-10



	GLfloat tri_colors[54] = {
		0.0,0.0,0.0, 0.0,0.0,0.0, 0.0,0.0,0.0,  1.0,0.0,1.0, 1.0,0.0,1.0, 1.0,0.0,1.0,
		1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0,  0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0,
		0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0,  0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0,
	};

	GLfloat move_lines[7560];
	GLfloat move_lines_color[3] = {
		0.0,0.0,0.0
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

		//model = scalezero * moveMatrix * Tx * Ty * scale; //원점z축이동
		model = scalezero * Tx * Ty * moveMatrix * scale; //도형z축 이동

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
		mytrisetting();
		glDrawArrays(GL_TRIANGLES, 0, 3); //바닥1
		glDrawArrays(GL_TRIANGLES, 3, 3); //바닥2
		glDrawArrays(GL_TRIANGLES, 6, 3); //뒤
		glDrawArrays(GL_TRIANGLES, 9, 3); //오

		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Ty = glm::mat4(1.0f);
		glm::mat4 moveMatrix = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);
		glm::mat4 scalezero = glm::mat4(1.0f);
		glm::mat4 Rx = glm::mat4(1.0f);
		glm::mat4 Ry = glm::mat4(1.0f);
		glm::mat4 moveMatrix2 = glm::mat4(1.0f);
		glm::mat4 moveMatrix3 = glm::mat4(1.0f);

		moveMatrix = glm::translate(moveMatrix, glm::vec3(0.0, -0.0, 0.0));
		Tx = glm::rotate(Tx, glm::radians(xangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(yangle), glm::vec3(0.0f, 1.0f, 0.0f));

		moveMatrix2 = glm::translate(moveMatrix2, glm::vec3(-0.0, 0.0, 0.0));			//원점이동
		Rx = glm::rotate(Rx, glm::radians(tri_angle), glm::vec3(1.0f, 0.0f, 0.0f));		//x축에 대하여 회전
		Ry = glm::rotate(Ry, glm::radians(-yangle), glm::vec3(0.0f, 1.0f, 0.0f));
		moveMatrix3 = glm::translate(moveMatrix3, glm::vec3(0.0, -0.0, 0.0));			//역 이동


		model = Tx * Ty * moveMatrix * Ry * moveMatrix2 * Rx * moveMatrix3;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 12, 3); //앞

		mytrisetting();
		glDrawArrays(GL_TRIANGLES, 15, 3); //왼
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

		model = scalezero * Tx * Ty * moveMatrix * scale; //도형z축 이동

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	}

	void mytrisetting()
	{
		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Ty = glm::mat4(1.0f);
		glm::mat4 moveMatrix = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);
		glm::mat4 scalezero = glm::mat4(1.0f);


		moveMatrix = glm::translate(moveMatrix, glm::vec3(0.0, -0.0, 0));
		Tx = glm::rotate(Tx, glm::radians(xangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(yangle), glm::vec3(0.0f, 1.0f, 0.0f));

		model = Tx * Ty * moveMatrix; //도형z축 이동

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
		movex = -0.0f;
		movey = 0.0f;
		movez = 0.0f;
		glux = 0.0f;
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
			//glVertex3f(x + -0.0, 0.7,y);
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
			prev_y = movey;
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
			move_index = 0;
			movex -= movex;
			movez -= movez;
			movey -= movey;

			movex += prev_x;
			movez += prev_z;
			movey += prev_y;
		}
	}
};

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

	myrect.setline();

	FILE* sphere = fopen("sphere.obj", "rb");
	if (sphere == NULL) {
		printf("File open failed...");
		exit(1);
	}
	Shpherindex = ReadObj(sphere);
	fclose(sphere);

	InitShader();
	InitBuffer();

	glutTimerFunc(1.0f, mytimer, 0);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(0, 0, 0, 0); // 바탕색을 ‘blue’ 로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	glUseProgram(s_program);

	if (myrect.depth_test)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	//투영
	glm::mat4 projection = glm::mat4(1.0f);
	//if (myrect.projection)
	//{
	//	if (myrect.projection_type) //직각
	//	{
	//		projection = glm::ortho(-0.0f, 0.0f, -0.0f, 0.0f, -2.0f, 2.0f);
	//	}
	//	else //원근
	//	{
	//		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	//		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- 공간을 약간 뒤로 미뤄줌
	//	}
	//}
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -7.0)); //--- 공간을 약간 뒤로 미뤄줌
	projection = glm::rotate(projection, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	projection = glm::rotate(projection, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	
	//xy축
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Rm = glm::mat4(1.0f);
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 mo = glm::mat4(1.0f);
	glm::mat4 moveMatrix = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 init = glm::mat4(1.0f);
	
	model = init;

	moveMatrix = glm::translate(moveMatrix, glm::vec3(myrect.light_x, myrect.light_y, myrect.light_z));
	Ty = glm::rotate(Ty, glm::radians(myrect.light_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(myrect.yangle), glm::vec3(1.0f, 0.0f, 0.0f));
	int lightmodelLocation = glGetUniformLocation(s_program, "lightmodel");
	model = Ty;
	glUniformMatrix4fv(lightmodelLocation, 1, GL_FALSE, glm::value_ptr(model));

	int lightPosLocation = glGetUniformLocation(s_program, "lightPos");
	glUniform3f(lightPosLocation, 0.0f,0.0f,3.0f + myrect.light_z);
	
	int lightColorLocation = glGetUniformLocation(s_program, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	int objColorLocation = glGetUniformLocation(s_program, "objectColor");
	glUniform3f(objColorLocation, 0.0, 1.0, 0.0);

	model = init;
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(vao[12]);
	for (int i = 0; i < Shpherindex; ++i)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}

	model = init;
	Rm = init, mo = init, Tx = init, Ty = init, Ry = init;
	moveMatrix = init;
	//moveMatrix = glm::translate(moveMatrix, glm::vec3(myrect.light_x, myrect.light_y, myrect.light_z));
	moveMatrix = glm::translate(moveMatrix, glm::vec3(0.0f,0.0f,3.0f + myrect.light_z));
	Ty = glm::rotate(Ty, glm::radians(myrect.xangle + myrect.light_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(myrect.yangle), glm::vec3(1.0f, 0.0f, 0.0f));
	Ry = init;
	//Ry = glm::rotate(Ry, glm::radians(myrect.light_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	//사각형
	for (int i = 6; i < 12; ++i)
	{
		glBindVertexArray(vao[i]);
		float movey = myrect.hex_slied_y;
		if (i == 6 || i == 11) //뒤 아래
		{
			model = Tx * Ty * moveMatrix * Ry;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		switch (i)
		{
		case 7: //앞
			Rx = glm::rotate(Rx, glm::radians(myrect.hex_front_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
			Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
			mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
			model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 8: //오
			Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 9: //왼
			Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 10: //위
			Rx = glm::rotate(Rx, glm::radians(myrect.hex_up_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
			Rm = glm::translate(Rm, glm::vec3(0.0f, -0.25f, 0.0f));
			mo = glm::translate(mo, glm::vec3(0.0f, 0.25f, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	glUniform3f(lightPosLocation, 0.0f, 0.0f, 2.5f);
	Ry = init;
	Rm = init, mo = init, Tx = init, Ty = init;
	if (myrect.change_rect)
	{
		moveMatrix = init;
		moveMatrix = glm::translate(moveMatrix, glm::vec3(0, 0, 0));
		Ty = glm::rotate(Ty, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Tx = glm::rotate(Tx, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Ry = glm::rotate(Ry, glm::radians(myrect.revolutiony), glm::vec3(0.0f, 1.0f, 0.0f));
		for (int i = 0; i < 5; ++i)
		{
			glBindVertexArray(vao[i]);
			switch (i)
			{
			case 0: //삼각형 앞
				Rx = glm::rotate(Rx, glm::radians(myrect.tri_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, -0.25f));
				mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, 0.25f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 1: // 뒤
				Rx = glm::rotate(Rx, glm::radians(-myrect.tri_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
				mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 2: // 왼
				Rx = glm::rotate(Rx, glm::radians(myrect.tri_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				Rm = glm::translate(Rm, glm::vec3(0.25f, 0.25f, 0.0f));
				mo = glm::translate(mo, glm::vec3(-0.25f, -0.25f, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 3: // 오
				Rx = glm::rotate(Rx, glm::radians(-myrect.tri_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				Rm = glm::translate(Rm, glm::vec3(-0.25f, 0.25f, 0.0f));
				mo = glm::translate(mo, glm::vec3(0.25f, -0.25f, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			}

			if (i < 4)
			{
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
			else
			{
				model = Tx * Ty * moveMatrix * Ry;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
	}
	else
	{
		Rm = init, mo = init, Tx = init, Ty = init;
		Ry = init;
		moveMatrix = init;
		moveMatrix = glm::translate(moveMatrix, glm::vec3(0, 0, 0));
		Ty = glm::rotate(Ty, glm::radians(myrect.xangle), glm::vec3(0.0f, 1.0f, 0.0f));
		Tx = glm::rotate(Tx, glm::radians(myrect.yangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ry = glm::rotate(Ry, glm::radians(myrect.revolutiony), glm::vec3(0.0f, 1.0f, 0.0f));
		//사각형
		for (int i = 6; i < 12; ++i)
		{
			glBindVertexArray(vao[i]);
			float movey = myrect.hex_slied_y;
			if (i == 6 || i == 11) //뒤 아래
			{
				model = Tx * Ty * moveMatrix * Ry;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			switch (i)
			{
			case 7: //앞
				Rx = glm::rotate(Rx, glm::radians(myrect.hex_front_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
				mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 8: //오
				Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 9: //왼
				Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 10: //위
				Rx = glm::rotate(Rx, glm::radians(myrect.hex_up_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, -0.25f, 0.0f));
				mo = glm::translate(mo, glm::vec3(0.0f, 0.25f, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			}
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	
	glutSwapBuffers(); // 화면에 출력하기
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

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
void make_vertexShaders()
{
	GLchar* vertexsource;
	vertexsource = filetobuf("light_vertex.glsl");
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
	fragmentsource = filetobuf("light_fragment.glsl"); // 프래그세이더 읽어오기
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
		std::cerr << "ERROR: fragment shader error\n" << errorLog << std::endl;
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
	glGenVertexArrays(14, vao);
	glBindVertexArray(vao[0]); //삼각뿔 앞

	glGenBuffers(27, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_front), myrect.tri_pos_front, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_front), myrect.tri_color_front, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[1]); //삼각뿔 뒤
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_back), myrect.tri_pos_back, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_back), myrect.tri_color_back, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[2]); //삼각뿔 왼
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_left), myrect.tri_pos_left, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_left), myrect.tri_color_left, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[3]); //삼각뿔 오
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_right), myrect.tri_pos_right, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_right), myrect.tri_color_right, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[4]); //삼각뿔 바닥
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_floor), myrect.tri_pos_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_floor), myrect.tri_color_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindVertexArray(vao[5]); //xyz축
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines), myrect.lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines_color), myrect.lines_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[6]); //사각형 앞
	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_front), myrect.hex_front, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_front_color), myrect.hex_front_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[7]); //사각형 뒤
	glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_back), myrect.hex_back, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_back_color), myrect.hex_back_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[8]); //사각형 오
	glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_right), myrect.hex_right, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[17]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_right_color), myrect.hex_right_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[9]); //사각형 왼
	glBindBuffer(GL_ARRAY_BUFFER, vbo[18]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_left), myrect.hex_left, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[19]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_left_color), myrect.hex_left_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[10]); //사각형 위
	glBindBuffer(GL_ARRAY_BUFFER, vbo[20]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_up), myrect.hex_up, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[21]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_up_color), myrect.hex_up_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[11]); //사각형 아래
	glBindBuffer(GL_ARRAY_BUFFER, vbo[22]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_down), myrect.hex_down, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[23]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_down_color), myrect.hex_down_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindVertexArray(vao[12]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[24]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SphereObj), SphereObj, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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

	uniformModel = glGetUniformLocation(s_program, "transform");

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
	case 'h':
		myrect.depth_test = !myrect.depth_test;
		break;
	case 'y':
		myrect.revolution = !myrect.revolution;
		break;
	case 't':
		myrect.hex_up_animation = !myrect.hex_up_animation;
		break;
	case 'f':
		if (myrect.hex_front_animation_ing == false)
		{
			myrect.hex_front_animation_ing = true;
			myrect.hex_front_animation = !myrect.hex_front_animation;

		}
		break;
	case '1':
		if (myrect.hex_slied_ing == false)
		{
			myrect.hex_slied_ing = true;
			myrect.hex_slied = !myrect.hex_slied;
		}
		break;
	case'0':
		myrect.projection = !myrect.projection;
		break;
	case 'p':
		myrect.projection_type = !myrect.projection_type;
		break;
	case 'n':
		myrect.change_rect = !myrect.change_rect;
		break;
	case 'a':
		myrect.light_x -= 0.1f;
		break;
	case 's':
		myrect.light_y -= 0.1f;
		break;
	case 'd':
		myrect.light_x += 0.1f;
		break;
	case 'w':
		myrect.light_y += 0.1f;
		break;
	case 'q':
		myrect.light_z += 0.1f;
		break;
	case 'e':
		myrect.light_z -= 0.1f;
		break;
	case 'i':
		myrect.light_rect_y += 0.1f;
		break;
	case 'k':
		myrect.light_rect_y -= 0.1f;
		break;
	case 'j':
		myrect.light_rect_x -= 0.1f;
		break;
	case 'l':
		myrect.light_rect_x += 0.1f;
		break;
	case 'u':
		myrect.light_rect_z += 0.1f;
		break;
	case 'o':
		myrect.light_rect_z -= 0.1f;
		break;
	case 'b':
		printf("%f %f %f \n", myrect.light_x, myrect.light_y, myrect.light_z);
		printf("%f %f %f \n", myrect.light_rect_x, myrect.light_rect_y, myrect.light_rect_z);
		break;
	case '3':
		myrect.xangle += 3.0f;
		break;
	case '4':
		myrect.yangle += 3.0f;
		break;
	case '5':
		myrect.light_angle += 3.0f;
		break;
	case 'm':
		myrect.lighton = !myrect.lighton;
		int lighton = glGetUniformLocation(s_program, "light_on");

		if (myrect.lighton)
		{
			glUniform3f(lighton, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glUniform3f(lighton, 1.0f, 1.0f, 1.0f);
		}
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
	if (myrect.tri_animation_ing)
	{
		if (myrect.tri_animation)
		{
			myrect.tri_angle += 1.0f;
			if (myrect.tri_angle >= 238)
			{
				myrect.tri_animation_ing = false;
			}
		}
		else
		{
			myrect.tri_angle -= 1.0f;
			if (myrect.tri_angle <= 0)
			{
				myrect.tri_animation_ing = false;
			}
		}
	}

	if (myrect.revolution)
	{
		myrect.revolutiony += 1.0f;
		if (myrect.revolutiony >= 360.0f)
			myrect.revolutiony = 0;
	}

	if (myrect.hex_up_animation)
	{
		myrect.hex_up_anglex += 1.0f;
		if (myrect.hex_up_anglex >= 360.0f)
			myrect.hex_up_anglex = 0;
	}

	if (myrect.hex_front_animation_ing)
	{
		if (myrect.hex_front_animation)
		{
			myrect.hex_front_anglex -= 1.0f;
			if (myrect.hex_front_anglex <= -90.0f)
			{
				myrect.hex_front_animation_ing = false;
			}
		}
		else
		{
			myrect.hex_front_anglex += 1.0f;
			if (myrect.hex_front_anglex >= 0.0f)
			{
				myrect.hex_front_animation_ing = false;
			}
		}
	}

	if (myrect.hex_slied_ing)
	{
		if (myrect.hex_slied)
		{
			myrect.hex_slied_y += 0.01f;
			if (myrect.hex_slied_y >= 0.45f)
				myrect.hex_slied_ing = false;
		}
		else
		{
			myrect.hex_slied_y -= 0.01f;
			if (myrect.hex_slied_y <= 0.0f)
				myrect.hex_slied_ing = false;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10.0f, mytimer, 0);
}