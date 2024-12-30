#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Material.h"

struct Triangle {
    inline Triangle() {}

    inline Triangle(
        glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
        Material material
    ): m_V0(v0), m_V1(v1), m_V2(v2), m_Material(material) {}

    inline ~Triangle() {}

    alignas(16) glm::vec3 m_V0;
    alignas(16) glm::vec3 m_V1;
    alignas(16) glm::vec3 m_V2;

    Material m_Material;
};

#endif