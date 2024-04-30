#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h> // include glut for Mac
#else
#include <GL/freeglut.h> //include glut for Windows
#endif

#include "RigidBody.h"

RigidBody::RigidBody(vec3 position, Mesh* mesh, float mass, float drag, float radius)
	: position(position), mesh(mesh), mass(mass), drag(drag), radius(radius), octantID(0)
{
}

void RigidBody::TranslateResolve(float deltaTime)
{
	force = vec3(0);

	// Apply gravity
	if (useGravity) force += vec3(0, mass * -9.8f, 0);

	// Drag
	force -= 0.5f * drag * velocity * length(velocity);

	// Velocity verlet numerical solution for translational motion
	position += (velocity * deltaTime) + (0.5f * acceleration * deltaTime * deltaTime);
	vec3 halfStepVelocity = velocity + (0.5f * acceleration * deltaTime);
	acceleration = (1 / mass) * force;
	velocity = halfStepVelocity + (0.5f * acceleration * deltaTime);
}

/// <summary>
/// Adds an impulse force to the rigidbody's velocity
/// </summary>
/// <param name="force"></param>
void RigidBody::AddForce(vec3 force)
{
	velocity += force / mass;
}

vec3 RigidBody::GetMinPos()
{
	return position - vec3(radius);
}

vec3 RigidBody::GetMaxPos()
{
	return position + vec3(radius);
}

void RigidBody::DisplayBoundingSphere(Camera g_cam)
{
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(value_ptr(translate(g_cam.viewMat, position)));
	glColor3f(0.0f, 0.0f, 1.0f);
	glutWireSphere(radius, 10, 10);
	glPopMatrix();
}
