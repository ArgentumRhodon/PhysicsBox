#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld(unsigned int numObjects, Mesh* mesh)
	: deltaTime(0), timeSinceStart(0), lastTimeSinceStart(0)
{
	RigidBody* rb;

	for (int i = 0; i < numObjects; i++)
	{
		vec3 pos = vec3(0, i, 0);
		rb = new RigidBody(pos, mesh);
		objects.push_back(rb);
	}
}

PhysicsWorld::~PhysicsWorld()
{
}

void PhysicsWorld::Update()
{
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	deltaTime = timeSinceStart - lastTimeSinceStart;
	lastTimeSinceStart = timeSinceStart;

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->TranslateResolve(deltaTime);
	}
}

void PhysicsWorld::Draw(Camera g_cam, vec3 g_lightPos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->mesh->Draw(translate(g_cam.viewMat, objects[i]->position), g_cam.projMat, g_lightPos);
	}
}
