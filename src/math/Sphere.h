#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Material.h"

struct Sphere {
    inline Sphere() {}

    inline Sphere(
        glm::vec3 center, 
        float radius,
        Material material
    ): m_Center(center), m_Radius(radius), m_Material(material) {}
    
    inline ~Sphere() {}

    alignas(16) glm::vec3 m_Center;
    alignas(4) float m_Radius;
    Material m_Material;

};

#endif