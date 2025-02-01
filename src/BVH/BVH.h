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

	void build(const std::vector<Triangle>& triangles, const int maxTrianglesPerLeaf = Config::BVH_MAX_TRIANGLES_PER_LEAF);

private:
	int buildRecursive(const std::vector<Triangle>& triangles, std::vector<int>& triangleIndices, const int maxTrianglesPerLeaf);
	void partitionSAH(const std::vector<Triangle>& triangles, std::vector<int>& triangleIndices, std::vector<int>& left, std::vector<int>& right);
};

#endif