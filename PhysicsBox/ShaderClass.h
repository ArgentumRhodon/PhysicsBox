#include <GL/glew.h>
#include <string>;

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

