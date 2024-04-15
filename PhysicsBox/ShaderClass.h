#ifndef SHADERCLASS_H
#define SHADERCLASS_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#pragma once
class ShaderClass
{
public:
	unsigned int id;
public:
	ShaderClass(void);
	~ShaderClass(void);
	void Create(const char* shaderFileName, GLenum targetType);
	void Destroy();
private:
	char* loadShaderFile(const char* fileName);
	void printShaderInfoLog(unsigned int shader_id);
};

#endif