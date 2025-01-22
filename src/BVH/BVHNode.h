#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "math/AABB.h"

struct BVHNode {
	AABB m_Bounds;
	int m_FirstPrimitive;
	int m_PrimitiveCount;
	int m_LeftChild;
	int m_RightChild;

	inline BVHNode(): m_FirstPrimitive(0), m_PrimitiveCount(0), m_LeftChild(-1), m_RightChild(-1) {}
};

#endif