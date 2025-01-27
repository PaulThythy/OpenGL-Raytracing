#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "math/AABB.h"

struct BVHNode {
	AABB m_Bounds;
	alignas(16) int m_FirstPrimitive;
	alignas(16) int m_PrimitiveCount;
	alignas(16) int m_LeftChild;
	alignas(16) int m_RightChild;

	inline BVHNode(): 
		m_Bounds(), m_FirstPrimitive(-1), m_PrimitiveCount(0), m_LeftChild(-1), m_RightChild(-1) {}

	inline bool isLeaf() const {
		return m_LeftChild == -1 && m_RightChild == -1;
	}
};

#endif