#include "core/Scene.h"

void Scene::init(SDL_Window* window, float aspectRatio, GLuint computeProgram) {
    m_Camera = Camera(
        { 0.0, 2.5, 8.0 },
        { 0.0, 2.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        60.0,
        aspectRatio,
        0.1f,
        100.0f
    );

    m_Camera.initUniforms(computeProgram);
    m_Camera.updateUniforms();

    initConstUniforms(computeProgram);

    Material redWall({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.5f, 0.0f);
    Material greenWall({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.5f, 0.0f);
    Material whiteWall({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.5f, 0.0f);
    Material emissiveCeiling({ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f, 0.0f, 0.0f);

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

    initSpheresSSBO();

    Light light1(glm::vec3(0.0f, 4.9f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
    Light light2(glm::vec3(2.0f, 4.9f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
    Light light3(glm::vec3(-2.0f, 4.9f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
    m_Lights.push_back(light1); m_Lights.push_back(light2); m_Lights.push_back(light3);
    initLightsSSBO();

    m_BVH.build(m_Triangles);
    initBVHSSBO();*/

    // Add ground plane
    Vector3 v0(glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Vector3 v1(glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Vector3 v2(glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Vector3 v3(glm::vec3(5.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    m_Triangles.push_back(Triangle(v0, v1, v2, whiteWall));
    m_Triangles.push_back(Triangle(v0, v2, v3, whiteWall));

    // Load icosphere mesh
    Mesh cube(std::string(MESH_DIR) + "/cube.obj", redWall, m_Triangles);

    // Initialize SSBOs in the correct order
    initTrianglesSSBO();    // First upload triangles

    // Build and upload BVH
    m_BVH.build(m_Triangles, Config::BVH_MAX_TRIANGLES_PER_LEAF);
    initBVHSSBO();

    // Add a light
    Light light1(glm::vec3(0.0f, 7.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 10.0);
    m_Lights.push_back(light1);
    initLightsSSBO();

    std::cout << "Scene initialized" << std::endl;
}

void Scene::initConstUniforms(GLuint computeProgram) {
    glUseProgram(computeProgram);

    GLint samplesLoc = glGetUniformLocation(computeProgram, "SAMPLES");
    GLint bouncesLoc = glGetUniformLocation(computeProgram, "BOUNCES");

    if (samplesLoc == -1) {
        std::cerr << "Warning: 'SAMPLES' uniform not found.\n";
    }
    else {
        glUniform1i(samplesLoc, Config::SAMPLES);
    }

    if (bouncesLoc == -1) {
        std::cerr << "Warning: 'BOUNCES' uniform not found.\n";
    }
    else {
        glUniform1i(bouncesLoc, Config::BOUNCES);
    }

    glUseProgram(0);
}

void Scene::updateConstUniforms(GLuint computeProgram) {
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

void Scene::initSpheresSSBO() {
    glGenBuffers(1, &m_SpheresSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpheresSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Spheres.size() * sizeof(Sphere), m_Spheres.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SpheresSSBO);       //binding 2
}

void Scene::updateSpheresSSBO() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SpheresSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Spheres.size() * sizeof(Sphere), m_Spheres.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SpheresSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::initTrianglesSSBO() {
    glGenBuffers(1, &m_TrianglesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TrianglesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Triangles.size() * sizeof(Triangle), m_Triangles.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TrianglesSSBO);       //binding 3
}

void Scene::updateTrianglesSSBO() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TrianglesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Triangles.size() * sizeof(Triangle), m_Triangles.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TrianglesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::initLightsSSBO() {
    glGenBuffers(1, &m_LightsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Lights.size() * sizeof(Light), m_Lights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_LightsSSBO);        //binding 4
}

void Scene::updateLightsSSBO() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Lights.size() * sizeof(Light), m_Lights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_LightsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::initBVHSSBO() {
    // Create and bind the SSBO for BVH nodes
    glGenBuffers(1, &m_BVHNodesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVHNodesSSBO);
    
    // Calculate total size needed for the buffer
    size_t totalSize = m_BVH.m_Nodes.size() * sizeof(BVHNode);
    
    // Allocate and initialize the buffer
    glBufferData(GL_SHADER_STORAGE_BUFFER, totalSize, m_BVH.m_Nodes.data(), GL_STATIC_DRAW);
    
    // Bind the buffer to the shader binding point
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_BVHNodesSSBO);  // binding 6
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::updateBVHSSBO() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVHNodesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_BVH.m_Nodes.size() * sizeof(BVHNode), m_BVH.m_Nodes.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_BVHNodesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}