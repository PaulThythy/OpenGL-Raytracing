#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <GL/glew.h>

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

    GLint m_LocCamLookFrom = -1;
    GLint m_LocCamLookAt   = -1;
    GLint m_LocCamUp       = -1;
    GLint m_LocCamFov      = -1;
    GLint m_LocCamAspect   = -1;

    GLuint m_ComputeProgram = 0; 

    inline void initUniforms(GLint computeProgram) {
        m_ComputeProgram = computeProgram;

        glUseProgram(m_ComputeProgram);

        m_LocCamLookFrom = glGetUniformLocation(m_ComputeProgram, "camera.lookFrom");
        m_LocCamLookAt   = glGetUniformLocation(m_ComputeProgram, "camera.lookAt");
        m_LocCamUp       = glGetUniformLocation(m_ComputeProgram, "camera.up");
        m_LocCamFov      = glGetUniformLocation(m_ComputeProgram, "camera.fov");
        m_LocCamAspect   = glGetUniformLocation(m_ComputeProgram, "camera.aspectRatio");

        glUseProgram(0);
    }

    inline void updateUniforms() {
        glUseProgram(m_ComputeProgram);

        if(m_LocCamLookFrom >= 0)
            glUniform3fv(m_LocCamLookFrom, 1, glm::value_ptr(m_LookFrom));
    
        if(m_LocCamLookAt >= 0)
            glUniform3fv(m_LocCamLookAt, 1, glm::value_ptr(m_LookAt));

        if(m_LocCamUp >= 0)
            glUniform3fv(m_LocCamUp, 1, glm::value_ptr(m_Up));

        if(m_LocCamFov >= 0)
            glUniform1f(m_LocCamFov, m_Fov);

        if(m_LocCamAspect >= 0)
            glUniform1f(m_LocCamAspect, m_AspectRatio);
    }
};

#endif