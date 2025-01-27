#ifndef BVH_H
#define BVH_H

#include <vector>

#include "BVHNode.h"
#include "math/Triangle.h"
#include "math/AABB.h"
class BVH {
public:
	std::vector<BVHNode> m_Nodes;
	std::vector<Triangle> m_OrderedTriangles;
	alignas(16)int m_RootNodeIndex;

	inline BVH(): m_RootNodeIndex(-1) {}

	void build(const std::vector<Triangle>& triangles, int maxTrianglesPerLeaf = 4);

private:
	int buildRecursive(int start, int end, const std::vector<AABB>& triangleBounds, int maxTrianglesPerLeaf);
};

#endif