#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <vector>
#include <iostream>

#include "Camera.h"
#include "globals/Globals.h"
#include "math/Sphere.h"
#include "math/Triangle.h"
#include "math/Light.h"
#include "math/Material.h"

struct Scene {
    inline Scene() {}
    inline ~Scene() {}

    Camera m_Camera;

    std::vector<Sphere> m_Spheres;
    GLuint m_SpheresSSBO = 0;

    std::vector<Triangle> m_Triangles;
    GLuint m_TrianglesSSBO = 0;

    std::vector<Light> m_Lights;
    GLuint m_LightsSSBO = 0;

    inline void init(SDL_Window* window, float aspectRatio, GLuint computeProgram) {
        m_Camera = Camera(
            {0.0, 2.5, 7.0}, 
            {0.0, 0.0, 0.0}, 
            {0.0, 1.0, 0.0}, 
            60.0, 
            aspectRatio,
            0.1f,
            100.0f
            );

        m_Camera.initUniforms(computeProgram);
        m_Camera.updateUniforms();

        initConstUniforms(computeProgram);

        Material redWall(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.5f, 0.0f);
        Material greenWall(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.5f, 0.0f);
        Material whiteWall(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.5f, 0.0f);
        Material emissiveCeiling(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.7f, 0.0f, 0.0f);

        Triangle redWall1(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(-5.0f, 5.0f, 5.0f), glm::vec3(-5.0f, 0.0f, -5.0f), redWall); 
        Triangle redWall2(glm::vec3(-5.0f, 5.0f, 5.0f), glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(-5.0f, 0.0f, -5.0f), redWall);
        m_Triangles.push_back(redWall1); m_Triangles.push_back(redWall2);

        Triangle greenWall1(glm::vec3(5.0f, 5.0f, -5.0f), glm::vec3(5.0f, 0.0f, -5.0f), glm::vec3(5.0f, 5.0f, 5.0f), greenWall); 
        Triangle greenWall2(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f, 0.0f, -5.0f), glm::vec3(5.0f, 0.0f, 5.0f), greenWall);
        m_Triangles.push_back(greenWall1); m_Triangles.push_back(greenWall2);

        Triangle backWall1(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(5.0f, 5.0f, -5.0f), whiteWall);
        Triangle backWall2(glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(5.0f, 0.0f, -5.0f), glm::vec3(5.0f, 5.0f, -5.0f), whiteWall);
        m_Triangles.push_back(backWall1); m_Triangles.push_back(backWall2);

        Triangle ground1(glm::vec3(5.0f, 0.0f, -5.0f), glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(-5.0f, 0.0f, 5.0f), whiteWall);
        Triangle ground2(glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(5.0f, 0.0f, -5.0f), whiteWall);
        m_Triangles.push_back(ground1); m_Triangles.push_back(ground2);

        Triangle ceiling1(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(-5.0f, 5.0f, 5.0f), whiteWall);
        Triangle ceiling2(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(5.0f, 5.0f, -5.0f), glm::vec3(5.0f, 5.0f, 5.0f), whiteWall);
        m_Triangles.push_back(ceiling1); m_Triangles.push_back(ceiling2);

        Triangle emissiveCeiling1(glm::vec3(-5.0f/2.0f, 4.9f, -5.0f/2.0f), glm::vec3(5.0f/2.0f, 4.9f, 5.0f/2.0f), glm::vec3(-5.0f/2.0f, 4.9f, 5.0f/2.0f), emissiveCeiling);
        Triangle emissiveCeiling2(glm::vec3(-5.0f/2.0f, 4.9f, -5.0f/2.0f), glm::vec3(5.0f/2.0f, 4.9f, -5.0f/2.0f), glm::vec3(5.0f/2.0f, 4.9f, 5.0f/2.0f), emissiveCeiling);
        m_Triangles.push_back(emissiveCeiling1); m_Triangles.push_back(emissiveCeiling2);

        initTrianglesSSBO();        

        //m_Lights.push_back(Light(glm::vec3(1.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 10.0));
        //initLightsSSBO();
    }

    inline void initConstUniforms(GLuint computeProgram) {
        glUseProgram(computeProgram);

        GLint samplesLoc = glGetUniformLocation(computeProgram, "SAMPLES");
        GLint bouncesLoc = glGetUniformLocation(computeProgram, "BOUNCES");

        if (samplesLoc == -1) {
            std::cerr << "Warning: 'SAMPLES' uniform not found.\n";
        } else {
            glUniform1i(samplesLoc, Config::SAMPLES);
        }

        if (bouncesLoc == -1) {
            std::cerr << "Warning: 'BOUNCES' uniform not found.\n";
        } else {
            glUniform1i(bouncesLoc, Config::BOUNCES);
        }

        glUseProgram(0);
    }

    inline void updateConstUniforms(GLuint computeProgram) {
        glUseProgram(computeProgram);

        GLint samplesLoc = glGetUniformLocation(computeProgram, "SAMPLES");
        GLint bouncesLoc = glGetUniformLocation(computeProgram, "BOUNCES");

        if (samplesLoc != -1) {
            glUniform1i(samplesLoc, Config::SAMPLES);
        }

        if (bouncesLoc != -1) {
            glUniform1i(bouncesLoc, Config::BOUNCES);
        }

        glUseProgram(0);
    }

    inline void initSpheresSSBO() {
        glGenBuffers(1, &m_SpheresSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpheresSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Spheres.size() * sizeof(Sphere), m_Spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SpheresSSBO);       //binding 2
    }

    inline void updateSpheresSSBO() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpheresSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Spheres.size() * sizeof(Sphere), m_Spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SpheresSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    inline void initTrianglesSSBO() {
        glGenBuffers(1, &m_TrianglesSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TrianglesSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Triangles.size() * sizeof(Triangle), m_Triangles.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TrianglesSSBO);       //binding 3
    }

    inline void updateTrianglesSSBO() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TrianglesSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Triangles.size() * sizeof(Triangle), m_Triangles.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TrianglesSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    inline void initLightsSSBO() {
        glGenBuffers(1, &m_LightsSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Lights.size() * sizeof(Light), m_Lights.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_LightsSSBO);        //binding 4
    }

    inline void updateLightsSSBO() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Lights.size() * sizeof(Light), m_Lights.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_LightsSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
};

#endif