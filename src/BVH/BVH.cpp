#include "BVH.h"

void BVH::build(const std::vector<Triangle>& triangles, const int maxTrianglesPerLeaf) {
    m_Nodes.clear();

    std::vector<int> indices(triangles.size());
        for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;

        int rootIndex = buildRecursive(triangles, indices, maxTrianglesPerLeaf);
}

int BVH::buildRecursive(const std::vector<Triangle>& triangles, std::vector<int>& triangleIndices, const int maxTrianglesPerLeaf) {
    if(triangleIndices.empty()) return -1;

    BVHNode node;
    for(int idx : triangleIndices) node.m_Bounds.expand(triangles[idx].getAABB());

    if (triangleIndices.size() <= maxTrianglesPerLeaf) {
        for (int i = 0; i < maxTrianglesPerLeaf; i++) node.m_PrimitiveIndices[i] = -1;

        for (int i = 0; i < triangleIndices.size() && i < maxTrianglesPerLeaf; i++) {
            node.m_PrimitiveIndices[i] = triangleIndices[i];
        }

        node.m_LeftChild  = -1;
        node.m_RightChild = -1;

        int nodeIndex = m_Nodes.size();
        m_Nodes.push_back(node);

        return nodeIndex;
    }

    std::vector<int> left, right;
    partitionSAH(triangles, triangleIndices, left, right);

    int nodeIndex = m_Nodes.size();
    m_Nodes.push_back(node);

    m_Nodes[nodeIndex].m_LeftChild = buildRecursive(triangles, left, maxTrianglesPerLeaf);
    m_Nodes[nodeIndex].m_RightChild = buildRecursive(triangles, right, maxTrianglesPerLeaf);

    return nodeIndex;
}

void BVH::partitionSAH(const std::vector<Triangle>& triangles, std::vector<int>& triangleIndices, std::vector<int>& left, std::vector<int>& right) {
    size_t mid = triangleIndices.size() / 2;
    std::nth_element(triangleIndices.begin(), triangleIndices.begin() + mid, triangleIndices.end(), [&](int a, int b) {
        return triangles[a].getAABB().m_Min.x < triangles[b].getAABB().m_Min.x;
    });

    left.assign(triangleIndices.begin(), triangleIndices.begin() + mid);
    right.assign(triangleIndices.begin() + mid, triangleIndices.end());
}