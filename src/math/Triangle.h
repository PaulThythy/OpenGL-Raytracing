#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "math/Vector3.h"
#include "Material.h"

struct Triangle {
    inline Triangle() {}

    inline Triangle(
        Vector3 v0, Vector3 v1, Vector3 v2,
        Material material
    ): m_V0(v0), m_V1(v1), m_V2(v2), m_Material(material) {}

    inline ~Triangle() {}

    Vector3 m_V0;
    Vector3 m_V1;
    Vector3 m_V2;

    Material m_Material;
};

#endif