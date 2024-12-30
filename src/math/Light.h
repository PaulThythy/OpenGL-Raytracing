#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light {
    inline Light() {}

    inline Light(glm::vec3 position, glm::vec3 color, float power): m_Position(position), m_Color(color), m_Power(power) {}

    inline ~Light() {}

    alignas(16) glm::vec3 m_Position;
    alignas(16) glm::vec3 m_Color;
    alignas(4) float m_Power;
};

#endif