/*********
  CTIS164 - Template Source Program
----------
STUDENT : Talal Salman Zafeer
SECTION : 02
HOMEWORK: 02
----------
PROBLEMS: 1) Active Timer doesn't work causing the issue in pausing of the game
		  2) Outbounds for the rocket are not working properly which doesn't allow rocket to stay in a specific place.
		  3) Minor issues in blasting with when bomb hits the van towards the end.
----------
ADDITIONAL FEATURES: 1) The loading Screen shows the screen loading of the game.
					 2) The left click changes the color of the figther plane.
					 3) Each car is assigned random points, so upon hitting player gets addition points.
					 4) Upon hitting the screen displays a blast like animation.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 700
#define TIMER_PERIOD 30// Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define LOADING_COUNTER 20
#define D2R 0.0174532
#define RUNNING_COUNTER 20

#define LOAD_SCREEN 1 //defines the state of the screens incoming.
#define RUN_SCREEN 2
#define OVER_SCREEN 3

#define MAXV 5 //Defines the maximum number of van being sent on the screen


/* Global Variables for Template File */

int loadTimer = LOADING_COUNTER, loadIncrement = 0.3;

bool up = false, down = false, right = false, left = false, activeTimer = false;

int  winWidth, winHeight; // current Window width and height

int stat = LOAD_SCREEN, speed = TIMER_PERIOD; // The loading screen animation timer along and it's speed

int t_score = 0, count = 20, increment = 0;  // score of the user at the end of the game, timer counter, timer influencer

bool bomb_status = false; // bomb movement

bool van_visible = false; // van visibility status in case of hit/not hit

int planeX = 0, planeY = 223;

int blastTimer = 0;

int blastX, blastY;

//Structures 

typedef struct {
	float xCor, yCor;
}coordinate_struct;  //general blueprint for coordinates

typedef struct {
	coordinate_struct bombPos; //Defines the bomb position
}bomb_struct;

typedef struct { //van's infomartion structure
	bool van_visible;
	coordinate_struct van_position; // van's coordinates
	int indiv_points; // each van's potential points reward
	double red, green, blue; // color variables for random allocation of colors

}van_info_struct;

van_info_struct* van;
bomb_struct bomb = { planeX + 33,planeY - 20 }; // bomb initial position allocation

//
// to draw circle, center at (x,y)
// radius r
//

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void displayBomb() {
	glColor3ub(17, 34, 34); // star 1
	glBegin(GL_TRIANGLES);
	glVertex2f(bomb.bombPos.xCor, bomb.bombPos.yCor + 20);
	glVertex2f(bomb.bombPos.xCor + 20, bomb.bombPos.yCor + 20);
	glVertex2f(bomb.bombPos.xCor + 10, bomb.bombPos.yCor);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(bomb.bombPos.xCor, bomb.bombPos.yCor + 5);
	glVertex2f(bomb.bombPos.xCor + 20, bomb.bombPos.yCor + 5);
	glVertex2f(bomb.bombPos.xCor + 10, bomb.bombPos.yCor + 20);
	glEnd();
}


//Initializes the structure of the van loading on the screen
void initializeStructure(van_info_struct* Truck) {
	int i;
	int x = -1000;
	for (i = 0; i < MAXV; i++) {
		(Truck + i)->van_position.xCor = x;
		(Truck + i)->van_visible = true;
		x += 200;
	}

}

void displayover() { //displays the game over screen
	glClearColor(0.3, 0.12, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.5f, 0.8f, 0);
	if (t_score > 30) { vprint2(-100, 100, 0.3, "Amazinggg !! :) "); }
	else
	{
		vprint2(-100, 100, 0.3, "Game Over :)");
	}
	vprint2(-80, 0, 0.2, "Your Score: %d", t_score);
	vprint(-60, -250, GLUT_BITMAP_8_BY_13, "F1 to Play Again");
}

void displayloading() { // Dispays the loading screen
	glClearColor(0.3, 0.12, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	vprint2(-100, 100, 0.3, "LOADING");

	int width = (LOADING_COUNTER - loadTimer) * 30;
	glColor3ub(170, 100, 140);
	glRectf(-300, -70, -300 + width, -20);
	vprint2(-250, 290, 0.2, "Van Blasting BY Talal Salman Zafeer");
}

void displayClouds(int x, int y) {

	glColor3ub(255, 250, 250); 
	circle(0 + x, 0 + y, 30);
	circle(-25 + x, y, 20);
	circle(25 + x, -2 + y, 20);
	circle(21 + x, -19 + y, 10);

	circle(0 - 350 + x, 0 + 50 + y, 30);
	circle(-25 - 350 + x, +50 + y, 20);
	circle(25 - 350 + x, -2 + 50 + y, 20);
	circle(21 - 350 + x, -19 + 50 + y, 10);

	circle(0 + 150 + x, 0 + y, 30); 
	circle(-25 + 150 + x, y, 20);
	circle(25 + 150 + x, -2 + y, 20);
	circle(21 + 150 + x, -19 + y, 10);

	circle(0 + 300 + x, 0 - 80 + y, 30);
	circle(-25 + 300 + x, -80 + y, 20);
	circle(25 + 300 + x, -80 - 2 + y, 20);
	circle(21 + 300 + x, -80 - 19 + y, 10);

	circle(0 + 480 + x, 0 + y, 30);
	circle(-25 + 480 + x, y, 20);
	circle(25 + 480 + x, -2 + y, 20);
	circle(21 + 480 + x, -19 + y, 10);

	circle(0 - 480 + x, 0 + 20 + y, 30); 
	circle(-25 - 480 + x, 20 + y, 20);
	circle(25 - 480 + x, 20 - 2 + y, 20);
	circle(21 - 480 + x, 20 + -19 + y, 10);

	circle(0 - 250 + x, 0 + y, 30); 
	circle(-25 - 250 + x, y, 20);
	circle(25 - 250 + x, -2 + y, 20);
	circle(21 - 250 + x, -19 + y, 10);
}

void displayrunning() {
	glColor3ub(255, 215, 0); // sun
	circle(190, 140, 70);

	// Sunspots
	glColor3ub(255, 255, 0); 
	circle(190, 120, 20);
	glColor3ub(255, 240, 0); 
	circle(220, 160, 15);
	glColor3ub(255, 240, 0); 
	circle(170, 170, 15);
	glColor3ub(255, 240, 0); 
	circle(155, 150, 10);
	glColor3ub(255, 240, 0); 
	circle(190, 170, 5);
}

void displayGradient(int x1, int y1, int w, int h, float r, float g, float b) {
	//Used for the gradient of the sky
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glColor3f(r + 0.2, g + 0.2, b + 0.2);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
}

void displayRoad(int x1, int y1, int w, int h, float r, float g, float b) {

	// Road base showing as gradient
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glColor3f(r + 0.2, g + 0.2, b + 0.2);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();

	//Yellow Stripes
	glColor3ub(255, 255, 0);
	glRectf(-700, -175, -500, -150);
	glRectf(-350, -175, -150, -150);
	glRectf(0, -175, 200, -150);
	glRectf(350, -175, 550, -150);
	glRectf(700, -175, 900, -150);

}

int r = 128, g = 128, b = 128; //random color assignment 
int r1 = 64, g1 = 64, b1 = 64; //random color assignment 
void display_fighter_plane(int planeX, int planeY) {
	// Body
	glColor3ub(r, g, b);
	glBegin(GL_POLYGON);
	glVertex2f(planeX, planeY);
	glVertex2f(planeX + 60, planeY - 10);
	glVertex2f(planeX + 60, planeY + 10);
	glVertex2f(planeX, planeY + 20);
	glEnd();

	// Wings
	glColor3ub(r1, g1, b1);
	glBegin(GL_QUADS);
	glVertex2f(planeX + 10, planeY);
	glVertex2f(planeX + 30, planeY - 20);
	glVertex2f(planeX + 35, planeY - 15);
	glVertex2f(planeX + 15, planeY + 5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(planeX + 10, planeY + 20);
	glVertex2f(planeX + 30, planeY + 40);
	glVertex2f(planeX + 35, planeY + 35);
	glVertex2f(planeX + 15, planeY + 15);
	glEnd();

	// Tail
	glColor3ub(r1, g1, b1);
	glBegin(GL_TRIANGLES);
	glVertex2f(planeX + 40, planeY + 10);
	glVertex2f(planeX + 50, planeY + 30);
	glVertex2f(planeX + 60, planeY + 10);
	glEnd();

}

//displays the vans being displayed
void display_van(int xB, int yB, int red, int green, int blue) {
	glColor3ub(red, green, blue); // back part
	glRectf(xB + 214, yB + 1, xB + 357, yB + 50);

	glColor3ub(255, 255, 255); // window part
	glRectf(xB + 224, yB + 35, xB + 347, yB + 75);

	glColor3ub(0, 0, 0); // left outer tire
	circle(xB + 243, yB + 2, 22);

	glColor3ub(105, 105, 105); // left inner tire
	circle(xB + 243, yB + 2, 12);

	glColor3ub(0, 0, 0); // right outer tire
	circle(xB + 329, yB + 5, 26);

	glColor3ub(105, 105, 105); // right inner tire
	circle(xB + 329, yB + 5, 12);

	glColor3ub(105, 105, 105); // front bumper
	glRectf(xB + 357, yB + 15, xB + 367, yB + 35);

	glColor3ub(240, 196, 32); // headlights
	circle(xB + 365, yB + 20, 3);
	circle(xB + 365, yB + 30, 3);
}


// Displays the blast when the bomb hits the car
void displayBlast(int x, int y, int radius) {
	glColor3ub(255, 153, 51);
	circle(x, y, radius);
	glColor3ub(255, 255, 0); 
	circle(x, y, radius / 2);
}


// Displays The house and the trees
void housesAndCity()
{
	glColor3ub(139, 115, 85);
	glRectf(-250, -80, -230, 30);

	glColor3ub(85, 107, 47);
	circle(-250, 20, 30);
	circle(-240, 40, 30);
	circle(-230, 20, 30);

	glColor3ub(255, 218, 185);
	glRectf(-100, -80, 50, 70);

	glColor3ub(139, 71, 38);
	glRectf(-20, -80, 20, -10);

	glColor3ub(205, 200, 177);
	glBegin(GL_TRIANGLES);
	glVertex2f(-100, 70);
	glVertex2f(50, 70);
	glVertex2f(-25, 150);
	glEnd();

	glColor3ub(139, 69, 19);
	circle(-25, 100, 15);

	glColor3ub(255, 215, 0);
	circle(-25, 100, 10);

	glColor3ub(139, 69, 19);
	glRectf(-93, -43, -27, 3);

	glColor3ub(255, 215, 0);
	glRectf(-90, -40, -30, 0);

	glColor3ub(0, 0, 0);
	circle(10, -45, 5);

}


//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0.17, 0.17, 0.17, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (stat)
	{
	case LOAD_SCREEN: displayloading(); break;
	case RUN_SCREEN: {
		glClearColor(0, 0.52, 0.52, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		displayGradient(-900, 450, 2000, 510, 0.5, 0.5, 0.9);
		displayRoad(-900, -50, 2000, 350, 0.24, 0.19, 0.15);
		housesAndCity();
		displayClouds(0, 220);
		displayrunning();
		displayBomb();
		if (blastTimer > 0)
		{
			displayBlast(blastX, blastY, 80);
		}
		display_fighter_plane(planeX, planeY);
		for (int i = 0; i < MAXV; i++) {
			if ((van + i)->van_visible == true) {
				display_van((van + i)->van_position.xCor, (van + i)->van_position.yCor, (van + i)->red, (van + i)->green, (van + i)->blue);
			}
		}
		glColor3ub(245,245,245);
		glRectf(-650,350,650, 300);
		glColor3f(0, 0, 0);
		vprint(-600, 330, GLUT_BITMAP_8_BY_13, "Total Score: %d", t_score);
		vprint(-600, 310, GLUT_BITMAP_8_BY_13, "Timer: %d", count);
		vprint(-400, 330, GLUT_BITMAP_8_BY_13, "<SpaceBar>: Drops The Bomb.");
		vprint(-400, 310, GLUT_BITMAP_8_BY_13, "<F1>: Play/Pause the Game.");
		vprint(-150, 330, GLUT_BITMAP_8_BY_13, "<Left-Click>: The fighter plane changes it's color.");

	}break;

	case OVER_SCREEN: displayover();
	}

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ' && stat == RUN_SCREEN)
		bomb_status = true;

	
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {

	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}
	if (key == GLUT_KEY_F1 && stat == RUN_SCREEN)
	{
		activeTimer == true;
	}
	if (activeTimer == false)
	{
		if (key == GLUT_KEY_LEFT && stat == RUN_SCREEN) {
			planeX -= 15;
			if (bomb_status == false && activeTimer == false)
				bomb.bombPos.xCor = planeX + 33;
		}

		if (key == GLUT_KEY_RIGHT && stat == RUN_SCREEN) {
			planeX += 15;
			if (bomb_status == false && activeTimer == false)
				bomb.bombPos.xCor = planeX + 33;
		}

	}

	if (key == GLUT_KEY_F1 && stat == OVER_SCREEN) //to restart game when it is over
	{
		bomb.bombPos.xCor = planeX + 33;
		bomb.bombPos.yCor = planeY - 20;
		stat = RUN_SCREEN;
		t_score = 0;
		increment = 0;
		bomb_status = false;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.

	srand(time(NULL));


	if (button == GLUT_RIGHT_BUTTON && stat == GLUT_DOWN && activeTimer == false) {
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;
		r1 = rand() % 255;
		g1 = rand() % 255;
		b1 = rand() % 255;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) {

	// to refresh the window it calls display() function
	glutPostRedisplay();
}


#if TIMER_ON == 1
void onTimer(int v) {
	// Write your codes here.
	glutTimerFunc(speed, onTimer, 0);
	if (stat == LOAD_SCREEN) {
		loadTimer -= 0.01;
		if (loadTimer > 0) {
			speed = 100;
		}
		else {
			// time expires

			stat = RUN_SCREEN;
			speed = TIMER_PERIOD;
		}
	}
	increment += 10;
	count = increment / 350;

	if (activeTimer == false)
	{
		if (stat == RUN_SCREEN && count == RUNNING_COUNTER) // to end at 20 seconds
			stat = OVER_SCREEN;
		if (bomb.bombPos.yCor > -560 && bomb_status == true && stat == RUN_SCREEN)
			bomb.bombPos.yCor -= 10;
		if (bomb.bombPos.yCor <= -560 && bomb_status == true && stat == RUN_SCREEN) {
			bomb.bombPos.yCor = planeY - 20;
			bomb.bombPos.xCor = planeX + 30;
			bomb_status = false;

		}

		for (int count_t = 0; count_t < MAXV; count_t++) //movement of each Truck
		{
			if ((van + count_t)->van_position.xCor <= 500)
				(((van + count_t)->van_position).xCor) += 5;
			if ((van + count_t)->van_position.xCor > 500)
			{
				(van + count_t)->van_position.xCor = -1100;
				(van + count_t)->van_position.yCor = -350 + rand() % -300;
				(van + count_t)->indiv_points = rand() % 4 + 1;
				(van + count_t)->red = rand() % 256;
				(van + count_t)->green = rand() % 256;
				(van + count_t)->blue = rand() % 256;
			}
		}
		for (int count_k = 0; count_k < MAXV; count_k++) { //check if Truck gets hit
			if (((van + count_k)->van_position.xCor + 250 <= bomb.bombPos.xCor + 40) && (((van + count_k)->van_position.xCor + 310 >= bomb.bombPos.xCor - 40)) && ((van + count_k)->van_position.yCor + 60 >= (bomb.bombPos.yCor + 15) && (van + count_k)->van_position.yCor - 40 <= (bomb.bombPos.yCor + 15)))
			{
				(van + count_k)->van_visible = false;
				(bomb.bombPos.xCor) = planeX + 34;
				bomb_status = false;
				bomb.bombPos.yCor = planeY - 25;
				t_score += (van + count_k)->indiv_points;
				blastTimer = 10;
				blastX = (van + count_k)->van_position.xCor + 280;
				blastY = (van + count_k)->van_position.yCor + 20;

				if ((van + count_k)->van_visible == false) {
					(van + count_k)->van_visible = true;

					(van + count_k)->van_position.xCor = -1100;
					(van + count_k)->van_position.yCor = -350 + rand() % -300;
					(van + count_k)->indiv_points = rand() % 4 + 1;
					(van + count_k)->red = rand() % 256;
					(van + count_k)->green = rand() % 256;
					(van + count_k)->blue = rand() % 256;
				}
			}
		}

		if (blastTimer > 0) {
			blastTimer--;
		}

	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initializeStructure(van);
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("TALAL SALMAN ZAFEER HW-02");

	int i;
	van = (van_info_struct*)malloc(sizeof(van_info_struct) * MAXV);
	srand(time(0));
	

	for (i = 0; i < MAXV; i++) {
		(van + i)->red = rand() % 256;
		(van + i)->green = rand() % 256;
		(van + i)->blue = rand() % 256;
		(van + i)->indiv_points = rand() % 5 + 1;
		(van + i)->van_position.yCor = -350 + rand() % -300;
	}

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}