#include "Mesh.h"
#include <fstream>

Mesh::Mesh()
{
	vert_num = tri_num = 0;
	vao = vbo = nbo = ibo = 0;
	modelMat = mat4(1.0f);
}

Mesh::~Mesh()
{
	delete[] vertices;
	delete[] triangles;
	delete[] faceNormals;
	delete[] vertexNormals;
}

void Mesh::Create(const char* filename, const char* v_shader_file, const char* f_shader_file)
{
	vector<vec3> ori_vertices;
	vector<uvec3> ori_triangles;

	ifstream fs(filename);

	char c;
	vec3 pos;
	int index[3];
	int vid = 0;

	// To be continued
}

void Mesh::Draw(mat4 viewMat, mat4 projMat, vec3 lightPos, float time)
{
}

void Mesh::computeNormals()
{
	faceNormals = new vec3(tri_num);
	vertexNormals = new vec3(vert_num);

	vec3 a, b, n;

	for (unsigned int i = 0; i < tri_num; i++)
	{ 
		// Side 1 Vec
		a = vertices[triangles[i][1]] - vertices[triangles[i][0]];
		// Side 2 Vec
		b = vertices[triangles[i][2]] - vertices[triangles[i][0]];

		// Compute face normal
		n = cross(a, b);
		faceNormals[i] = normalize(n);
	}

	// Initialize vertex normals
	for (unsigned int i = 0; i < vert_num; i++)
	{
		vertexNormals[i] = vec3(0.0f);
	}

	// Compute vertex normal directions
	for (unsigned int i = 0; i < tri_num; i++)
	{
		for (unsigned int j; j < 3; j++)
		{
			vertexNormals[triangles[i][j]] += faceNormals[i];
		}
	}

	// Normalize vertex normals
	for (unsigned int i = 0; i < vert_num; i++)
	{
		vertexNormals[i] = normalize(vertexNormals[i]);
	}
}

void Mesh::prepareVBOAndShaders(const char* v_shader_file, const char* f_shader_file)
{
	vShader.Create(v_shader_file, GL_VERTEX_SHADER);
	fShader.Create(f_shader_file, GL_FRAGMENT_SHADER);
	shaderProg.Create();
	shaderProg.Link(vShader);
	shaderProg.Link(fShader);

	vShader.Destroy();
	fShader.Destroy();

	// Send data to the GPU memory
	// Create VBO for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vert_num, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Do the same for normals and indicies
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vert_num, vertexNormals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvec3) * tri_num, triangles, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Make data accesssible from shaders with VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
