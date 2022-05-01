/** 
*	MPC-MPG 2022 - Zaverecny projekt
*
*	Autor:		xkured00
*	Student ID:	203736
*	Email:		xkured00@vut.cz
* 
*	In progress ukoly:
*	menu(2b)
*	letani(2b)
* 
*	Seznam vypracovanych ukolu:
*		
*   Neco vlastniho co by mohlo byt take hodnoceno:
*
*	Ocekavany pocet bodu:	fakt hodne
*
**/
#include "imageLoad.h"
#include <cmath>
#include <GL/glut.h>

// konstanty a makra pridejte zde
// PI/180
#define PIover180 0.017453292519943f

// globalni promenne pro transformace
int xnew = 0, ynew = 0;
int xold = 0, yold = 0;
int xx, yy, zz;
int stav;
float uhel = 0;
float tranz = -100.0f, tranx = 0.0f, trany = 0.0f;

#define MENU_RESET  1001
#define MENU_EXITOK 1002
#define MENU_EXITNO 1003 
#define MENU_TIMERON 1004
#define MENU_TIMEROFF 1005

bool timerOn = true;
bool increasing = true;

int ymouse = 0, xmouse = 0;

float xOffset = 0;
float yOffset = 0;

int window1, window2, winWidth;

// nastaveni projekce
float fov = 60.0;
float nearPlane = 0.1;
float farPlane = 150.0;

// zde vlastnosti svetel materialu, menu, popripade jinych vlastnosti
// ...

void onTimer(int value)
{

	if (increasing)
		xOffset += 1;
	else
		xOffset -= 1;

	if (xOffset > winWidth)
	{
		xOffset = winWidth;
		increasing = false;
	}
	else if (xOffset < 0)
	{
		xOffset = 0;
		increasing = true;
	};

	glutPostRedisplay();

	if (timerOn)
		glutTimerFunc(5, onTimer, value);
}

// obsluha menu
void onMenu(int value)
{
	switch (value)
	{
	case MENU_RESET:  xOffset = 0; yOffset = 0; xmouse = 0; ymouse = 0; break;
	case MENU_EXITOK: exit(1); break;
	case MENU_EXITNO: break;
	case MENU_TIMERON:
		timerOn = true;
		glutTimerFunc(5, onTimer, value);
		break;
		break;
	case MENU_TIMEROFF:
		timerOn = false;
		break;
	}

	glutPostRedisplay();
}

void createMenu()
{
	// vytvoreni podmenu
	int idSub = glutCreateMenu(onMenu);
	glutAddMenuEntry("Ano", MENU_EXITOK);
	glutAddMenuEntry("Ne", MENU_EXITNO);

	// doplnte kod
	int idTimerSub = glutCreateMenu(onMenu);
	glutAddMenuEntry("Ano", MENU_TIMERON);
	glutAddMenuEntry("Ne", MENU_TIMEROFF);

	// vytvoreni hlavniho menu
	glutCreateMenu(onMenu);
	glutAddMenuEntry("Reset pozice ", MENU_RESET);
	glutAddSubMenu("Timer", idTimerSub);
	glutAddSubMenu("Konec", idSub);

	// prirazeni hlavniho menu na tlacitko mysi
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// funkce volana pri zmene velikosti sceny
void OnReshape(int w, int h)            // event handler pro zmenu velikosti okna
{
	glViewport(0, 0, w, h);             // OpenGL: nastaveni rozmenu viewportu
	glMatrixMode(GL_PROJECTION);        // OpenGL: matice bude typu projekce
	glLoadIdentity();                   // OpenGL: matice bude identicka (jen jednicky v hlavni diagonale)
	gluPerspective(fov, (double)w/h, nearPlane, farPlane); // perspektivni projekce
}

// funkce pro inicializaci
// do teto funkce vlozit vsechny vlastnosti a inicializace, ktere se nebudou menit pri behu aplikace
void OnInit(void)
{
	// steny
	glFrontFace(GL_CW);					// nastaveni front == clockwise
	glPolygonMode(GL_FRONT, GL_FILL);   // mod predni steny fill
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// dalsi stavy a vlastnosti (napr. nastaveni svetel, materialu, ...)
}

// doporucene vykresleni objektu 
// (1) pouzijte PUSH/POP pro GL_MODELVIEW matice, abyste si nerozhodili transformace cele sceny
// (2) vykreslujte vlastni objekty okolo pocatku (0,0,0) pro snazsi provadeni transformaci a umisteni ve scene
// (3) pro ziskaji aktulanich stavovych promennych v OpenGL muzete pouzit funkce glGetBooleanv(...)
void DrawPlane(int size)
{
	// (1)
	glPushMatrix();
	
	// nastaveni transformace (pokud je treba)
	glTranslatef(0, -15, 0);
	
	// (3) nastaveni vlastnosti OpenGL
	GLboolean isLightingEnabled;
	glGetBooleanv(GL_LIGHTING, &isLightingEnabled);

	if(isLightingEnabled)
		glDisable(GL_LIGHTING);
	
	glColor3f (0.0 ,0.7 ,0.0);

	// (2) vykresleni objektu
	glBegin( GL_QUADS);
		glVertex3i(-size, 0, size);
		glVertex3i(-size, 0, -size);
		glVertex3i(size , 0, -size);
		glVertex3i(size , 0, size);
	glEnd();

	// v pripade, ze jste vypnuli/zapnuli nejaky stav, vratte ho do puvodniho
	if(isLightingEnabled)
		glEnable( GL_LIGHTING);

	glPopMatrix();
}

// funkce volana pri prekresleni sceny
void OnDisplay(void)				   
{
    // nastaveni parametru pro defaultni mazani rovin
    glClearColor(0.8, 0.8, 0.8, 0.0);  
    glClearDepth(1.0f);

    // mazani rovin
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);      
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);   

    // inicializace matice MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// provedeni transformaci sceny (pro rotaci mysi a pohyb klavesami)
	glRotatef(ynew, 1.0f, 0.0f, 0.0f);		// rotace podle osy x
    glRotatef(xnew, 0.0f, 1.0f, 0.0f);		// rotace podle osy y
	glTranslatef(tranx, trany, tranz);		// relativni zmena v z
	
	// zde vykreslovani dalsich objektu ...

	// (priklad) vykresli podlahu
	DrawPlane(100);

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
		}
		else {
			stav = 0;
			xold = xnew;
			yold = ynew;
		}
	}

	// pripadne test pro prave tlacitko pro vyvolani menu
	// ... GLUT_RIGHT_BUTTON

	glutPostRedisplay();
}

// funkce volana pri pohybu mysi a stistenem tlacitku mysi
void OnMouseMotion(int x, int y)
{ 
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;

	if (stav == 1) {
		xnew = xold + x - xx;
		ynew = -(yold + y - yy);
		uhel = xnew * PIover180;
		glutPostRedisplay();
	}
}

// funkce volana pri stisku specialnich klaves
void OnSpecial(int key, int mx, int my)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			{
				tranz += 2;	
			}
			break;
		case GLUT_KEY_DOWN:
			{
				tranz -= 2;	
			}
			break;
		case GLUT_KEY_RIGHT:
			{
				tranx -= 2;
			}
		break;
		case GLUT_KEY_LEFT:
			{
				tranx += 2;
			}
		break;
		case GLUT_KEY_PAGE_UP:
			{
				trany -= 2;
			}
			break;
		case GLUT_KEY_PAGE_DOWN:
			{
				trany += 2;
			}
			break;
		default:
			break;
	}

	glutPostRedisplay();
}

// funkce main
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);					// inicializace knihovny GLUT
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // init double buffer

	glutInitWindowSize(640, 480);			// nastaveni pocatecni velikosti dale oteviranych oken
    glutInitWindowPosition(200, 200);		// nastaveni pocatecniho umisteni dale oteviranych oken
    
	glutCreateWindow("Projekt 2022 – Autor: xkured00");	// vytvoreni okna

	// registracni funkce
	glutDisplayFunc(OnDisplay);				// registrace funkce volane pri prekreslovani aktualniho okna
    glutReshapeFunc(OnReshape);				// registrace funkce volane pri zmene velikosti aktualniho okna
    glutMouseFunc(OnMouseButton);			// registrace funkce pro stisk tlacitka mysi
    glutMotionFunc(OnMouseMotion);			// registrace funkce pro pohyb mysi pri stisknutem tlacitku
	glutSpecialFunc(OnSpecial);				// registrace funkce pro zachytavani specialnich klaves
	createMenu();

	// pripadne dalsi udalosti...

	// inicializace sceny
	OnInit();
	
	glutMainLoop();

    return 0;
}
