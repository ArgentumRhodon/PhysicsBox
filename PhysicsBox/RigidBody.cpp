#include "RigidBody.h"

RigidBody::RigidBody(vec3 position, Mesh* mesh, float mass, float drag)
	: position(position), mesh(mesh), mass(mass), drag(drag)
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