#include "ComputeCollisionWorld.h"
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <iostream>

using namespace std;

ComputeCollisionWorld::ComputeCollisionWorld()
{
	num = 0;
	pos_array = NULL;
	radius_array = NULL;
	octant_array = NULL;

	pos_ssbo = radius_ssbo = octant_ssbo = 0;

	cShaderProg = ShaderProgram();
	// cShader_broad = ShaderClass();
	cShader_narrow = ShaderClass();
}

ComputeCollisionWorld::~ComputeCollisionWorld()
{
}

void ComputeCollisionWorld::Create(vector<RigidBody*> rigidBodies, const char* compute_shader_file_narrow)
{
	this->rigidBodies = rigidBodies;
	num = rigidBodies.size();

	// cShader_broad.Create(compute_shader_file_broad, GL_COMPUTE_SHADER);
	cShader_narrow.Create(compute_shader_file_narrow, GL_COMPUTE_SHADER);
	cShaderProg.Create();

	// cShaderProg.Link(cShader_broad);
	cShaderProg.Link(cShader_narrow);

	// cShader_broad.Destroy();
	cShader_narrow.Destroy();

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	// Map and create position array
	glGenBuffers(1, &pos_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pos_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(vec4), NULL, GL_STATIC_DRAW);

	pos_array = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(vec4), bufMask);
	if (pos_array != NULL)
	{
		for (unsigned int i = 0; i < num; i++)
		{
			pos_array[i] = vec4(rigidBodies[i]->position, 1.0f);
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Map and create radius array
	glGenBuffers(1, &radius_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, radius_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), NULL, GL_STATIC_DRAW);
	radius_array = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(float), bufMask);

	if (radius_array != NULL)
	{
		for (unsigned int i = 0; i < num; i++)
		{
			radius_array[i] = rigidBodies[i]->radius;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Map and create octant index array
	glGenBuffers(1, &octant_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, octant_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	octant_array = (unsigned int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(unsigned int), bufMask);

	if (octant_array != NULL)
	{
		for (unsigned int i = 0; i < num; i++)
		{
			octant_array[i] = rigidBodies[i]->octantID;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Map and create collisions array
	glGenBuffers(1, &collisions_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisions_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);

	collisions_array = (unsigned int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(unsigned int), bufMask);

	if (collisions_array != NULL)
	{
		for (unsigned int i = 0; i < num; i++)
		{
			collisions_array[i] = 0;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Bind SSBOs
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pos_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, radius_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, octant_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, collisions_ssbo);
}

void ComputeCollisionWorld::Update()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisions_ssbo);
	unsigned int* collisionsData = (unsigned int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	if (collisionsData)
	{
		// Copy collision data to CPU-accessible array
		memcpy(collisions_array, collisionsData, num * sizeof(unsigned int));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	// Update position data
	glGenBuffers(1, &pos_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pos_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(vec4), NULL, GL_STATIC_DRAW);

	pos_array = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(vec4), bufMask);
	if (pos_array != NULL)
	{
		for (unsigned int i = 0; i < num; i++)
		{
			pos_array[i] = vec4(rigidBodies[i]->position, 1.0f);
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Update octant index data
	glGenBuffers(1, &octant_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, octant_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	octant_array = (unsigned int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(unsigned int), bufMask);

	if (octant_array != NULL)
	{
		for (unsigned int i = 0; i < num; i++)
		{
			octant_array[i] = rigidBodies[i]->octantID;
		}
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pos_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, octant_ssbo);

	glUseProgram(cShaderProg.id);
	glDispatchCompute((num + 128 - 1) / 128, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
