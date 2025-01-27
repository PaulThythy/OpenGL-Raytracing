#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "math/AABB.h"
#include "globals/Globals.h"

struct BVHNode {
	AABB m_Bounds;
	int m_PrimitiveIndices[Config::BVH_MAX_TRIANGLES_PER_LEAF];
	alignas(16) int m_LeftChild;
	alignas(16) int m_RightChild;

	inline BVHNode(): m_LeftChild(-1), m_RightChild(-1) {
		// Initialize primitive indices to -1 (invalid index)
		for (int i = 0; i < Config::BVH_MAX_TRIANGLES_PER_LEAF; i++) {
			m_PrimitiveIndices[i] = -1;
		}
	}

	inline bool isLeaf() const {
		return m_LeftChild == -1 && m_RightChild == -1;
	}
};

#endif