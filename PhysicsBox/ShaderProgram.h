#pragma once

#include <GL/glew.h>
#include <string>;
#include "ShaderClass.h"

class ShaderProgram
{
public:
	unsigned int id;
public:
	ShaderProgram(void);
	~ShaderProgram(void);
	void Create();
	void Link(ShaderClass shader);

	void use();

	//functions to set uniform variables
	void SetFloat(const char* name, float value);
	void SetInt(const char* name, int value);
	void SetFloat1V(const char* name, unsigned int count, const float* floatPtr);
	void SetFloat3V(const char* name, unsigned int count, const float* floatPtr);
	void SetMatrix4fv(const char* name, unsigned int count, const float* floatPtr);
	
	void CreateDataTexture(unsigned int& pGLTextID, float* pData, unsigned int pMaxWidth, unsigned int pMaxHeight);

	void SetSampler(const char* sampleName, GLuint textureUnit);

private:
	void printProgramInfoLog(unsigned int prog_id);
};

