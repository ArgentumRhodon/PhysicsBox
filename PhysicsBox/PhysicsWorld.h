#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "RigidBody.h"
#include "Camera.h"
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h> // include glut for Mac
#else
#include <GL/freeglut.h> //include glut for Windows
#endif

using namespace std;
using namespace glm;

class PhysicsWorld
{
private:
	float timeSinceStart;
	float lastTimeSinceStart;
	float deltaTime;
	float randomf(float min, float max);
public:
	vector<RigidBody*> objects;
	PhysicsWorld(unsigned int numObjects, Mesh* mesh);
	~PhysicsWorld();
	void Update();
	void Draw(Camera g_cam, vec3 g_lightPos);
};

