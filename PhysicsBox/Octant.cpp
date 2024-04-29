#include "Octant.h"

unsigned int Octant::octantCount = 0;
unsigned int Octant::maxLevel = 3;
unsigned int Octant::idealEntityCount = 5;

using namespace std;

Octant::Octant(unsigned int maxLevel, unsigned int idealEntityCount)
{
	Init();
	octantCount = 0;
	this->maxLevel = maxLevel;
	this->idealEntityCount = idealEntityCount;
	id = octantCount;

	root = this;
	usedChildNodes.clear();

	RigidBody* firstObject = physicsWorld->objects[0];
	min = firstObject->GetMinPos();
	max = firstObject->GetMaxPos();

	for (unsigned int i = 0; i < physicsWorld->objects.size(); i++)
	{
		vec3 entityMin = physicsWorld->objects[i]->GetMinPos();
		vec3 entityMax = physicsWorld->objects[i]->GetMaxPos();

		if (max.x < entityMax.x) max.x = entityMax.x;
		else if (min.x > entityMin.x) min.x = entityMin.x;

		if (max.y < entityMax.y) max.y = entityMax.y;
		else if (min.y > entityMin.y) min.y = entityMin.y;

		if (max.z < entityMax.z) max.z = entityMax.z;
		else if (min.z > entityMin.z) min.z = entityMin.z;
	}

	float longestSideLength = 0;
	for (int i = 0; i < 3; i++)
	{
		if (abs(min[i]) > longestSideLength) longestSideLength = abs(min[i]);
		if (abs(max[i]) > longestSideLength) longestSideLength = abs(max[i]);
	}
	
	min = vec3(-longestSideLength);
	max = vec3(longestSideLength);

	size = (max - min).x;
	center = (max + min) / 2.0f;

	octantCount++;
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

void Octant::Subdivide(void)
{
	// Return if at max level or already subdivided
	if (level >= maxLevel) return;

	if (numChildren > 0)
	{
		return;
	}

	float quarterWidth = size / 4;
	float halfWidth = size / 2;

	child[0] = new Octant(center + vec3(quarterWidth), halfWidth);
	child[1] = new Octant(center + vec3(-quarterWidth, quarterWidth, quarterWidth), halfWidth);
	child[2] = new Octant(center + vec3(quarterWidth, -quarterWidth, quarterWidth), halfWidth);
	child[3] = new Octant(center + vec3(quarterWidth, quarterWidth, -quarterWidth), halfWidth);
	child[4] = new Octant(center + vec3(-quarterWidth, -quarterWidth, quarterWidth), halfWidth);
	child[5] = new Octant(center + vec3(-quarterWidth, quarterWidth, -quarterWidth), halfWidth);
	child[6] = new Octant(center + vec3(quarterWidth, -quarterWidth, -quarterWidth), halfWidth);
	child[7] = new Octant(center + vec3(-quarterWidth), halfWidth);

	numChildren = 8;

	for (unsigned int i = 0; i < 8; i++)
	{
		child[i]->level = level + 1;

		if (child[i]->ContainsAtLeast(idealEntityCount))
		{
			child[i]->Subdivide();
		}
	}
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
	if (numChildren > 0) return;

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
			}
		}
	}
}

unsigned int Octant::GetOctantCount(void)
{
	return octantCount;
}

Octant::Octant(vec3 center, float size)
	: center(center), size(size)
{
	Init();
	min = center - (vec3(size) / 2.0f);
	max = center + (vec3(size) / 2.0f);

	octantCount++;
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
