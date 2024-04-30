#include "PhysicsWorld.h"

float PhysicsWorld::randomf(float min, float max)
{
	if (max < min) return 0.0f;
	float n = ((float)rand()) / ((float)RAND_MAX);
	float range = max - min;
	return n * range + min;
}

PhysicsWorld::PhysicsWorld(unsigned int numObjects, Mesh* mesh)
	: deltaTime(0), timeSinceStart(0), lastTimeSinceStart(0)
{
	RigidBody* rb;

	for (unsigned int i = 0; i < numObjects; i++)
	{
		for (unsigned int j = 0; j < numObjects; j++) {
			for (unsigned int k = 0; k < numObjects; k++)
			{
				vec3 pos = vec3(4*i, 4*j, 4*k);
				rb = new RigidBody(pos, mesh);
				rb->useGravity = false;
				objects.push_back(rb);
			}
		}
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
		objects[i]->AddForce(vec3(randomf(-0.125, 0.125), randomf(-0.125, 0.125), randomf(-0.125, 0.125)));
	}
}

void PhysicsWorld::Draw(Camera g_cam, vec3 g_lightPos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->mesh->Draw(translate(g_cam.viewMat, objects[i]->position), g_cam.projMat, g_lightPos);
		// objects[i]->DisplayBoundingSphere(g_cam);
	}
}
