#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Camera.h"

using namespace glm;
using namespace std;

class RigidBody
{
public:
	bool useGravity = true;
	float mass;
	float drag;
	Mesh* mesh;
	vec3 position;
	float radius;
	unsigned int octantID;

private:
	vec3 velocity;
	vec3 acceleration;
	vec3 force;

public:
	RigidBody(vec3 position, Mesh* mesh, float mass = 1.0f, float drag = 0.0f, float radius = 1.0f);
	void TranslateResolve(float deltaTime);
	void AddForce(vec3 torque);
	vec3 GetMinPos();
	vec3 GetMaxPos();
	void DisplayBoundingSphere(Camera g_cam);
};

