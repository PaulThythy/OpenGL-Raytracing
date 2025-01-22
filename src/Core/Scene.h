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
#include "math/Vector3.h"
#include "mesh/Mesh.h"

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

    std::vector<Mesh> m_Meshes;
    GLuint m_MeshesSSBO = 0;

    inline void init(SDL_Window* window, float aspectRatio, GLuint computeProgram) {
        m_Camera = Camera(
            {0.0, 2.5, 8.0}, 
            {0.0, 2.0, 0.0}, 
            {0.0, 1.0, 0.0}, 
            60.0, 
            aspectRatio,
            0.1f,
            100.0f
            );

        m_Camera.initUniforms(computeProgram);
        m_Camera.updateUniforms();

        initConstUniforms(computeProgram);

        Material redWall({1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.5f, 0.0f);
        Material greenWall({0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.5f, 0.0f);
        Material whiteWall({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.5f, 0.0f);
        Material emissiveCeiling({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0.0f);

        /*Triangle redWall1(
            Vector3({-5.0f, 5.0f, -5.0f}, {1.0f, 0.0f, 0.0f}), 
            Vector3({-5.0f, 5.0f, 5.0f}, {1.0f, 0.0f, 0.0f}), 
            Vector3({-5.0f, 0.0f, -5.0f}, {1.0f, 0.0f, 0.0f}), 
            redWall
        ); 
        Triangle redWall2(
            Vector3({-5.0f, 5.0f, 5.0f}, {1.0f, 0.0f, 0.0f}), 
            Vector3({-5.0f, 0.0f, 5.0f}, {1.0f, 0.0f, 0.0f}), 
            Vector3({-5.0f, 0.0f, -5.0f}, {1.0f, 0.0f, 0.0f}), 
            redWall
        );
        m_Triangles.push_back(redWall1); m_Triangles.push_back(redWall2);

        Triangle greenWall1(
            Vector3({5.0f, 5.0f, -5.0f}, {-1.0f, 0.0f, 0.0f}), 
            Vector3({5.0f, 0.0f, -5.0f}, {-1.0f, 0.0f, 0.0f}), 
            Vector3({5.0f, 5.0f, 5.0f}, {-1.0f, 0.0f, 0.0f}), 
            greenWall
        ); 
        Triangle greenWall2(
            Vector3({5.0f, 5.0f, 5.0f}, {-1.0f, 0.0f, 0.0f}), 
            Vector3({5.0f, 0.0f, -5.0f}, {-1.0f, 0.0f, 0.0f}), 
            Vector3({5.0f, 0.0f, 5.0f}, {-1.0f, 0.0f, 0.0f}), 
            greenWall
        );
        m_Triangles.push_back(greenWall1); m_Triangles.push_back(greenWall2);

        Triangle backWall1(
            Vector3({-5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f}), 
            Vector3({-5.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f}), 
            Vector3({5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f}), 
            whiteWall
        );
        Triangle backWall2(
            Vector3({-5.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f}), 
            Vector3({5.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f}), 
            Vector3({5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f}), 
            whiteWall
        );
        m_Triangles.push_back(backWall1); m_Triangles.push_back(backWall2);

        Triangle ground1(
            Vector3({5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}), 
            Vector3({-5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}), 
            Vector3({-5.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}), 
            whiteWall
        );
        Triangle ground2(
            Vector3({-5.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}), 
            Vector3({5.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}), 
            Vector3({5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}), 
            whiteWall
        );
        m_Triangles.push_back(ground1); m_Triangles.push_back(ground2);

        Triangle ceiling1(
            Vector3({-5.0f, 5.0f, -5.0f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({5.0f, 5.0f, 5.0f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({-5.0f, 5.0f, 5.0f}, {0.0f, -1.0f, 0.0f}),
            whiteWall
        );
        Triangle ceiling2(
            Vector3({-5.0f, 5.0f, -5.0f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({5.0f, 5.0f, -5.0f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({5.0f, 5.0f, 5.0f}, {0.0f, -1.0f, 0.0f}), 
            whiteWall
        );
        m_Triangles.push_back(ceiling1); m_Triangles.push_back(ceiling2);

        Triangle emissiveCeiling1(
            Vector3({-5.0f/1.5f, 4.9f, -5.0f/1.5f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({5.0f/1.5f, 4.9f, 5.0f/1.5f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({-5.0f/1.5f, 4.9f, 5.0f/1.5f}, {0.0f, -1.0f, 0.0f}), 
            emissiveCeiling
        );
        Triangle emissiveCeiling2(
            Vector3({-5.0f/1.5f, 4.9f, -5.0f/1.5f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({5.0f/1.5f, 4.9f, -5.0f/1.5f}, {0.0f, -1.0f, 0.0f}), 
            Vector3({5.0f/1.5f, 4.9f, 5.0f/1.5f}, {0.0f, -1.0f, 0.0f}), 
            emissiveCeiling
        );
        m_Triangles.push_back(emissiveCeiling1); m_Triangles.push_back(emissiveCeiling2);

        initTrianglesSSBO();        

        Material gold({1.0f, 0.9f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.1f, 1.0f);
        Material silver({0.7, 0.7, 0.7}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.1f, 1.0f);
        Material blue({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 1.0f, 0.0f);

        Sphere sphereGold({-2.5f, 2.5f, -2.5f}, 2.5f, gold);
        Sphere sphereSilver({1.0f, 1.2f, -1.0f}, 1.2f, silver);
        Sphere sphereBlue({-3.0f, 0.4f, 3.0f}, 0.4f, blue);
        m_Spheres.push_back(sphereGold); m_Spheres.push_back(sphereSilver); m_Spheres.push_back(sphereBlue);

        initSpheresSSBO();*/

        /*Light light1(glm::vec3(0.0f, 4.9f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
        Light light2(glm::vec3(2.0f, 4.9f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
        Light light3(glm::vec3(-2.0f, 4.9f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
        m_Lights.push_back(light1); m_Lights.push_back(light2); m_Lights.push_back(light3);
        initLightsSSBO();*/

        Mesh suzanne(std::string(MESH_DIR) + "/suzanne.obj", redWall, m_Triangles);
        m_Meshes.push_back(suzanne);

        initMeshesSSBO();
        initTrianglesSSBO();

        Light light1(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
        m_Lights.push_back(light1);
        initLightsSSBO();
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

    inline void initMeshesSSBO() {
        glGenBuffers(1, &m_MeshesSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MeshesSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Meshes.size() * sizeof(Mesh), m_Meshes.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_MeshesSSBO);        //binding 5
    }

    inline void updateMeshesSSBO() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MeshesSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_Meshes.size() * sizeof(Mesh), m_Meshes.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_MeshesSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
};

#endif