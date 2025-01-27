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
#include "BVH/BVH.h"

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

    BVH m_BVH;
    GLuint m_BVHNodesSSBO = 0;

    void init(SDL_Window* window, float aspectRatio, GLuint computeProgram);

    void initConstUniforms(GLuint computeProgram);

    void updateConstUniforms(GLuint computeProgram);

    void initSpheresSSBO();
    void updateSpheresSSBO();

    void initTrianglesSSBO();
    void updateTrianglesSSBO();

    void initLightsSSBO();
    void updateLightsSSBO();

    void initMeshesSSBO();
    void updateMeshesSSBO();

    void initBVHSSBO();
    void updateBVHSSBO();

};

#endif