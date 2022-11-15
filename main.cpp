/** 
*	MPC-MPG 2022 - Zaverecny projekt
*
*	Autor:		xkured00
*	Student ID:	203736
*	Email:		xkured00@vut.cz
*	Nazev:		zima 
* 
*	Seznam vypracovanych ukolu:
*	letani (2b)
*	stoupani (1b)
*	animace (1b) - slunce
*	chuze (2b)
*	hod predmetu (2b) - pri aktivaci (INSERT) je predmet hozen, pri opetovnem zmacknuti klavesy zmizi
*	bezierovy platy (2b) - je pomoci nich vytvoren mrak na obloze
*	modelovani objektu (3b) - plot, dum, strom, studna, zidle
*	nepruchozi objekt (2b) - plot je nepruchozi (ze vsech stran), je mozne jej preletet
*	pruhlednost (1b) - nachazi se na podlaze (ovlivnuje odstin snehu) - lze skrz podlahu hodit predmet pro overeni
*	menu (2b) - lze vypnout i simulaci chuze a zapnout debug mod (informace o souradnicich), nelze ovladat osvetleni
*	vypis textu (2b)
*	tlacitka (2b) - lze vypnout/zapnout zobrazeni domecku a zmenit barvu listi stromu
*	
*	celkem - 22b
*	
*	neuplne ukoly:
*	textura (2b) - neni vytvorena vlastni (-1b)
*	blender model (2b) - neni aplikovana textura (-1b)
* 
*	celkem - 2b
*	
*	Ocekavany pocet bodu: 24b
*
*	Ovladani: sipky, page_down/up, 
*	insert pro hod predmetu,
*	prave tlacitko mysi pro menu
*	kliknutim na tlacitka v horni casti obrazovky lze zmenit barvu stromu a vypnout/zapnout domecky
* 
*	JE POTREBA ZMENIT CESTU K HEADER SOUBORU drawObjects.h
* 
*	Vyvinuto na Windows 10, debugger x64
* 
**/

#include "imageLoad.h"
#include "OBJ_Loader.h"

// bohuzel mi jednoduchy include header filu nefungoval, potreba zmenit
#include "../source/repos/Projekt_MPG/Projekt_MPG/drawObjects.h"

#include <cmath>
#include <string>
#include <GL/glut.h>

#include <iostream>
#include <random>

// konstanty a makra pridejte zde
// PI/180

#define PIover180 0.017453292519943f;
float PI = 3.14159265;

//menu
#define MENU_RESET  1001
#define MENU_EXITOK 1002
#define MENU_EXITNO 1003
#define MENU_TEXTUREON 1004
#define MENU_TEXTUREOFF 1005
#define MENU_STEPSON 1006
#define MENU_STEPSOFF 1007
#define MENU_TIMEON 1008
#define MENU_TIMEOFF 1009
#define MENU_DEBUGON 1012
#define MENU_DEBUGOFF 1013

objl::Loader Objekt;
bool loaded;

// globalni promenne pro transformace
int xnew = 0, ynew = 0;
int xold = 0, yold = 0;
int xx, yy, zz;
int stav;
float angleX = 0;
float angleY = 0;
float tranz = -48.0f, tranx = 0.0f, trany = 0.0f;

int windowHeight, windowWidth;

float walkingSpeed = 0.7;
int window;

bool timerOn = true;
bool increasing = true;

int ymouse = 0, xmouse = 0;

float xOffset = 0;
float yOffset = 0;

int window1, window2, winWidth;

// pole pro nazvy textur
GLuint textury[1];

bool mipmap = true;

// nastaveni projekce
float fov = 60.0;
float nearPlane = 0.1;
float farPlane = 300.0;

//kolize (souradnice shodne s plotem)
float collisionStart = -22, collisionEnd = 22, collisionZ = -20;

// promenne pro hod predmetu
bool throwableExist = false;
float throwableX;
float throwableY;
float throwableZ;
float throwableAngleHor;
float throwableAngleVer;
float throwableSpeed = 0.1; // konstanta rychlosti hodu

// promene pro simulaci kroku
bool isWalking = false; //promenna rikajici, jestli se uzivatel aktualne ve scene pohybuje
bool stepsOn = true; //zapnuti/vypnuti chuze
float stepsHeight = 0.8f; // max. vyska chuze
float stepsCurrentHeight = 0.0f; //aktualni vyska
bool stepsAscending = true; //aktualni smer chuze (stoupani/klesani)

// promenne pro udalosti
bool animOn = true; //animace
bool textureOn = true; //textury
bool debugOn = false; //debug text
bool greenTree = false; //tlacitko meni barvu stromu
bool housesOn = true;
bool housesButtonCLicked = false;
bool greenTreeButtonCLicked = false;

//pocatecni hodnoty pro slunce
float sunX;
float sunY = 70;
float sunRadius = 250;
float sunAngle = 0;

//vychozi logger pro vypsany text
string logger = "zadny";

void isFenceCollision(void) {
	float zmax = 21;
	float zmin = 19;
	float xmax = 22;
	float xmin = -22;

	if (-trany < 4) {
		if (tranx > xmin && tranx < xmax) {
			if (-tranz > zmin && -tranz - 1 < zmin) {
				tranz = -zmin + 1;
			}
			if (-tranz < zmax && -tranz + 1 > zmax) {
				tranz = -zmax - 1;
			}
		}
	}
}

//nastaveni kolizi
bool isCollisionFront(void) {
	if (tranx > collisionStart && tranx < collisionEnd - 1 && tranz >= -22 && tranz < -20 && -trany < 4 ) {
		return true;
	}
	else {
		return false;
	}
}

bool isCollisionBack(void) {
	if (tranx  > collisionStart && tranx < collisionEnd && tranz <= -18 && tranz > -20 && -trany < 4) {
		return true;
	}
	else {
		return false;
	}
}

bool isCollisionLeft(void) {
	if (tranx > -collisionEnd && tranx < -20 && -22 < tranz && tranz < -18 && -trany < 4) {
		return true;
	}
	else {
		return false;
	}
}

bool isCollisionRight(void) {
	if (-tranx > collisionStart && tranx > 20 && -22 < tranz &&  tranz < -18 && -trany < 4) {
		return true;
	}
	else {
		return false;
	}
}

//timer pro hod predmetu
void onTimerthrowable(int value)
{
	if (throwableExist == true)
	{
		throwableX = throwableX + sin(throwableAngleHor) * throwableSpeed;
		throwableY = throwableY - sin(throwableAngleVer) * throwableSpeed;
		throwableZ = throwableZ - cos(throwableAngleHor) * throwableSpeed;


		glutPostRedisplay();
		glutTimerFunc(5, onTimerthrowable, value);
	}

}

//timer pro simulaci kroku
void onTimerSteps(int value)
{
	if (stepsOn == true && isWalking == true)
	{
		if (stepsAscending == true)
		{
			stepsCurrentHeight = stepsCurrentHeight + 0.01;
		}
		else
		{
			stepsCurrentHeight = stepsCurrentHeight - 0.01;
		}

		if (stepsCurrentHeight > (stepsHeight) / 2)
		{
			stepsAscending = false;
		}
		else if (stepsCurrentHeight < -(stepsHeight / 2))
		{
			stepsAscending = true;
		}

		glutPostRedisplay();

	}

	glutTimerFunc(5, onTimerSteps, value);
}

// timer pro slunce
void onTimerSun(int value)
{
	if (animOn) {
		sunX += 0.0001 * (cos(sunAngle) * sunRadius);
		sunY += 0.0001 * (sin(sunAngle) * sunRadius);
		sunAngle = sunAngle + 0.001;

		glutPostRedisplay();
		glutTimerFunc(5, onTimerSun, value);
	}
}

// obsluha menu
void onMenu(int value)
{
	logger = "menu";
	switch (value)
	{
	case MENU_RESET:  tranx = 0; trany = 0; tranz = -45; xOffset = 0; yOffset = 0; xmouse = 0; ymouse = 0; xnew = 0; ynew = 0; xold = 0; yold = 0; angleX = 0; angleY = 0; break;
	case MENU_EXITOK: exit(1); break;
	case MENU_EXITNO: break;
	case MENU_TIMEON: animOn = true; glutTimerFunc(5, onTimerSun, 1); break;
	case MENU_TIMEOFF: animOn = false; break;
	case MENU_STEPSON: stepsOn = true;  break;
	case MENU_STEPSOFF: stepsOn = false; break;
	case MENU_TEXTUREON: textureOn = true;  break;
	case MENU_TEXTUREOFF: textureOn = false; break;
	case MENU_DEBUGON: debugOn = true;  break;
	case MENU_DEBUGOFF: debugOn = false; break;
	}
	glutPostRedisplay();
}

void createMenu()
{
	// vytvoreni podmenu
	int idSub = glutCreateMenu(onMenu);
	glutAddMenuEntry("Ano", MENU_EXITOK);
	glutAddMenuEntry("Ne", MENU_EXITNO);

	int idSteps = glutCreateMenu(onMenu);
	glutAddMenuEntry("Zapnout", MENU_STEPSON);
	glutAddMenuEntry("Vypnout", MENU_STEPSOFF);

	int idTime = glutCreateMenu(onMenu);
	glutAddMenuEntry("Zapnout", MENU_TIMEON);
	glutAddMenuEntry("Vypnout", MENU_TIMEOFF);

	int idTexture = glutCreateMenu(onMenu);
	glutAddMenuEntry("Zapnout", MENU_TEXTUREON);
	glutAddMenuEntry("Vypnout", MENU_TEXTUREOFF);

	int idDebug = glutCreateMenu(onMenu);
	glutAddMenuEntry("Zapnout", MENU_DEBUGON);
	glutAddMenuEntry("Vypnout", MENU_DEBUGOFF);

	// vytvoreni hlavniho menu
	glutCreateMenu(onMenu);
	glutAddMenuEntry("Reset pozice ", MENU_RESET);
	glutAddSubMenu("Konec", idSub);
	glutAddSubMenu("Kroky", idSteps);
	glutAddSubMenu("Animace", idTime);
	glutAddSubMenu("Textury", idTexture);
	glutAddSubMenu("Debug", idDebug);

	// prirazeni hlavniho menu na tlacitko mysi
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//funkcionalita textu a tlacitek
void showText()
{
	glPushMatrix();
	char text[512];
	glColor3f(1.0, 1.0, 1.0);

	glDisable(GL_TEXTURE_2D);

	sprintf_s(text, "Zapnout/vypnout domecky: ");
	glRasterPos3f(-0.04, 0.052, -0.10);
	for (int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);

	float buttonHousesSizeX = 0.003;
	float buttonHousesSizeY = 0.003;

	float buttonHousesX = -0.0025;
	float buttonHousesY = 0.052;
	float buttonHousesZ = -0.1;

	glBegin(GL_QUADS);
	housesButtonCLicked ? glColor3f(0.5, 0.5, 0.5) : glColor3f(1, 1, 1);
	glVertex3f(buttonHousesX, buttonHousesY, buttonHousesZ);
	glVertex3f(buttonHousesX + buttonHousesSizeX, buttonHousesY, buttonHousesZ);
	glVertex3f(buttonHousesX + buttonHousesSizeX, buttonHousesY + buttonHousesSizeY, buttonHousesZ);
	glVertex3f(buttonHousesX, buttonHousesY + buttonHousesSizeY, buttonHousesZ);
	glEnd();

	glColor3f(1, 1, 1);
	sprintf_s(text, "Zmenit barvu stromu: ");
	glRasterPos3f(0.004, 0.052, -0.10);
	for (int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);

	float buttonTreeSizeX = 0.003;
	float buttonTreeSizeY = 0.003;

	float buttonTreeX = 0.033;
	float buttonTreeY = 0.052;
	float buttonTreeZ = -0.1;

	glBegin(GL_QUADS);
	greenTreeButtonCLicked ? glColor3f(0.5, 0.5, 0.5) : glColor3f(1, 1, 1);
	glVertex3f(buttonTreeX, buttonTreeY, buttonTreeZ);
	glVertex3f(buttonTreeX + buttonTreeSizeX, buttonTreeY, buttonTreeZ);
	glVertex3f(buttonTreeX + buttonTreeSizeX, buttonTreeY + buttonTreeSizeY, buttonTreeZ);
	glVertex3f(buttonTreeX, buttonTreeY + buttonTreeSizeY, buttonTreeZ);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);

	sprintf_s(text, "Posledni vykonany prikaz: %s", logger.c_str());
	glRasterPos3f(0.004, 0.049, -0.10);
	for (int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);

	if (debugOn) {

		sprintf_s(text, "x: %.1f, y: %.1f, z: %.1f", tranx, trany, tranz);
		glRasterPos3f(0.004, 0.046, -0.10);
		for (int i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);

		sprintf_s(text, "Rotace x: %d, Rotace y: %d", xnew, ynew);
		glRasterPos3f(0.004, 0.043, -0.10);
		for (int i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);

		sprintf_s(text, "xx: %d, yy: %d, zz: %d, xnew: %d, ynew: %d, xold: %d, yold: %d", xx, yy, zz, xnew, ynew, xold, yold);
		glRasterPos3f(-0.06, 0.040, -0.10);
		for (int i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);

		sprintf_s(text, "xmouse: %d, ymouse: %d, xOffset: %.1f, yOffset: %.1f, angleX: %.1f, angleY: %.1f", xmouse, ymouse, xOffset, yOffset, angleX, angleY);
		glRasterPos3f(-0.07, 0.037, -0.10);
		for (int i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);

		sprintf_s(text, "collisionStart: %.1f, collisionEnd: %.1f, collisionZ: %.1f", collisionStart, collisionEnd, collisionZ);
		glRasterPos3f(-0.06, 0.034, -0.10);
		for (int i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);

	}

	glPopMatrix();

}

// funkce volana pri zmene velikosti sceny
void OnReshape(int w, int h)            // event handler pro zmenu velikosti okna
{	
	glViewport(0, 0, w, h);             // OpenGL: nastaveni rozmenu viewportu
	glMatrixMode(GL_PROJECTION);        // OpenGL: matice bude typu projekce
	glLoadIdentity();                   // OpenGL: matice bude identicka (jen jednicky v hlavni diagonale)
	gluPerspective(fov, (double)w/h, nearPlane, farPlane); // perspektivni projekce
	showText();
}

// funkce pro inicializaci
// do teto funkce vlozit vsechny vlastnosti a inicializace, ktere se nebudou menit pri behu aplikace
void OnInit(void)
{	// dalsi stavy a vlastnosti (napr. nastaveni svetel, materialu, ...)

	loaded = Objekt.LoadFile("stul3.obj");

	if (!setTexture("snow.bmp", &textury[0], mipmap)) {
		std::cout << "Chyba pri nacitani souboru" << std::endl;
		exit((int)0);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (mipmap) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (mipmap) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPointSize(5);

}

// doporucene vykresleni objektu 
// (1) pouzijte PUSH/POP pro GL_MODELVIEW matice, abyste si nerozhodili transformace cele sceny
// (2) vykreslujte vlastni objekty okolo pocatku (0,0,0) pro snazsi provadeni transformaci a umisteni ve scene
// (3) pro ziskaji aktulanich stavovych promennych v OpenGL muzete pouzit funkce glGetBooleanv(...)

void checkBounds(void)
{
	if (tranx < -50)
	{
		tranx = -50;
	}

	if (tranx > 50)
	{
		tranx = 50;
	}

	if (tranz < -50)
	{
		tranz = -50;
	}

	if (tranz > 50)
	{
		tranz = 50;
	}

	if (trany > 0) {
		trany = 0;
	}
}

void DrawPlane(int size)
{
	// (1)
	glPushMatrix();
	
	// nastaveni transformace (pokud je treba)
	glTranslatef(0, -1, 0);
	
	// (3) nastaveni vlastnosti OpenGL

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// (2) vykresleni objektu
	glBegin(GL_QUADS);
	glColor4f(0.5, 0.5, 0.5, 0.7);
	glTexCoord2f(-size, size); glVertex3i(-size, 0, size);
	glTexCoord2f(-size, -size); glVertex3i(-size, 0, -size);
	glTexCoord2f(size, -size); glVertex3i(size, 0, -size);
	glTexCoord2f(size, size); glVertex3i(size, 0, size);
	glEnd();
	glDisable(GL_BLEND);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// v pripade, ze jste vypnuli/zapnuli nejaky stav, vratte ho do puvodniho

	glPopMatrix();
}

// funkce volana pri prekresleni sceny
void OnDisplay(void)				   
{
    // nastaveni parametru pro defaultni mazani rovin
	glClearDepth(1.0f);
    glClearColor(0.7, 0.7, 0.7, 0.3);
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);      
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	// inicializace matice MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	showText();

	glBindTexture(GL_TEXTURE_2D, textury[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// provedeni transformaci sceny (pro rotaci mysi a pohyb klavesami)
	glRotatef(ynew, 1.0f, 0.0f, 0.0f);		// rotace podle osy x
    glRotatef(xnew, 0.0f, 1.0f, 0.0f);		// rotace podle osy y
	glTranslatef(tranx, trany + stepsCurrentHeight, tranz);		// relativni zmena v z

	if (throwableExist == true)
	{
		glPushMatrix();
		drawCuboid(throwableX, throwableY, throwableZ, 1, 1, 1, 0.5, 0.5, 0.5);
		glTranslatef(-tranx, -trany, -tranz);
		glPopMatrix();
	}

	if (textureOn) {
		glEnable(GL_TEXTURE_2D);
	}
	DrawPlane(50);
	if (textureOn) {
		glDisable(GL_TEXTURE_2D);
	}

	// vykresleni domu
	if (housesOn == true) {
		drawHouse(-15, -1, 0, 1, 0.7, 0.2, 180);
		drawHouse(5, -1, 0, 0, 0.1, 0, 180);
	}

	// vykresleni zidle
	drawTree(-10, -0.5, -5, greenTree);

	// vykresleni zidle
	drawChair(2, -0.5, 5);

	//vykresleni studny
	drawWell(4, -0.5, 7);

	//vykresleni plotu
	drawFence(-10, -0.5, 10, 40, 0.5, 3);

	//vykresleni slunce
	drawSun(sunX, sunY, 0);

	// vykresleni mraku - bezier
	drawCloud(-20, 60, 20);

	//vykresleni stolu
	if (loaded)
	{
		for (int i = 0; i < Objekt.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Objekt.LoadedMeshes[i];

			glBegin(GL_TRIANGLES);
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				glNormal3f(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z);
				glVertex3f(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y - 1, curMesh.Vertices[j].Position.Z);
			}
			glEnd();
		}
	}

    glFlush();		
    glutSwapBuffers(); 
}

// funkce volana pri stisku tlacitka
void OnMouseButton(int button, int state, int x, int y)
{ 
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			stav = 1;
			xx = x;
			yy = y;
			logger = "klik";
			if (xx >= 457 && yy >= 457 && xx <= 470 && yy <= 470) {
				greenTree = !greenTree;
				greenTreeButtonCLicked = true;
				logger = "klik na tlacitko";
			}
			if (xx >= 310 && yy >= 457 && xx <= 322 && yy <= 470) {
				housesOn = !housesOn;
				housesButtonCLicked = true;
				logger = "klik na tlacitko";
			}
		}
		else {
			stav = 0;
			xold = xnew;
			yold = ynew;
			greenTreeButtonCLicked = false;
			housesButtonCLicked = false;
		}
	}
	glutPostRedisplay();
}

// funkce volana pri pohybu mysi a stistenem tlacitku mysi
void OnMouseMotion(int x, int y)
{ 
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;

	if (stav == 1) {
		xnew = xold + x - xx;
		ynew = -(yold + y - yy);
		angleX = xnew * PIover180;
		angleY = ynew * PIover180;
		glutPostRedisplay();
	}
}

// funkce volana pri stisku specialnich klaves
void OnSpecial(int key, int mx, int my)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			isWalking = true;	
			tranz = tranz + cos(angleX) * walkingSpeed;
			tranx = tranx - sin(angleX) * walkingSpeed;
			trany = trany + sin(angleY) * walkingSpeed;
			logger = "dopredu";
			isFenceCollision();
			checkBounds();
			break;
		case GLUT_KEY_DOWN:
			isWalking = true;
			tranz = tranz - cos(angleX) * walkingSpeed;
			tranx = tranx + sin(angleX) * walkingSpeed;
			trany = trany - sin(angleY) * walkingSpeed;
			isFenceCollision();
			checkBounds();
			logger = "dozadu";
			break;
		case GLUT_KEY_RIGHT:
			tranz = tranz - cos(angleX - PI / 2) * walkingSpeed;
			tranx = tranx + sin(angleX - PI / 2) * walkingSpeed;
			isFenceCollision();
			checkBounds();
			logger = "doprava";
		break;
		case GLUT_KEY_LEFT: 
			tranz = tranz - cos(angleX + PI / 2) * walkingSpeed;
			tranx = tranx + sin(angleX + PI / 2) * walkingSpeed;
			isFenceCollision();
			checkBounds();
			logger = "doleva";
		break;
		case GLUT_KEY_PAGE_UP:
			{
				trany -= 2;
			}
			checkBounds();
			logger = "nahoru";
			break;
		case GLUT_KEY_PAGE_DOWN:
			{
				trany += 2;
			}
			checkBounds();
			logger = "dolu";
			break;
		case GLUT_KEY_INSERT:
			logger = "hod";
			if (throwableExist == false)
			{
				throwableX = -tranx;
				throwableY = -trany;
				throwableZ = -tranz;
				throwableAngleHor = angleX;
				throwableAngleVer = angleY;

				throwableExist = true;
				glutTimerFunc(5, onTimerthrowable, 1);
				break;
			}
			else if (throwableExist == true)
			{
				throwableExist = false;
				break;
			}
		default:
			break;
	}

	glutPostRedisplay();
}

// funkce volana pri pusteni klavesy
void onSpecialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: isWalking = false; stepsCurrentHeight = 0; break;
	case GLUT_KEY_DOWN: isWalking = false; stepsCurrentHeight = 0; break;
	case GLUT_KEY_RIGHT: isWalking = false; stepsCurrentHeight = 0; break;
	case GLUT_KEY_LEFT: isWalking = false; stepsCurrentHeight = 0;  break;
	}
}

// funkce main
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);					// inicializace knihovny GLUT
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // init double buffer

	glutInitWindowSize(640, 480);			// nastaveni pocatecni velikosti dale oteviranych oken
    glutInitWindowPosition(200, 200);		// nastaveni pocatecniho umisteni dale oteviranych oken
    
	window = glutCreateWindow("Projekt 2022 – Autor: xkured00");	// vytvoreni okna

	// registracni funkce
	glutSetWindow(window);
	glutDisplayFunc(OnDisplay);				// registrace funkce volane pri prekreslovani aktualniho okna
    glutReshapeFunc(OnReshape);				// registrace funkce volane pri zmene velikosti aktualniho okna

	glutSpecialFunc(OnSpecial);				// registrace funkce pro zachytavani specialnich klaves
	glutSpecialUpFunc(onSpecialUp);			// pusteni klavesy

	glutMouseFunc(OnMouseButton);			// registrace funkce pro stisk tlacitka mysi
	glutMotionFunc(OnMouseMotion);			// registrace funkce pro pohyb mysi pri stisknutem tlacitku

	// pripadne dalsi udalosti...
	glutTimerFunc(5, onTimerSteps, 1);
	glutTimerFunc(5, onTimerSun, 1);

	createMenu();

	// inicializace sceny
	OnInit();
	glutMainLoop();

    return 0;
}
