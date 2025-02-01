#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "math/AABB.h"
#include "globals/Globals.h"

struct BVHNode {
	AABB m_Bounds;
	int m_PrimitiveIndices[Config::BVH_MAX_TRIANGLES_PER_LEAF];
	int m_LeftChild = -1;
	int m_RightChild = -1;

	//for data alignment for std430 layout
	int padding[2] = {0, 0};

	inline bool isLeaf() const {
		return m_LeftChild == -1 && m_RightChild == -1;
	}
};

#endif