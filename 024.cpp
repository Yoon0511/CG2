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
GLuint vao[13], vbo[26];
GLuint s_program;
float rect_size = RECT_SIZE;

float vertices[] = { //--- ���ؽ� �Ӽ�: ��ǥ��(FragPos), �븻�� (Normal)
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

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
public:
	myRect() {
		revolutiony = 0.0f;
		tri_angle = 0.0f;
		movespeed = 0.05;
		xangle = -35.0f;
		yangle = -10.0f;
		movex = -0.5f;
		movey = 0.5f;
		movez = 0.0f;
		glux = 0.5f;
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
	};
	GLfloat hexahedronp[108] = {
		  0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25,		//��
		  0.25,0.25,0.0, 0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,0.25,0.0, 0.25,0.25,0.0,			//��
		  0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,0.0, 0.25,-0.25,0.0, 0.25,0.25,0.0, 0.25,0.25,0.25,			//��
		 -0.25,0.25,0.25, -0.25,0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.0, -0.25,-0.25,0.25, -0.25,0.25,0.25,		//��
		  0.25,0.25,0.25, 0.25,0.25,0.0, -0.25,0.25,0.0, -0.25,0.25,0.0, -0.25,0.25,0.25, 0.25,0.25,0.25,			//��
		  0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.0, -0.25,-0.25,0.0, 0.25,-0.25,0.0, 0.25,-0.25,0.25		//�Ʒ�
	};
	GLfloat hex_front[18] = { 0.25,0.25,0.25, -0.25,0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,0.25, 0.25,-0.25,0.25, 0.25,0.25,0.25 };		// ��
	GLfloat hex_back[18] = { 0.25,0.25,-0.25, 0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,0.25,-0.25, 0.25,0.25,-0.25 };	// ��
	GLfloat hex_right[18] = { 0.25,0.25,0.25, 0.25,-0.25,0.25, 0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,0.25,-0.25, 0.25,0.25,0.25 };		// ��
	GLfloat hex_left[18] = { -0.25,0.25,0.25, -0.25,0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, -0.25,-0.25,0.25, -0.25,0.25,0.25 };	// ��
	GLfloat hex_up[18] = { 0.25,0.25,0.25, 0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,-0.25, -0.25,0.25,0.25, 0.25,0.25,0.25 };			// ��
	GLfloat hex_down[18] = { 0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,-0.25, -0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25,-0.25,0.25 };	// �Ʒ�

	GLfloat hex_front_color[18] = { 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0 };		//��
	GLfloat hex_back_color[18] = { 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0 };		//��
	GLfloat hex_right_color[18] = { 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0 };		//��
	GLfloat hex_left_color[18] = { 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0 };		//��
	GLfloat hex_up_color[18] = { 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0 };		//��
	GLfloat hex_down_color[18] = { 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5 };		//�Ʒ�

	//const GLfloat colors[108] = { //--- �ﰢ�� ������ ����
	//	1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0, //��
	//	0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
	//	0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0, //��
	//	1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, //��
	//	0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, 0.0,1.0,1.0, //��
	//	0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5  //�Ʒ�
	//};

	const GLfloat colors[108] = { //--- �ﰢ�� ������ ����
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, //��
		0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0  //�Ʒ�
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

	GLfloat tri_pos_front[3][3] = { 0.0,0.2,0.0, -0.25,-0.25,0.25, 0.25,-0.25,0.25 };
	GLfloat tri_pos_back[3][3] = { 0.0,0.2,0.0, 0.25,-0.25,-0.25, -0.25,-0.25,-0.25 };
	GLfloat tri_pos_right[3][3] = { 0.0,0.2,0.0, 0.25,-0.25,0.25, 0.25,-0.25,-0.25 };
	GLfloat tri_pos_left[3][3] = { 0.0,0.2,0.0, -0.25,-0.25,-0.25, -0.25,-0.25,0.25 };
	GLfloat tri_pos_floor[6][3] = { 0.25,-0.25,0.25, -0.25,-0.25,0.25, -0.25,-0.25,-0.25,-0.25,-0.25,-0.25,0.25,-0.25,-0.25,0.25,-0.25,0.25 };

	GLfloat tri_color_front[3][3] = { 0.0,1.0,0.0, 0.0,1.0,0.0, 0.0,1.0,0.0 };
	GLfloat tri_color_back[3][3] = { 1.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,0.0 };
	GLfloat tri_color_right[3][3] = { 0.0,0.0,1.0, 0.0,0.0,1.0, 0.0,0.0,1.0 };
	GLfloat tri_color_left[3][3] = { 1.0,0.5,0.0, 1.0,0.5,0.0, 1.0,0.5,0.0 };
	GLfloat tri_color_floor[6][3] = { 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0, 1.0,1.0,0.0 };



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

	void mydraw_tri()
	{
		glBindVertexArray(vao[2]);
		mytrisetting();
		glDrawArrays(GL_TRIANGLES, 0, 3); //�ٴ�1
		glDrawArrays(GL_TRIANGLES, 3, 3); //�ٴ�2
		glDrawArrays(GL_TRIANGLES, 6, 3); //��
		glDrawArrays(GL_TRIANGLES, 9, 3); //��

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

		moveMatrix = glm::translate(moveMatrix, glm::vec3(0.5, -0.5, 0.0));
		Tx = glm::rotate(Tx, glm::radians(xangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(yangle), glm::vec3(0.0f, 1.0f, 0.0f));

		moveMatrix2 = glm::translate(moveMatrix2, glm::vec3(-0.5, 0.5, 0.0));			//�����̵�
		Rx = glm::rotate(Rx, glm::radians(tri_angle), glm::vec3(1.0f, 0.0f, 0.0f));		//x�࿡ ���Ͽ� ȸ��
		Ry = glm::rotate(Ry, glm::radians(-yangle), glm::vec3(0.0f, 1.0f, 0.0f));
		moveMatrix3 = glm::translate(moveMatrix3, glm::vec3(0.5, -0.5, 0.0));			//�� �̵�


		model = Tx * Ty * moveMatrix * Ry * moveMatrix2 * Rx * moveMatrix3;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 12, 3); //��

		mytrisetting();
		glDrawArrays(GL_TRIANGLES, 15, 3); //��
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

		model = scalezero * Tx * Ty * moveMatrix * scale; //����z�� �̵�

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


		moveMatrix = glm::translate(moveMatrix, glm::vec3(0.5, -0.5, 0));
		Tx = glm::rotate(Tx, glm::radians(xangle), glm::vec3(1.0f, 0.0f, 0.0f));
		Ty = glm::rotate(Ty, glm::radians(yangle), glm::vec3(0.0f, 1.0f, 0.0f));

		model = Tx * Ty * moveMatrix; //����z�� �̵�

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
	glClearColor(0, 0, 0, 0); // �������� ��blue�� �� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glUseProgram(s_program);

	if (myrect.depth_test)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� ��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	//����
	glm::mat4 projection = glm::mat4(1.0f);
	//if (myrect.projection)
	//{
	//	if (myrect.projection_type) //����
	//	{
	//		projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -2.0f, 2.0f);
	//	}
	//	else //����
	//	{
	//		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	//		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- ������ �ణ �ڷ� �̷���
	//	}
	//}
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 500.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- ������ �ణ �ڷ� �̷���
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, -myrect.light_x + myrect.light_rect_x, -myrect.light_y + myrect.light_rect_y, myrect.light_z + myrect.light_rect_z);
	int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	int objColorLocation = glGetUniformLocation(s_program, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
	glUniform3f(objColorLocation, 0.0, 1.0, 0.0);
	//xy��
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Rm = glm::mat4(1.0f);
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 mo = glm::mat4(1.0f);
	glm::mat4 moveMatrix = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 init = glm::mat4(1.0f);

	
	/*moveMatrix = glm::translate(moveMatrix, glm::vec3(0, 0, 0));
	Ty = glm::rotate(Ty, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = Tx * Ty;
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(vao[5]);
	glDrawArrays(GL_LINES, 0, 6);*/

	//����
	

	//�ﰢ��
	Rm = init, mo = init, Tx = init, Ty = init;
	moveMatrix = init;
	moveMatrix = glm::translate(moveMatrix, glm::vec3(myrect.light_x + myrect.light_rect_x, myrect.light_y + myrect.light_rect_y, myrect.light_z + myrect.light_rect_z));
	Ty = glm::rotate(Ty, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//�簢��
	for (int i = 6; i < 12; ++i)
	{
		glBindVertexArray(vao[i]);
		float movey = myrect.hex_slied_y;
		if (i == 6 || i == 11) //�� �Ʒ�
		{
			model = Tx * Ty * moveMatrix * Ry;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		switch (i)
		{
		case 7: //��
			Rx = glm::rotate(Rx, glm::radians(myrect.hex_front_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
			Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
			mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
			model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 8: //��
			Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 9: //��
			Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 10: //��
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

	Rm = init, mo = init, Tx = init, Ty = init;
	moveMatrix = init;
	moveMatrix = glm::translate(moveMatrix, glm::vec3(myrect.light_x - 0.1f, myrect.light_y, myrect.light_z));
	Ty = glm::rotate(Ty, glm::radians(myrect.xangle), glm::vec3(0.0f, 1.0f, 0.0f));
	Tx = glm::rotate(Tx, glm::radians(myrect.yangle), glm::vec3(1.0f, 0.0f, 0.0f));

	//�簢��
	for (int i = 6; i < 12; ++i)
	{
		glBindVertexArray(vao[i]);
		float movey = myrect.hex_slied_y;
		if (i == 6 || i == 11) //�� �Ʒ�
		{
			model = Tx * Ty * moveMatrix * Ry;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		switch (i)
		{
		case 7: //��
			Rx = glm::rotate(Rx, glm::radians(myrect.hex_front_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
			Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
			mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
			model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 8: //��
			Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 9: //��
			Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
			model = Tx * Ty * moveMatrix * Ry * Rm;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Rm = init, mo = init, Rx = init;
			break;
		case 10: //��
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
			case 0: //�ﰢ�� ��
				Rx = glm::rotate(Rx, glm::radians(myrect.tri_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, -0.25f));
				mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, 0.25f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 1: // ��
				Rx = glm::rotate(Rx, glm::radians(-myrect.tri_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
				mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 2: // ��
				Rx = glm::rotate(Rx, glm::radians(myrect.tri_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				Rm = glm::translate(Rm, glm::vec3(0.25f, 0.25f, 0.0f));
				mo = glm::translate(mo, glm::vec3(-0.25f, -0.25f, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 3: // ��
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
		moveMatrix = init;
		moveMatrix = glm::translate(moveMatrix, glm::vec3(0, 0, 0));
		Ty = glm::rotate(Ty, glm::radians(myrect.xangle), glm::vec3(0.0f, 1.0f, 0.0f));
		Tx = glm::rotate(Tx, glm::radians(myrect.yangle), glm::vec3(1.0f, 0.0f, 0.0f));

		//�簢��
		for (int i = 6; i < 12; ++i)
		{
			glBindVertexArray(vao[i]);
			float movey = myrect.hex_slied_y;
			if (i == 6 || i == 11) //�� �Ʒ�
			{
				model = Tx * Ty * moveMatrix * Ry;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			switch (i)
			{
			case 7: //��
				Rx = glm::rotate(Rx, glm::radians(myrect.hex_front_anglex), glm::vec3(1.0f, 0.0f, 0.0f));
				Rm = glm::translate(Rm, glm::vec3(0.0f, 0.25f, 0.25f));
				mo = glm::translate(mo, glm::vec3(0.0f, -0.25f, -0.25f));
				model = Tx * Ty * moveMatrix * Ry * mo * Rx * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 8: //��
				Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 9: //��
				Rm = glm::translate(Rm, glm::vec3(0.0f, movey, 0.0f));
				model = Tx * Ty * moveMatrix * Ry * Rm;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Rm = init, mo = init, Rx = init;
				break;
			case 10: //��
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
	fragmentsource = filetobuf("light_fragment.glsl"); // �����׼��̴� �о����
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
	glGenVertexArrays(13, vao);
	glBindVertexArray(vao[0]); //�ﰢ�� ��

	glGenBuffers(26, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_front), myrect.tri_pos_front, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_front), myrect.tri_color_front, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[1]); //�ﰢ�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_back), myrect.tri_pos_back, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_back), myrect.tri_color_back, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[2]); //�ﰢ�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_left), myrect.tri_pos_left, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_left), myrect.tri_color_left, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[3]); //�ﰢ�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_right), myrect.tri_pos_right, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_right), myrect.tri_color_right, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[4]); //�ﰢ�� �ٴ�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_pos_floor), myrect.tri_pos_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.tri_color_floor), myrect.tri_color_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindVertexArray(vao[5]); //xyz��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines), myrect.lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.lines_color), myrect.lines_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[6]); //�簢�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_front), myrect.hex_front, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_front_color), myrect.hex_front_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[7]); //�簢�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_back), myrect.hex_back, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_back_color), myrect.hex_back_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[8]); //�簢�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_right), myrect.hex_right, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[17]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_right_color), myrect.hex_right_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[9]); //�簢�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[18]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_left), myrect.hex_left, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[19]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_left_color), myrect.hex_left_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[10]); //�簢�� ��
	glBindBuffer(GL_ARRAY_BUFFER, vbo[20]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_up), myrect.hex_up, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[21]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_up_color), myrect.hex_up_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[11]); //�簢�� �Ʒ�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[22]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_down), myrect.hex_down, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[23]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_down_color), myrect.hex_down_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[12]); //����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[24]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[25]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(myrect.hex_down_color), myrect.hex_down_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glUseProgram(s_program);
	
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

	uniformModel = glGetUniformLocation(s_program, "transform");

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