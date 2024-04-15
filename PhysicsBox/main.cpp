#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h> // include glut for Mac
#else
#include <GL/freeglut.h> //include glut for Windows
#endif

#include "Camera.h"
#include "Mesh.h"

using namespace std;

int g_winWidth = 1440;
int g_winHeight = 800;

char v_shader_file[] = ".\\shaders\\basic.vert";
char f_shader_file[] = ".\\shaders\\basic.frag";
const char meshFile[128] = "./meshes/sphere.obj";

Camera g_cam;
Mesh sphere;
vec3 g_lightPos = vec3(1.0f, 1.0f, 1.0f);
float g_time = 0.0f;

unsigned char g_keyStates[256];

void init()
{
	g_cam.Set(38.0f, 13.0f, 4.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight, 45.0f, 0.01f, 10000.0f);
	sphere.Create(meshFile, v_shader_file, f_shader_file);
}

void initialGL()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_cam.DrawGrid();
	g_time = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	sphere.Draw(g_cam.viewMat, g_cam.projMat, g_lightPos, g_time);

	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
	if (h == 0)
	{
		h = 1;
	}
	g_cam.SetProjectionMatrix(g_winWidth, g_winHeight);
	g_cam.SetViewMatrix();
	glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
	g_cam.MouseClick(button, state, x, y, g_winWidth, g_winHeight);
}

void motion(int x, int y)
{
	g_cam.MouseMotion(x, y, g_winWidth, g_winHeight);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutSetOption(GLUT_MULTISAMPLE, 4);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowSize(g_winWidth, g_winHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("PhysicsBox: A Simple Physics Simulation");

	glewInit();
	initialGL();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);

	init();

	glutMainLoop();
	return EXIT_SUCCESS;
}