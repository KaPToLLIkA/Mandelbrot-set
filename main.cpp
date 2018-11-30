//#include "Complex.h"

#include <iostream>
#include <windows.h>
#include <cmath>
#include <GL/glut.h>
#include <GL/GLAUX.H>

const int	NumberOfIterations = 500,
			radius = 16;

int		wHeight = 600, //window size
		wWidth = 800;

float	scale = 180;

double  x, y;

void reshape(int w, int h)
{
	wHeight = h;
	wWidth = w;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for (int y = 0; y < wHeight; y++) //���������� ���������
		for (int x = 0; x < wWidth; x++)
		{
			Complex z(0, 0);
			int i = 0;
			while (i < NumberOfIterations && z.abs() < radius) {
				z = z * z + Complex(((x - wWidth / 2) / scale), ((y - wHeight / 2) / scale));
				i++;
			}
			double r = 0.1 + i * 0.03 * 0.2; //������ 
			double g = 0.2 + i * 0.03 * 0.3; //��������
			double b = 0.3 + i * 0.03 * 0.1; //��� ��������� ���������
			glColor3d(r, g, b);
			glVertex2d(x, y);
		}
	glEnd();

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Mandelbrot set");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();
}