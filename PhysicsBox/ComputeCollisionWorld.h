#pragma once
#pragma once
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <vector>
#include "RigidBody.h"
#include "PhysicsWorld.h"

using namespace glm;

class ComputeCollisionWorld
{
public:
	unsigned int num;

	GLuint pos_ssbo;
	GLuint radius_ssbo;
	GLuint octant_ssbo;
	GLuint collisions_ssbo;

	PhysicsWorld* physicsWorld;
	unsigned int* collisions_array; // int a at index i means collision between objects (a-1) and i (0 = no collisions)
private:
	vec4* pos_array;
	float* radius_array;
	unsigned int* octant_array;

	ShaderProgram cShaderProg; // Compute shader program for broad phase
	// ShaderClass cShader_broad;
	ShaderClass cShader_narrow;
	vector<RigidBody*> rigidBodies;

public:
	ComputeCollisionWorld();
	~ComputeCollisionWorld();
	void Create(vector<RigidBody*> rigidBodies, const char* compute_shader_file_narrow);
	void Update();
};

