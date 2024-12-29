#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "Camera.h"

struct Scene {
    inline Scene() {}
    inline ~Scene() {}

    Camera m_Camera;

    GLint m_LocCamLookFrom = -1;
    GLint m_LocCamLookAt   = -1;
    GLint m_LocCamUp       = -1;
    GLint m_LocCamFov      = -1;
    GLint m_LocCamAspect   = -1;

    GLuint m_ComputeProgram = 0; 

    inline void init(SDL_Window* window, float aspectRatio, GLuint computeProgram) {
        m_ComputeProgram = computeProgram;

        m_Camera = Camera({4.0, 4.0, 4.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, 60.0, aspectRatio);

        initCameraUniforms(m_ComputeProgram);

        updateCameraUniforms();
    }

    inline void initCameraUniforms(GLuint programID) {
        m_ComputeProgram = programID;

        glUseProgram(m_ComputeProgram);

        m_LocCamLookFrom = glGetUniformLocation(m_ComputeProgram, "camera.lookFrom");
        m_LocCamLookAt   = glGetUniformLocation(m_ComputeProgram, "camera.lookAt");
        m_LocCamUp       = glGetUniformLocation(m_ComputeProgram, "camera.up");
        m_LocCamFov      = glGetUniformLocation(m_ComputeProgram, "camera.fov");
        m_LocCamAspect   = glGetUniformLocation(m_ComputeProgram, "camera.aspectRatio");

        glUseProgram(0);
    }

    inline void updateCameraUniforms() const {
        glUseProgram(m_ComputeProgram);

        if(m_LocCamLookFrom >= 0)
            glUniform3fv(m_LocCamLookFrom, 1, glm::value_ptr(m_Camera.m_LookFrom));
    
        if(m_LocCamLookAt >= 0)
            glUniform3fv(m_LocCamLookAt, 1, glm::value_ptr(m_Camera.m_LookAt));

        if(m_LocCamUp >= 0)
            glUniform3fv(m_LocCamUp, 1, glm::value_ptr(m_Camera.m_Up));

        if(m_LocCamFov >= 0)
            glUniform1f(m_LocCamFov, m_Camera.m_Fov);

        if(m_LocCamAspect >= 0)
            glUniform1f(m_LocCamAspect, m_Camera.m_AspectRatio);
    }
};

#endif