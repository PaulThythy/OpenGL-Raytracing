#ifndef BVH_H
#define BVH_H

#include <vector>

#include "BVHNode.h"
#include "math/Triangle.h"
#include "math/AABB.h"
#include "globals/Globals.h"

class BVH {
public:
	std::vector<BVHNode> m_Nodes;
	int m_RootNodeIndex = -1;

	inline BVH(): m_RootNodeIndex(-1) {}

	void build(const std::vector<Triangle>& triangles, const int maxTrianglesPerLeaf = Config::BVH_MAX_TRIANGLES_PER_LEAF);

private:
	int buildRecursive(const std::vector<Triangle>& triangles, std::vector<int>& triangleIndices, int start, int end, const int maxTrianglesPerLeaf);
	AABB computeAABB(const std::vector<Triangle>& triangles, const std::vector<int>& triangleIndices, int start, int end);
	glm::vec3 computeCentroid(const Triangle& triangle);
};

#endif