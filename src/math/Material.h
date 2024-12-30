#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Material {
    inline Material() {}

    inline Material(
        glm::vec3 albedo,
        glm::vec3 emission,
        float emissionStrength,
        float roughness,
        float metallic
    ): m_Albedo(albedo), m_Emission(emission), 
        m_EmissionStrength(emissionStrength),
        m_Roughness(roughness),
        m_Metallic(metallic) {}
    
    inline ~Material() {}

    alignas(16) glm::vec3 m_Albedo;
    alignas(16) glm::vec3 m_Emission;
    alignas(4) float m_EmissionStrength;
    alignas(4) float m_Roughness;
    alignas(4) float m_Metallic;
    //float m_Ior;
    //float m_Transparency;
};

#endif