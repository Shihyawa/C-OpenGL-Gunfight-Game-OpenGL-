/*------------------------------头文件------------------------------*/
#define GLUT_DISABLE_ATEXIT_HACK    //不加就报错，这两行都是
#define GLEW_STATIC
#define BMP_Header_Length 54
#define WindowWidth  GetSystemMetrics(SM_CXSCREEN)
#define WindowHeight GetSystemMetrics(SM_CYSCREEN)
/*------------------------------基础处理------------------------------*/
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windef.h>
#include <wingdi.h>
#include <windows.h>
/*-------------------------------OpenGL链接库及头文件，还加了个EGE图形库-----------------------------*/
#include <gl\glu.h>
#include <gl\glut.h>
#include <gl\glaux.h>
#include <graphics.h>
#include <glm\glm.hpp>
#include <glfw\glfw3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "glaux.lib")           //链接OpenGL库
#pragma comment (lib,"gdi32.lib")
#pragma comment (lib,"User32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment (lib, "glfw3.lib")
#pragma comment (lib, "assimp.lib")
#pragma comment (lib, "glew32s.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32mxs.lib")
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define PI 3.1415926535897932384626     //必须加后面，加前面会报错，我也不知道为什么
using namespace std;            //我也不知道是什么



/*__________________________________________定义变量_________________________________________________*/
int sjfx, sjfx2, sjfx3, sjfx4;
int FX;
static float r = 1.5, h = 0.0; //r是视点绕y轴的半径，h是视点高度即在y轴上的坐标
static float c = PI / 180.0; //弧度和角度转换参数
int hth1 = 1, hth2 = 1, hth3 = 1, hth4 = 1;
static GLclampd EyeFar = 60.0;
static int du = 90, OriX = -1, OriY = -1;
static int PLYERY = 8, DRY1 = 30, DRY2 = 22, DRY3 = 43, DRY4 = 70, PLYERX = 8, DRX1 = 33, DRX2 = 41, DRX3 = 56,
           DRX4 = 68;
static GLfloat sx = 0, sz = 0, pyl1, pyl2, pyl12, pyl22, pyl13, pyl23, pyl14, pyl24;
const float lkdx = (float)GetSystemMetrics(SM_CXSCREEN) / (float)GetSystemMetrics(SM_CYSCREEN);
/*__________________________________________定义变量End______________________________________________*/
void FullScreen() {
	HWND hwnd = FindWindow(NULL, "OpenGL枪战游戏");
	LONG l_WinStyle = GetWindowLong(hwnd, GWL_STYLE); /* 获取窗口信息 */
	/* 设置窗口信息 最大化 取消标题栏及边框 */
	SetWindowLong(hwnd, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER);
}

static void Resize(int width, int height) {
	const float ar = (float)width / (float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, EyeFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.001f, 0.0f); //设置观察点
}

void Init() {
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_POINT_SMOOTH);//对点进行抗锯齿处理
	glEnable(GL_LINE_SMOOTH);//对线进行抗锯齿处理
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//混合
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	typedef BOOL (APIENTRY * PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);

	FullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);
	SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
}

void bmppm(void) {
	FILE    *pDummyFile;  //指向另一bmp文件，用于复制它的文件头和信息头数据
	FILE    *pWritingFile;  //指向要保存截图的bmp文件
	GLubyte *pPixelData;    //指向新的空的内存，用于保存截图bmp文件数据
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;   //BMP文件数据总长度

	// 计算像素数据的实际长度
	i = WindowWidth * 3;   // 得到每一行的像素数据长度
	while ( i % 4 != 0 )   // 补充数据，直到i是的倍数
		++i;
	PixelDataLength = i * WindowHeight;  //补齐后的总位数

	// 分配内存和打开文件
	pPixelData = (GLubyte *)malloc(PixelDataLength);
	if ( pPixelData == 0 )
		exit(0);

	pDummyFile = fopen("Data\\BmpData\\getPM.data", "rb");//只读形式打开
	if ( pDummyFile == 0 ) {
		ShowWindow(GetForegroundWindow(), SW_HIDE);
		MessageBox(GetForegroundWindow(), "Error:No DataFile For Game!\nYou Need Redownload Game!", "OpenGLGame-Error",
		           MB_ICONERROR);
		exit(0);
	}
	pWritingFile = fopen("Game\\Pictures\\MyGamePicture.bmp", "wb"); //只写形式打开
	if ( pWritingFile == 0 ) {
		ShowWindow(GetForegroundWindow(), SW_HIDE);
		MessageBox(GetForegroundWindow(), "Error:No DataFile For Game!\nYou Need Redownload Game!", "OpenGLGame-Error",
		           MB_ICONERROR);
		exit(0);
	}
	//把读入的bmp文件的文件头和信息头数据复制，并修改宽高数据
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);  //读取文件头和信息头，占据54字节
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET); //移动到0X0012处，指向图像宽度所在内存
	i = WindowWidth;
	j = WindowHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 读取当前画板上图像的像素数据
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //设置4位对齐方式
	glReadPixels(0, 0, WindowWidth, WindowHeight,
	             GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

	// 写入像素数据
	fseek(pWritingFile, 0, SEEK_END);
	//把完整的BMP文件数据写入pWritingFile
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// 释放内存和关闭文件
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}

void DrawCube() {
	glutSolidCube(1.0);
}

void DrawCld(double radius) {
	glutSolidSphere(radius, 100, 100);//实心球
}

void drawqhe() {
	glTranslatef(0, 1, 0);
	glScalef(5, 4, 1);
	DrawCube();
	glTranslatef(0, -0.25, 0);
	glScalef(0.2, 0.25, 1);
}

void drawqsu() {
	glTranslatef(0, 1, 0);
	glScalef(1, 4, 5);
	DrawCube();
	glTranslatef(0, -0.25, 0);
	glScalef(1, 0.25, 0.2);
}

void drawdr() {
	if (hth1 <= 0) {
		hth1 = 0;
	} else {
		sjfx = 0;
		sjfx = rand() % 50;
		if (sjfx == 1) {
			pyl1 += 0.36f;
			DRX1++;
		}
		if (sjfx == 2) {
			pyl1 -= 0.36f;
			DRX1--;
		}
		if (sjfx == 3) {
			pyl2 -= 0.36f;
			DRY1--;
		}
		if (sjfx == 4) {
			pyl2 += 0.36f;
			DRY1++;
		}
		glTranslatef(0, 0, 0.1f);
		glTranslatef(pyl1, 0, pyl2);
		glTranslatef(14.4f, 1.8f, 14.4f);
		glColor3ub(255, 0, 0);
		glScalef(0.65f, 0.65f, 0.65f);
		DrawCld(1);
//身体
		glScalef(1.55f, 1.55f, 1.55f);
		glTranslatef(0, -0.8f, 0);
		DrawCube();
		glTranslatef(-15, -1, -14);
	}
}

void drawdr2() {
	if (hth2 <= 0) {
		hth2 = 0;
	} else {
		sjfx2 = 0;
		sjfx2 = rand() % 50;
		if (sjfx2 == 1) {
			pyl12 += 0.36f;
		}
		if (sjfx2 == 2) {
			pyl12 -= 0.36f;
		}
		if (sjfx2 == 3) {
			pyl22 -= 0.36f;
		}
		if (sjfx2 == 4) {
			pyl22 += 0.36f;
		}
		glTranslatef(0, 0, 0.1f);
		glTranslatef(pyl12, 0, pyl22);
		glTranslatef(14.4f, 1.8f, 14.4f);
		glColor3ub(255, 0, 0);
		glScalef(0.65f, 0.65f, 0.65f);
		DrawCld(1);
//身体
		glScalef(1.55f, 1.55f, 1.55f);
		glTranslatef(0, -0.8f, 0);
		DrawCube();
		glTranslatef(-15, -1, -14);
	}
}

void drawdr3() {
	if (hth3 <= 0) {
		hth3 = 0;
	} else {
		sjfx3 = 0;
		sjfx3 = rand() % 50;
		if (sjfx3 == 1) {
			pyl13 += 0.36f;
		}
		if (sjfx3 == 2) {
			pyl13 -= 0.36f;
		}
		if (sjfx3 == 3) {
			pyl23 -= 0.36f;
		}
		if (sjfx3 == 4) {
			pyl23 += 0.36f;
		}
		glTranslatef(0, 0, 0.1f);
		glTranslatef(pyl13, 0, pyl23);
		glTranslatef(14.4f, 1.8f, 14.4f);
		glColor3ub(255, 0, 0);
		glScalef(0.65f, 0.65f, 0.65f);
		DrawCld(1);
//身体
		glScalef(1.55f, 1.55f, 1.55f);
		glTranslatef(0, -0.8f, 0);
		DrawCube();
		glTranslatef(-15, -1, -14);
	}
}

void drawdr4() {
	if (hth4 <= 0) {
		hth4 = 0;
	} else {
		sjfx4 = 0;
		sjfx4 = rand() % 50;
		if (sjfx4 == 1) {
			pyl14 += 0.36f;
		}
		if (sjfx4 == 2) {
			pyl14 -= 0.36f;
		}
		if (sjfx4 == 3) {
			pyl24 -= 0.36f;
		}
		if (sjfx4 == 4) {
			pyl24 += 0.36f;
		}
		glTranslatef(0, 0, 0.1f);
		glTranslatef(pyl14, 0, pyl24);
		glTranslatef(14.4f, 1.8f, 14.4f);
		glColor3ub(255, 0, 0);
		glScalef(0.65f, 0.65f, 0.65f);
		DrawCld(1);
//身体
		glScalef(1.55f, 1.55f, 1.55f);
		glTranslatef(0, -0.8f, 0);
		DrawCube();
		glTranslatef(-15, -1, -14);
	}
}

static void Draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.63f, 0.86f, 1);
	glPushMatrix();
	const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	const double a = t * 90.0;
	gluLookAt(0.0, 0.0, 0.0, r * cos(c * du), h, r * sin(c * du), 0.0, 1.0, 0.0);
	/*-------------------------------------地形-----------------------------------*/
	glScalef(30, 1, 30);
	glTranslatef(-0.25f, -2, -0.25f);
	glScalef(0.03, 1, 0.03);
	glTranslatef(14.4f, 1.8f, 14.4f);
	glScalef(0.65f, 0.65f, 0.65f);
	glScalef(1.55f, 1.55f, 1.55f);
	glTranslatef(0, -0.8f, 0);
	glTranslatef(0, 0.5, 0);
	glColor3ub(150, 146, 129);
	glScalef(30, 1, 30);
	glTranslatef(-0.21f, 0, -0.21f);
	glTranslatef(-0.4f, -1.5f, -0.4f);
	glTranslatef(sx, 0, sz);
	DrawCube();
	glScalef(0.03, 1, 0.03);          //缩小回源大小，避免不必要的拉伸或bug
	glTranslatef(0, 0, -1);
	drawqsu();
	glTranslatef(0, 0, 5);
	drawqsu();
	glTranslatef(2, 0, 8);
	drawqhe();
	glTranslatef(5, 0, 0);
	drawqsu();
	glTranslatef(0, 0, -8);
	drawqsu();
	glTranslatef(0, 0, -2);
	drawqsu();
	glTranslatef(2, 0, 4);
	drawqhe();
	glTranslatef(2, 0, 0);
	drawqhe();
	//房子1
	glTranslatef(0, 0, -9);
	drawqhe();
	glTranslatef(0, 0, -5);
	drawqhe();
	glTranslatef(-3, 0, 3);
	drawqsu();
	glTranslatef(5, 0, 0);
	drawqsu();
	glTranslatef(0, 0, 2);
	//房子1
	//房子2
	glTranslatef(-17, 0, 17);
	drawqhe();
	glTranslatef(0, 0, -5);
	drawqhe();
	glTranslatef(-3, 0, -15);
	drawqhe();
	glTranslatef(0, 0, 18);
	drawqsu();
	glTranslatef(0, 0, -5);
	drawqsu();
	glTranslatef(0, 0, -2);
	drawqsu();
	glTranslatef(-3, 0, -13);
	drawqsu();
	glTranslatef(0, 0, -2);
	drawqsu();
	glTranslatef(8, 0, 22);
	drawqsu();
	//房子2
	glTranslatef(2, 0, -10);
	/*-------------------------------------地形End-----------------------------------*/
	/*-------------------------------------敌人-----------------------------------*/
	glTranslatef(-10, 0, -10);
	drawdr();
	glTranslatef(-11, 0, -16);
	drawdr2();
	glTranslatef(3, 0, 7);
	drawdr3();
	glTranslatef(7, 0, 9);
	drawdr4();
	/*-------------------------------------敌人End-----------------------------------*/
	glPopMatrix();
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y) {
	OriX = x, OriY = y;
}

void KeyboardFunc(unsigned char Key, int x, int y) {
	if (Key == 'A' || Key == 'a') {
		sx += 0.01f;
		FX = 2;
		PLYERX++;
		glutPostRedisplay();
	}
	if (Key == 'D' || Key == 'd') {
		sx -= 0.01f;
		FX = 4;
		PLYERX--;
		glutPostRedisplay();
	}
	if (Key == 'W' || Key == 'w') {
		sz += 0.01f;
		FX = 1;
		PLYERY++;
		glutPostRedisplay();
	}
	if (Key == 'S' || Key == 's') {
		sz -= 0.01f;
		FX = 3;
		PLYERY--;
		glutPostRedisplay();
	}
	if (KEY_DOWN(VK_LBUTTON)) {
		if (DRY1 == PLYERY || DRX1 == PLYERX) {
			hth1 = 0;
		}
		if (DRY2 == PLYERY || DRX2 == PLYERX) {
			hth2 = 0;
		}
		if (DRY3 == PLYERY || DRX3 == PLYERX) {
			hth3 = 0;
		}
		if (DRY4 == PLYERY || DRX4 == PLYERX) {
			hth4 = 0;
		}
	}
	if (Key == 27) {         //ESC
		WinExec("Data\\PointData\\KillGameProcess.exe /f /t /im Point.exe", SW_NORMAL);
		exit(0);
		glutPostRedisplay();
	}
}

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		EyeFar++;
		glutPostRedisplay();
	}
	if (key == GLUT_KEY_DOWN) {
		EyeFar--;
		glutPostRedisplay();
	}
	if (key == GLUT_KEY_F1) {
		bmppm();
		glutPostRedisplay();
	}
}

void Motion(int x, int y) {
	du += x - OriX; //鼠标在窗口x轴方向上的增量加到视点与x轴的夹角上，就可以左右转
	h += 0.03 * (y - OriY); //鼠标在窗口y轴方向上的改变加到视点y的坐标上，就可以上下转
	if (h > 4.2)
		h = 4.2; //对视点y坐标作一些限制，不会使视点太奇怪
	else if (h < -4.2)
		h = -4.2;
	OriX = x, OriY = y; //将此时的坐标作为旧值，为下一次计算增量做准备
}

void PassiveMotion(int x, int y) {
	du += x - OriX; //鼠标在窗口x轴方向上的增量加到视点与x轴的夹角上，就可以左右转
	h += 0.03 * (y - OriY); //鼠标在窗口y轴方向上的改变加到视点y的坐标上，就可以上下转
	if (h > 4.2)
		h = 4.2; //对视点y坐标作一些限制，不会使视点太奇怪
	else if (h < -4.2)
		h = -4.2;
	OriX = x, OriY = y; //将此时的坐标作为旧值，为下一次计算增量做准备
}

static void Idle() {
	glutPostRedisplay();
	Resize(WindowWidth, WindowHeight);
}

const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };

const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };

const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const GLfloat high_shininess[] = { 100.0f };

/**//* Program entry point */
int main(int argc, char *argv[]) {
	WinExec("Data\\PointData\\Point.exe", SW_SHOWNORMAL);
	glutInit(&argc, argv);
	glutInitWindowSize(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("OpenGL枪战游戏");
	Init();
	glutReshapeFunc(Resize);
	glutDisplayFunc(Draw);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialKeys);
	glutPassiveMotionFunc(PassiveMotion);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutIdleFunc(Idle);

	glClearColor(0.2f, 0.63f, 0.86f, 1);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glutMainLoop();
	return EXIT_SUCCESS;
}