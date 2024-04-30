#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h> // include glut for Mac
#else
#include <GL/freeglut.h> //include glut for Windows
#endif

#include "Octant.h"

unsigned int Octant::octantCount = 0;
unsigned int Octant::maxLevel = 3;
unsigned int Octant::idealEntityCount = 5;
PhysicsWorld* Octant::physicsWorld = nullptr;
using namespace std;

Octant::Octant(PhysicsWorld* physicsWorld, unsigned int maxLevel, unsigned int idealEntityCount)
{
	Init();
	this->physicsWorld = physicsWorld;
	octantCount = 0;
	this->maxLevel = maxLevel;
	this->idealEntityCount = idealEntityCount;
	id = octantCount;

	root = this;
	usedChildNodes.clear();

	Update();
}

void Octant::Update()
{
	if (physicsWorld->objects.empty())
	{
		// No objects to update, clear and return
		KillBranches();
		entityList.clear();
		return;
	}

	// Calculate the bounding box encompassing all objects
	min = physicsWorld->objects[0]->GetMinPos();
	max = physicsWorld->objects[0]->GetMaxPos();

	for (unsigned int i = 1; i < physicsWorld->objects.size(); i++)
	{
		vec3 entityMin = physicsWorld->objects[i]->GetMinPos();
		vec3 entityMax = physicsWorld->objects[i]->GetMaxPos();

		min = glm::min(min, entityMin);
		max = glm::max(max, entityMax);
	}

	// Recalculate center and size of this octant
	center = (max + min) / 2.0f;
	vec3 extent = max - min;
	size = glm::max(glm::max(extent.x, extent.y), extent.z);

	// Adjust min and max to ensure correct size
	vec3 halfSize = vec3(size) / 2.0f;
	min = center - halfSize;
	max = center + halfSize;

	// Reconstruct the tree based on updated parameters
	octantCount = 1;
	ConstructTree(maxLevel);
}

Octant::~Octant(void)
{
	Release();
}

float Octant::GetSize(void)
{
	return size;
}

vec3 Octant::GetCenter(void)
{
	return center;
}

vec3 Octant::GetMin(void)
{
	return min;
}

vec3 Octant::GetMax(void)
{
	return max;
}

bool Octant::IsColliding(unsigned int rbIndex)
{
	if (rbIndex > physicsWorld->objects.size()) return false;

	RigidBody* entityBody = physicsWorld->objects[rbIndex];

	// AABB collision check
	if (max.x < entityBody->GetMinPos().x)
		return false;
	if (min.x > entityBody->GetMaxPos().x)
		return false;

	if (max.y < entityBody->GetMinPos().y)
		return false;
	if (min.y > entityBody->GetMaxPos().y)
		return false;

	if (max.z < entityBody->GetMinPos().z)
		return false;
	if (min.z > entityBody->GetMaxPos().z)
		return false;

	return true; // for the sake of startup code
}

void Octant::ClearEntityList(void)
{
	for (unsigned int nChild = 0; nChild < numChildren; nChild++)
	{
		child[nChild]->ClearEntityList();
	}

	entityList.clear();
}

void Octant::Subdivide()
{
    // Return if already subdivided or at max level
    if (numChildren > 0 || level >= maxLevel) {
        return;
    }

    float halfSize = size / 2.0f;
    vec3 quarterSize = vec3(halfSize) / 2.0f;

    for (unsigned int i = 0; i < 8; i++) {
        vec3 childCenter = center;
        childCenter.x += (i & 1) ? quarterSize.x : -quarterSize.x;
        childCenter.y += (i & 2) ? quarterSize.y : -quarterSize.y;
        childCenter.z += (i & 4) ? quarterSize.z : -quarterSize.z;

        child[i] = new Octant(childCenter, halfSize, root);
        child[i]->level = level + 1;

        if (child[i]->ContainsAtLeast(idealEntityCount)) {
            child[i]->Subdivide();
        }
    }

    numChildren = 8;
}

Octant* Octant::GetChild(unsigned int childIndex)
{
	if (childIndex > 7) return nullptr;
	return child[childIndex];
}

Octant* Octant::GetParent(void)
{
	return parent;
}

bool Octant::IsLeaf(void)
{
	return numChildren == 0;
}

bool Octant::ContainsAtLeast(unsigned int numEntities)
{
	// If this isn't a lead node, return false
	if (numChildren > 0) return false;

	unsigned int numEntitiesContained = 0;
	for (unsigned int i = 0; i < physicsWorld->objects.size(); i++)
	{
		if (IsColliding(i))
		{
			numEntitiesContained++;
		}

		if (numEntitiesContained > numEntities)
		{
			return true;
		}
	}

	return false;
}

void Octant::KillBranches(void)
{
	for (unsigned int nIndex = 0; nIndex < numChildren; nIndex++)
	{
		child[nIndex]->KillBranches();
		delete child[nIndex];
		child[nIndex] = nullptr;
	}
	numChildren = 0;
}

void Octant::ConstructTree(unsigned int maxLevel)
{
	if (level != 0) return;

	this->maxLevel = maxLevel;
	octantCount = 1;

	entityList.clear();
	KillBranches();
	usedChildNodes.clear();

	if (ContainsAtLeast(idealEntityCount))
	{
		Subdivide();
	}

	AssignIDToEntity();
	ConstructList();
}

void Octant::AssignIDToEntity(void)
{
	if (numChildren != 0)
	{
		for (int i = 0; i < 8; i++)
		{
			child[i]->AssignIDToEntity();
		}
	}
	else
	{
		for (unsigned int i = 0; i < physicsWorld->objects.size(); i++)
		{
			if (IsColliding(i))
			{
				physicsWorld->objects[i]->octantID = id;
				entityList.push_back(i);
			}
		}
	}
}

unsigned int Octant::GetOctantCount(void)
{
	return octantCount;
}

Octant::Octant(vec3 center, float size, Octant* root)
{
	Init();
	this->center = center;
	this->size = size;
	min = center - (vec3(size) / 2.0f);
	max = center + (vec3(size) / 2.0f);

	this->root = root;

	octantCount++;
}

void Octant::Display(Camera g_cam)
{
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(value_ptr(translate(g_cam.viewMat, center)));
	glColor3f(1.0f, 0.6f, 0.6f);
	glutWireCube(size);
	glPopMatrix();
}

void Octant::DisplayLeaves(Camera g_cam)
{
	unsigned int nLeaves = usedChildNodes.size();
	root->Display(g_cam);
	for (unsigned int nChild = 0; nChild < nLeaves; nChild++)
	{
		root->usedChildNodes[nChild]->Display(g_cam);
	}
}

// Big Three
//Octant::Octant(Octant const& other)
//{
//}

//Octant& Octant::operator=(Octant const& other)
//{
//	// TODO: insert return statement here
//}

void Octant::Release(void)
{
	if (level == 0)
	{
		KillBranches();
	}
	numChildren = 0;
	size = 0.0f;
	entityList.clear();
	usedChildNodes.clear();
}

void Octant::Init(void)
{
	numChildren = 0;

	size = 0.0f;

	id = octantCount;
	level = 0;

	center = vec3(0.0f);
	min = vec3(0.0f);
	max = vec3(0.0f);

	root = nullptr;
	parent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		child[n] = nullptr;
	}
}

void Octant::ConstructList(void)
{
	for (unsigned int nChild = 0; nChild < numChildren; nChild++)
	{
		child[nChild]->ConstructList();
	}

	if (entityList.size() > 0)
	{
		root->usedChildNodes.push_back(this);
	}
}
