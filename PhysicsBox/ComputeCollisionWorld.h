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

using namespace glm;

class ComputeCollisionWorld
{
public:
	unsigned int num;

	GLuint pos_ssbo;
	GLuint radius_ssbo;

private:
	vec4* pos_array;
	float* radius_array;

	ShaderProgram cShaderProg; // Compute shader program for broad phase
	ShaderClass cShader_broad;
	ShaderClass cShader_narrow;

public:
	ComputeCollisionWorld();
	~ComputeCollisionWorld();
	void Create(vector<RigidBody*> rigidBodies, const char* compute_shader_file_broad, const char* compute_shader_file_narrow);
	void Update();
};

