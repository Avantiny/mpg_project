#pragma once

#include <GL/glut.h>

void drawCuboid(float x, float y, float z, float r, float g, float b, float lengthx, float lengthy, float lengthz)
{
	// predni stena
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex3f(x, y, z);
	glVertex3f(x + lengthx, y, z);
	glVertex3f(x + lengthx, y + lengthy, z);
	glVertex3f(x, y + lengthy, z);
	glEnd();

	// zadni stena
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex3f(x, y, z + lengthz);
	glVertex3f(x + lengthx, y, z + lengthz);
	glVertex3f(x + lengthx, y + lengthy, z + lengthz);
	glVertex3f(x, y + lengthy, z + lengthz);
	glEnd();

	// prava stena
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex3f(x + lengthx, y, z);
	glVertex3f(x + lengthx, y, z + lengthz);
	glVertex3f(x + lengthx, y + lengthy, z + lengthz);
	glVertex3f(x + lengthx, y + lengthy, z);
	glEnd();

	// leva stena
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + lengthz);
	glVertex3f(x, y + lengthy, z + lengthz);
	glVertex3f(x, y + lengthy, z);
	glEnd();

	// horni stena
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex3f(x, y + lengthy, z);
	glVertex3f(x, y + lengthy, z + lengthz);
	glVertex3f(x + lengthx, y + lengthy, z + lengthz);
	glVertex3f(x + lengthx, y + lengthy, z);
	glEnd();
}




void drawWindow(float xwin, float ywin, float zwin)
{
	glBegin(GL_QUADS);
	glColor3f(0.8, 0.97, 1);
	glVertex3f(xwin, ywin, zwin);
	glVertex3f(xwin + 1, ywin, zwin);
	glVertex3f(xwin + 1, ywin + 1, zwin);
	glVertex3f(xwin, ywin + 1, zwin);
	glEnd();
}


void drawCloud(float xcloud, float ycloud, float zcloud)
{
	// parametry pro bezierovy platy
	float t = 0.0;
	int numVer = 100; //pocet vertexu
	float X0, Z0, X1, Z1, X2, Z2, X3, Z3;
	float x, z;

	float X0puv = 0;
	float Z0puv = 396;

	X0 = 0;
	Z0 = 0;
	X1 = 20 - X0puv;
	Z1 = 396 - Z0puv;
	X2 = 16 - X0puv;
	Z2 = 345 - Z0puv;
	X3 = 0 - X0puv;
	Z3 = 345 - Z0puv;


	glBegin(GL_POLYGON);

	for (int i = 0; i <= numVer; i++)
	{
		glColor3f(0.91, 0.96, 1);
		x = t * t * t * (-1 * X0 + 3 * X1 - 3 * X2 + X3) + t * t * (3 * X0 - 6 * X1 + 3 * X2) + t * (-3 * X0 + 3 * X1) + X0;
		z = t * t * t * (-1 * Z0 + 3 * Z1 - 3 * Z2 + Z3) + t * t * (3 * Z0 - 6 * Z1 + 3 * Z2) + t * (-3 * Z0 + 3 * Z1) + Z0;

		glVertex3f(xcloud + x, ycloud, zcloud + z);
		t += 1.0 / numVer;
	}
	glEnd();

	t = 0.0;
	glBegin(GL_POLYGON);
	for (int i = 0; i <= numVer; i++)
	{

		x = t * t * t * (-1 * X0 + 3 * X1 - 3 * X2 + X3) + t * t * (3 * X0 - 6 * X1 + 3 * X2) + t * (-3 * X0 + 3 * X1) + X0;
		z = t * t * t * (-1 * Z0 + 3 * Z1 - 3 * Z2 + Z3) + t * t * (3 * Z0 - 6 * Z1 + 3 * Z2) + t * (-3 * Z0 + 3 * Z1) + Z0;

		glVertex3f(xcloud - x, ycloud, zcloud + z);
		t += 1.0 / numVer;
	}
	glEnd();


}

void drawSun(float xsun, float ysun, float zsun)
{
	glPushMatrix();
	glTranslatef(xsun, ysun, zsun);

	GLUquadric* q = gluNewQuadric();
	glColor3f(1, 1, 0);
	gluSphere(q, 15, 20, 20);
	gluDeleteQuadric(q);

	glPopMatrix();
}

void drawChair(float xchair, float ychair, float zchair) {
	glPushMatrix();
	glTranslatef(xchair, ychair, zchair);

	float r = 0.5;
	float g = 0.35;
	float b = 0.05;

	float side = 0.3;
	float height = 1;

	//nohy
	drawCuboid(xchair, ychair, zchair, r, g, b, side, height, side);
	drawCuboid(xchair + 1, ychair, zchair, r, g, b, side, height, side);
	drawCuboid(xchair + 1, ychair, zchair + 1, r, g, b, side, height, side);
	drawCuboid(xchair, ychair, zchair + 1, r, g, b, side, height, side);

	//sedadlo
	drawCuboid(xchair, ychair + height, zchair, r, g, b, 1 + side, 0.2, 1 + side);

	//operadlo
	drawCuboid(xchair, ychair + height, zchair, r, g, b, 1 + side, 2, 0.2);


	glPopMatrix();
}

void drawWell(float xwell, float ywell, float zwell) {
	glPushMatrix();
	glTranslatef(xwell, ywell, zwell);

	float r = 0.5;
	float g = 0.5;
	float b = 0.5;

	float width = 0.5;
	float height = 2;
	float depth = 3;

	//steny
	drawCuboid(xwell, ywell, zwell, r, g, b, depth, height, width);
	drawCuboid(xwell + depth, ywell, zwell, r, g, b, width, height, depth + width);
	drawCuboid(xwell, ywell, zwell, r, g, b, width, height, depth);
	drawCuboid(xwell, ywell, zwell + depth, r, g, b, depth, height, width);

	//voda
	glBegin(GL_QUADS);
	glColor3f(0, 0, 1);
	glVertex3f(xwell, -0.3, zwell);
	glVertex3f(xwell + depth, -0.3, zwell);
	glVertex3f(xwell + depth, -0.3, zwell + depth);
	glVertex3f(xwell, -0.3, zwell + depth);
	glEnd();

	glPopMatrix();
}

void drawTree(float xtree, float ytree, float ztree, bool isGreen) {
	glPushMatrix();
	glTranslatef(xtree, ytree, ytree);

	float r = 0.6;
	float g = 0.35;
	float b = 0.05;

	float rl = 1;
	float gl = 1;
	float bl = 1;

	float side = 1.5;
	float height = 6;

	//kmen

	drawCuboid(xtree, ytree, ztree, r, g, b, side, height, side);

	float bigSide = 4;
	float midSide = 3;
	float smallSide = 2;
	float treeHeight = 2;

	if (isGreen) {
		rl = 0;
		gl = 1;
		bl = 0;
	}

	//vrstvy listu
	drawCuboid(xtree - bigSide + 0.5 * side, ytree + height, ztree - bigSide + 0.5 * side, rl, gl, bl, 2 * bigSide, treeHeight, 2 * bigSide);
	drawCuboid(xtree - midSide + 0.5 * side, ytree + height + treeHeight, ztree - midSide + 0.5 * side, rl, gl, bl, 2 * midSide, treeHeight, 2 * midSide);
	drawCuboid(xtree - smallSide + 0.5 * side, ytree + height + 2 * treeHeight, ztree - smallSide + 0.5 * side, rl, gl, bl, 2 * smallSide, treeHeight, 2 * smallSide);

	glPopMatrix();
}

void drawFence(float xfence, float yfence, float zfence, float length, float xwidth, float yheight) {
	glPushMatrix();
	glTranslatef(xfence, yfence, zfence);

	for (int i = 0; i < length; i++) {
		if (i % 2 != 0) {
			glBegin(GL_QUADS);
			glColor3f(0, 0, 0);
			glVertex3f(xfence + i, yfence, zfence);
			glVertex3f(xfence + i + xwidth, yfence, zfence);
			glVertex3f(xfence + i + xwidth, yfence + yheight, zfence);
			glVertex3f(xfence + i, yfence + yheight, zfence);
			glEnd();
		}
	}

	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	glVertex3f(xfence - 0.5, yfence + 2, zfence);
	glVertex3f(xfence + length + 0.5, yfence + 2, zfence);
	glVertex3f(xfence + length + 0.5, yfence + 2.5, zfence);
	glVertex3f(xfence - 0.5, yfence + 2.5, zfence);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	glVertex3f(xfence - 0.5, yfence + 1.2, zfence);
	glVertex3f(xfence + length + 0.5, yfence + 1.2, zfence);
	glVertex3f(xfence + length + 0.5, yfence + 1.7, zfence);
	glVertex3f(xfence - 0.5, yfence + 1.7, zfence);
	glEnd();

	glPopMatrix();
}

void drawHouse(float xhouse, float yhouse, float zhouse, float r, float g, float b, float uhelX)
{
	glPushMatrix();
	glRotatef(uhelX, 0, 1, 0);

	float width = 8;
	float length = 6.5;
	float baseHeight = 5;
	float roofHeight = 3;

	// zakladna
	drawCuboid(xhouse, yhouse, zhouse, r, g, b, width, baseHeight, length);

	// strecha
	glColor3f(1, 0, 0);
	//predni
	glBegin(GL_TRIANGLES);
	glVertex3f(xhouse, yhouse + baseHeight, zhouse);
	glVertex3f(xhouse + width, yhouse + baseHeight, zhouse);
	glVertex3f(xhouse + (width / 2), yhouse + baseHeight + roofHeight, zhouse + (length / 2));
	glEnd();
	// zadni
	glBegin(GL_TRIANGLES);
	glVertex3f(xhouse, yhouse + baseHeight, zhouse + length);
	glVertex3f(xhouse + width, yhouse + baseHeight, zhouse + length);
	glVertex3f(xhouse + (width / 2), yhouse + baseHeight + roofHeight, zhouse + (length / 2));
	glEnd();
	// prava
	glBegin(GL_TRIANGLES);
	glVertex3f(xhouse + width, yhouse + baseHeight, zhouse);
	glVertex3f(xhouse + width, yhouse + baseHeight, zhouse + length);
	glVertex3f(xhouse + (width / 2), yhouse + baseHeight + roofHeight, zhouse + (length / 2));
	glEnd();
	// leva
	glBegin(GL_TRIANGLES);
	glVertex3f(xhouse, yhouse + baseHeight, zhouse);
	glVertex3f(xhouse, yhouse + baseHeight, zhouse + length);
	glVertex3f(xhouse + (width / 2), yhouse + baseHeight + roofHeight, zhouse + (length / 2));
	glEnd();

	// komin
	drawCuboid(xhouse + width / 3, yhouse + baseHeight, zhouse + length / 6, 0.4, 0.4, 0.4, 1, roofHeight + 0.3, 1);


	// okna
	// predni
	drawWindow(xhouse + width / 4, yhouse + baseHeight / 1.6, zhouse - 0.01);
	drawWindow(xhouse + (width - width / 4 - 1), yhouse + baseHeight / 1.6, zhouse - 0.01);
	// zadni
	drawWindow(xhouse + width / 4, yhouse + baseHeight / 1.6, zhouse + 0.01 + length);
	drawWindow(xhouse + (width - width / 4 - 1), yhouse + baseHeight / 1.6, zhouse + 0.01 + length);


	// dvere
	glColor3f(0.3, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f((xhouse + width / 2) - 0.5, yhouse, zhouse - 0.01);
	glVertex3f((xhouse + width / 2) - 0.5, yhouse + 2, zhouse - 0.01);
	glVertex3f((xhouse + width / 2) - 0.5 + 1, yhouse + 2, zhouse - 0.01);
	glVertex3f((xhouse + width / 2) - 0.5 + 1, yhouse, zhouse - 0.01);

	glEnd();


	glPopMatrix();
}