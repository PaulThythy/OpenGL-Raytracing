#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    inline Camera() {}
    inline Camera(  glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 up,
                    float fov, float aspectRatio
                ): m_LookFrom(lookFrom), m_LookAt(lookAt), m_Up(up), m_Fov(fov), m_AspectRatio(aspectRatio){}
    
    inline ~Camera() {}

    glm::vec3 m_LookFrom;
    glm::vec3 m_LookAt;
    glm::vec3 m_Up;
    float m_Fov;
    float m_AspectRatio;
};

#endif