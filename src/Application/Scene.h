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

    inline void init(SDL_Window* window, float aspectRatio, GLuint computeProgram) {
        m_Camera = Camera(
            {4.0, 4.0, 4.0}, 
            {0.0, 0.0, 0.0}, 
            {0.0, 1.0, 0.0}, 
            60.0, 
            aspectRatio);

        m_Camera.initUniforms(computeProgram);
        m_Camera.updateUniforms();
    }
};

#endif