#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "math/Vector3.h"
#include "math/AABB.h"
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

    inline AABB getAABB() const {
        AABB aabb;
        aabb.m_Min = glm::min(glm::min(m_V0.m_Position, m_V1.m_Position), m_V2.m_Position);
        aabb.m_Max = glm::max(glm::max(m_V0.m_Position, m_V1.m_Position), m_V2.m_Position);

        aabb.m_Min -= glm::vec3(0.001f);
        aabb.m_Max += glm::vec3(0.001f);

        return aabb;
    }
};

#endif