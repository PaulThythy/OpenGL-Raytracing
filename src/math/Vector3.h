#ifndef VECTOR3_H
#define VECTOR3_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vector3 {
    inline Vector3() {}

    inline Vector3(
        glm::vec3 position,
        glm::vec3 normal
    ): m_Position(position), m_Normal(normal) {}

    inline ~Vector3() {}

    alignas(16) glm::vec3 m_Position;
    alignas(16) glm::vec3 m_Normal;
};

#endif