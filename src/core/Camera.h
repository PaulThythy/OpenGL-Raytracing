#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <GL/glew.h>

struct Camera {
    inline Camera() {}
    inline Camera(  glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 up,
                    float fov, float aspectRatio,
                    float nearPlane, float farPlane
                ): m_LookFrom(lookFrom), m_LookAt(lookAt), m_Up(up), m_Fov(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
    {
        m_Front = glm::normalize(m_LookAt - m_LookFrom);
        m_Right = glm::normalize(glm::cross(m_Front, m_Up));
        //to ensure that it is orthogonal to Front and Right
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
    
    inline ~Camera() {}

    glm::vec3 m_LookFrom;
    glm::vec3 m_LookAt;
    glm::vec3 m_Up;
    float m_Fov;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;
    glm::vec3 m_Front;
    glm::vec3 m_Right;

    GLuint m_LocCamLookFrom  = -1;
    GLuint m_LocCamLookAt    = -1;
    GLuint m_LocCamUp        = -1;
    GLuint m_LocCamFov       = -1;
    GLuint m_LocCamAspect    = -1;
    GLuint m_LocCamNearPlane = -1;
    GLuint m_LocCamFarPlane  = -1;
    GLuint m_LocCamFront     = -1;
    GLuint m_LocCamRight     = -1;

    GLuint m_ComputeProgram = 0; 

    inline void initUniforms(GLuint computeProgram) {
        m_ComputeProgram = computeProgram;

        glUseProgram(m_ComputeProgram);

        m_LocCamLookFrom    = glGetUniformLocation(m_ComputeProgram, "camera.lookFrom");
        m_LocCamLookAt      = glGetUniformLocation(m_ComputeProgram, "camera.lookAt");
        m_LocCamUp          = glGetUniformLocation(m_ComputeProgram, "camera.up");
        m_LocCamFov         = glGetUniformLocation(m_ComputeProgram, "camera.fov");
        m_LocCamAspect      = glGetUniformLocation(m_ComputeProgram, "camera.aspectRatio");
        m_LocCamNearPlane   = glGetUniformLocation(m_ComputeProgram, "camera.nearPlane");
        m_LocCamFarPlane    = glGetUniformLocation(m_ComputeProgram, "camera.farPlane");
        m_LocCamFront       = glGetUniformLocation(m_ComputeProgram, "camera.front");
        m_LocCamRight       = glGetUniformLocation(m_ComputeProgram, "camera.right");

        if(m_LocCamLookFrom == -1)  std::cerr << "Warning: Camera lookFrom uniform location not found.\n";
        if(m_LocCamLookAt == -1)    std::cerr << "Warning: Camera lookAt uniform location not found.\n";
        if(m_LocCamUp == -1)        std::cerr << "Warning: Camera up uniform location not found.\n";
        if(m_LocCamFov == -1)       std::cerr << "Warning: Camera fov uniform location not found.\n";
        if(m_LocCamAspect == -1)    std::cerr << "Warning: Camera aspectRatio uniform location not found.\n";
        if(m_LocCamNearPlane == -1) std::cerr << "Warning: Camera nearPlane uniform location not found.\n";
        if(m_LocCamFarPlane == -1)  std::cerr << "Warning: Camera farPlane uniform location not found.\n";
        if(m_LocCamFront == -1)     std::cerr << "Warning: Camera front uniform location not found.\n";
        if(m_LocCamRight == -1)     std::cerr << "Warning: Camera right uniform location not found.\n";

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
        
        if(m_LocCamNearPlane >= 0)
            glUniform1f(m_LocCamNearPlane, m_NearPlane);

        if(m_LocCamFarPlane >= 0)
            glUniform1f(m_LocCamFarPlane, m_FarPlane);
        
        if(m_LocCamFront >= 0)
            glUniform3fv(m_LocCamFront, 1, glm::value_ptr(m_Front));
            
        if(m_LocCamRight >= 0)
            glUniform3fv(m_LocCamRight, 1, glm::value_ptr(m_Right));
    }

    inline void rotateAroundUp(float angleDegrees) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), m_Up);
        m_Front = glm::normalize(glm::vec3(rotation * glm::vec4(m_Front, 1.0f)));
        m_Right = glm::normalize(glm::cross(m_Front, m_Up));
        m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
        m_LookAt = m_LookFrom + m_Front;
        updateUniforms();
    }

    inline void rotateAroundRight(float angleDegrees) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), m_Right);
        m_Front = glm::normalize(glm::vec3(rotation * glm::vec4(m_Front, 1.0f)));
        m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
        m_LookAt= m_LookFrom + m_Front;
        updateUniforms();
    }

    inline void moveForward(float delta) {
        m_LookFrom += delta * m_Front;
        m_LookAt   += delta * m_Front;
        m_Front = glm::normalize(m_LookAt - m_LookFrom);
        m_Right = glm::normalize(glm::cross(m_Front, m_Up));
        m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
        updateUniforms();
    }
};

#endif