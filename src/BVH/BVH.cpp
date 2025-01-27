#include "BVH.h"

void BVH::build(const std::vector<Triangle>& triangles, int maxTrianglesPerLeaf) {
    m_OrderedTriangles = triangles;
    std::vector<AABB> triangleBounds(triangles.size());

    for (size_t i = 0; i < triangles.size(); i++) {
        triangleBounds[i] = AABB();
        triangleBounds[i].expandToInclude(triangles[i].m_V0.m_Position);
        triangleBounds[i].expandToInclude(triangles[i].m_V1.m_Position);
        triangleBounds[i].expandToInclude(triangles[i].m_V2.m_Position);
    }

    m_RootNodeIndex = buildRecursive(0, triangles.size(), triangleBounds, maxTrianglesPerLeaf);
}

int BVH::buildRecursive(int start, int end, const std::vector<AABB>& primitiveBounds, int maxTrianglesPerLeaf) {
    BVHNode node;
    node.m_FirstPrimitive = start;
    node.m_PrimitiveCount = end - start;

    node.m_Bounds = AABB();
    for (int i = start; i < end; i++) {
        node.m_Bounds.expandToInclude(primitiveBounds[i]);
    }

    if(node.m_PrimitiveCount <= maxTrianglesPerLeaf) {
        node.m_LeftChild = -1;
        node.m_RightChild = -1;
        m_Nodes.push_back(node);
        return m_Nodes.size() - 1;
    }

    glm::vec3 extent = node.m_Bounds.m_Max - node.m_Bounds.m_Min;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    int mid = (start + end) / 2;
    std::nth_element(
        m_OrderedTriangles.begin() + start,
        m_OrderedTriangles.begin() + mid,
        m_OrderedTriangles.begin() + end,
        [axis](const Triangle& a, const Triangle& b) {
            glm::vec3 centerA = (a.m_V0.m_Position + a.m_V1.m_Position + a.m_V2.m_Position) / 3.0f;
            glm::vec3 centerB = (b.m_V0.m_Position + b.m_V1.m_Position + b.m_V2.m_Position) / 3.0f;
            return centerA[axis] < centerB[axis];
        }
    );

    int nodeIndex = m_Nodes.size();
    m_Nodes.push_back(node);

    node.m_LeftChild = buildRecursive(start, mid, primitiveBounds, maxTrianglesPerLeaf);
    node.m_RightChild = buildRecursive(mid, end, primitiveBounds, maxTrianglesPerLeaf);

    return nodeIndex;
}