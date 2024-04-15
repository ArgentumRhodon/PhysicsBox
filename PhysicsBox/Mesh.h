#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include "ShaderProgram.h";
#include <fstream>
#include <sstream>

using namespace glm;
using namespace std;

#pragma once
class Mesh
{
public:
	uint vert_num, tri_num;
	vec3* vertices;
	uvec3* triangles;
	vec3* faceNormals;
	vec3* vertexNormals;

	GLuint vao, vbo, nbo, ibo;
	ShaderProgram shaderProg;
	ShaderClass vShader;
	ShaderClass fShader;

	mat4 modelMat;
public:
	Mesh();
	~Mesh();

	void Create(const char* filename, const char* v_shader_file, const char* f_shader_file);
	void Draw(mat4 viewMat, mat4 projMat, vec3 lightPos, float time);
private:
	void computeNormals();
	void prepareVBOAndShaders(const char* v_shader_file, const char* f_shader_file);
};

