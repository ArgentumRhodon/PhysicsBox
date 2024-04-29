#pragma once
#include "PhysicsWorld.h"

class Octant
{
	static unsigned int octantCount;
	static unsigned int maxLevel; // Max octant level
	static unsigned int idealEntityCount; // Ideal contain count

	unsigned int id = 0; // Id
	unsigned int level = 0; // Current level
	unsigned int numChildren = 0; // (0 || 8)

	float size = 0.0f;

	PhysicsWorld* physicsWorld = nullptr;

	vec3 center = vec3(0.0f);
	vec3 min = vec3(0.0f);
	vec3 max = vec3(0.0f);

	Octant* parent = nullptr;
	Octant* child[8] = { nullptr };

	std::vector<unsigned int> entityList; // List of indicies of contained objects in this octant

	Octant* root = nullptr; // Root octant
	std::vector<Octant*> usedChildNodes; // List of child nodes with objects (root only)

public:
	// USED FOR ROOT ONLY
	Octant(unsigned int maxLevel = 2, unsigned int idealEntityCount = 5);

	~Octant(void);

	float GetSize(void);

	vec3 GetCenter(void);

	vec3 GetMin(void);

	vec3 GetMax(void);

	// Checks if rb is colliding with node
	bool IsColliding(unsigned int rbIndex);

	void ClearEntityList(void);

	void Subdivide(void);

	Octant* GetChild(unsigned int childIndex);

	Octant* GetParent(void);

	bool IsLeaf(void);

	bool ContainsAtLeast(unsigned int numEntities);

	void KillBranches(void);

	void ConstructTree(unsigned int maxLevel = 3);

	void AssignIDToEntity(void);

	unsigned int GetOctantCount(void);

private:
	Octant(vec3 center, float size);

	// Big Three
	//Octant(Octant const& other);

	//Octant& operator=(Octant const& other);

	void Release(void);

	void Init(void);

	void ConstructList(void);
};

