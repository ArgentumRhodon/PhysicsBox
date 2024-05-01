#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h> // include glut for Mac
#else
#include <GL/freeglut.h> //include glut for Windows
#endif

#include "Camera.h"
#include "Mesh.h"
#include "PhysicsWorld.h"
#include "Octant.h"
#include "ComputeCollisionWorld.h"

using namespace std;

int g_winWidth = 1440;
int g_winHeight = 800;

char v_shader_file[] = ".\\shaders\\perVert_lambert.vert";
char f_shader_file[] = ".\\shaders\\perVert_lambert.frag";
char c_shader_file[] = ".\\shaders\\c_shader.comp";
const char meshFile[128] = "./meshes/sphere.obj";

PhysicsWorld* physicsWorld;
Camera g_cam;
Mesh* sphere;
vec3 g_lightPos = vec3(500.0f, 500.0f, 500.0f);
Octant* octree;
ComputeCollisionWorld colWorld;

unsigned char g_keyStates[256];

void init()
{
	g_cam.Set(38.0f, 13.0f, 4.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight, 45.0f, 0.01f, 10000.0f);
	sphere = new Mesh();
	sphere->Create(meshFile, v_shader_file, f_shader_file);
	physicsWorld = new PhysicsWorld(10, sphere);
	octree = new Octant(physicsWorld, 3U, 50U);
	colWorld.Create(physicsWorld->objects, c_shader_file);
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

	physicsWorld->Draw(g_cam, g_lightPos);
	octree->DisplayLeaves(g_cam);

	glutSwapBuffers();
}

void idle()
{
	physicsWorld->Update();
	octree->Update();
	colWorld.Update();
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
	glutSetOption(GLUT_MULTISAMPLE, 1);
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