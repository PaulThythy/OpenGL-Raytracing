#include "BVH.h"

void BVH::build(const std::vector<Triangle>& triangles) {
    m_OrderedTriangles = triangles;
	std::vector<AABB> primitiveBounds(triangles.size());

	for (size_t i = 0; i < triangles.size(); i++) {
		primitiveBounds[i] = computeTriangleAABB(triangles[i]);
	}

	m_Nodes.push_back(BVHNode());
	m_RootNodeIndex = 0;

	buildRecursive(m_RootNodeIndex, 0, triangles.size(), primitiveBounds);
}

AABB BVH::computeTriangleAABB(const Triangle& triangle) {
	AABB bounds;
	bounds.expandToInclude(triangle.m_V0.m_Position);
	bounds.expandToInclude(triangle.m_V1.m_Position);
	bounds.expandToInclude(triangle.m_V2.m_Position);
	return bounds;
}

void BVH::buildRecursive(int nodeIndex, int start, int end, std::vector<AABB>& primitiveBounds) {
    BVHNode& node = m_Nodes[nodeIndex];
    node.m_FirstPrimitive = start;
    node.m_PrimitiveCount = end - start;

    AABB nodeBounds;
    for (int i = start; i < end; i++) {
        nodeBounds.expandToInclude(primitiveBounds[i]);
    }
    node.m_Bounds = nodeBounds;

    if (node.m_PrimitiveCount <= 4) {
        return;
    }

    glm::vec3 extent = nodeBounds.m_Max - nodeBounds.m_Min;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    int mid = start + node.m_PrimitiveCount / 2;

    std::nth_element(
        m_OrderedTriangles.begin() + start,
        m_OrderedTriangles.begin() + mid,
        m_OrderedTriangles.begin() + end,
        [axis](const Triangle& a, const Triangle& b) {
            return a.m_V0.m_Position[axis] < b.m_V0.m_Position[axis];
        }
    );

    node.m_LeftChild = m_Nodes.size();
    m_Nodes.push_back(BVHNode());
    node.m_RightChild = m_Nodes.size();
    m_Nodes.push_back(BVHNode());

    buildRecursive(node.m_LeftChild, start, mid, primitiveBounds);
    buildRecursive(node.m_RightChild, mid, end, primitiveBounds);
}