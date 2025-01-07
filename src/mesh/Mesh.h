#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>

#include "math/Triangle.h"
#include "math/Material.h"

struct Mesh {
    inline Mesh() {}

    inline Mesh(const std::string& meshPath, const std::string& materialPath) {
        //TODO
    }

    inline Mesh(const std::string& meshPath, Material material) {
        //TODO
    }

    inline ~Mesh() {}

    std::vector<Triangle> m_Triangles;
    Material m_Material;
};

#endif