#ifndef BVH_H
#define BVH_H

#include <vector>

#include "BVHNode.h"
#include "math/Triangle.h"

class BVH {
public:
	std::vector<BVHNode> m_Nodes;
	std::vector<Triangle> m_OrderedTriangles;
	int m_RootNodeIndex;

	inline BVH(): m_RootNodeIndex(0) {}

	void build(const std::vector<Triangle>& triangles);

private:
	AABB computeTriangleAABB(const Triangle& triangle);
	void buildRecursive(int nodeIndex, int start, int end, std::vector<AABB>& primitiveBounds);
	
};

#endif