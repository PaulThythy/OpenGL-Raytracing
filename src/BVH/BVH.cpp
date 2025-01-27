#include "BVH.h"

void BVH::build(const std::vector<Triangle>& triangles, const int maxTrianglesPerLeaf) {
    //eventually clear the BVH
    //m_Nodes.clear();
    //m_Nodes.resize(triangles.size() * 2);

    std::vector<int> triangleIndices(triangles.size());
    for (size_t i = 0; i < triangles.size(); i++) {
        triangleIndices[i] = i;
    }

    m_RootNodeIndex = buildRecursive(triangles, triangleIndices, 0, (int)triangleIndices.size(), maxTrianglesPerLeaf);
}

int BVH::buildRecursive(const std::vector<Triangle>& triangles, std::vector<int>& triangleIndices, int start, int end, const int maxTrianglesPerLeaf) {
    int nodeIndex = (int)m_Nodes.size();
    m_Nodes.emplace_back();
    BVHNode& node = m_Nodes[nodeIndex];

    AABB bounds = computeAABB(triangles, triangleIndices, start, end);

    int nPrimitives = end - start;
    if (nPrimitives <= maxTrianglesPerLeaf) {
        // Initialize all indices to -1 first
        for (int i = 0; i < 8; i++) {
            node.m_PrimitiveIndices[i] = -1;
        }
        // Then fill with actual primitive indices
        for (int i = 0; i < nPrimitives; i++) {
            node.m_PrimitiveIndices[i] = triangleIndices[start + i];
        }
        node.m_LeftChild = -1;
        node.m_RightChild = -1;
        return nodeIndex;
    }

    glm::vec3 extent = bounds.m_Max - bounds.m_Min;
    int axis = 0;
    if (extent.y > extent.x && extent.y > extent.z) axis = 1;
    if (extent.z > extent.x && extent.z > extent.y) axis = 2;

    float midValue = 0.5f * (bounds.m_Min[axis] + bounds.m_Max[axis]);

    auto midIter = std::partition(
        triangleIndices.begin() + start,
        triangleIndices.begin() + end,
        [&](int triIdx) {
            glm::vec3 centroid = computeCentroid(triangles[triIdx]);
            return centroid[axis] < midValue;
        }
    );

    int mid = (int)std::distance(triangleIndices.begin(), midIter);

    if (mid == start || mid == end) {
        mid = start + (nPrimitives / 2);
    }

    int leftChildIndex = buildRecursive(triangles, triangleIndices, start, mid, maxTrianglesPerLeaf);
    int rightChildIndex = buildRecursive(triangles, triangleIndices, mid, end, maxTrianglesPerLeaf);

    BVHNode& internalNode = m_Nodes[nodeIndex];
    internalNode.m_Bounds = bounds;
    internalNode.m_LeftChild = leftChildIndex;
    internalNode.m_RightChild = rightChildIndex;

    return nodeIndex;
}

AABB BVH::computeAABB(const std::vector<Triangle>& triangles, 
    const std::vector<int>& triangleIndices, 
    int start, int end) {

    AABB bounds;
    bounds.m_Min = glm::vec3(INFINITY);
    bounds.m_Max = glm::vec3(-INFINITY);

    for (int i = start; i < end; i++) {
        int triIdx = triangleIndices[i];
        const Triangle& tri = triangles[triIdx];
        bounds.m_Min = glm::min(bounds.m_Min, tri.m_V0.m_Position);
        bounds.m_Min = glm::min(bounds.m_Min, tri.m_V1.m_Position);
        bounds.m_Min = glm::min(bounds.m_Min, tri.m_V2.m_Position);
        bounds.m_Max = glm::max(bounds.m_Max, tri.m_V0.m_Position);
        bounds.m_Max = glm::max(bounds.m_Max, tri.m_V1.m_Position);
        bounds.m_Max = glm::max(bounds.m_Max, tri.m_V2.m_Position);
    }
    return bounds;
}

glm::vec3 BVH::computeCentroid(const Triangle& triangle) {
    return (triangle.m_V0.m_Position + triangle.m_V1.m_Position + triangle.m_V2.m_Position) / 3.0f;
}