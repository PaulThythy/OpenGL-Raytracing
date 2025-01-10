#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_map>

#include "tiny_obj_loader.h"
#include "math/Triangle.h"
#include "math/Material.h"

struct Mesh {
    inline Mesh() {}

    inline Mesh::Mesh(const std::string& meshPath, const std::string& materialPath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str(), materialPath.c_str(), true);

        if (!warn.empty()) {
            std::cout << "WARNING: " << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << "ERROR: " << err << std::endl;
        }

        if (!ret) {
            std::cerr << "Failed to load/parse .obj.\n";
            return;
        }

        std::unordered_map<std::string, int> materialMap;

        for (const auto& shape : shapes) {
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f];
                if (fv != 3) {
                    std::cerr << "Non-triangle face detected. Skipping.\n";
                    index_offset += fv;
                    continue;
                }

                Triangle tri;

                for (size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    glm::vec3 position = glm::vec3(vx, vy, vz);

                    glm::vec3 normal(0.0f, 1.0f, 0.0f); //normal by default
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                        tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                        tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                        normal = glm::vec3(nx, ny, nz);
                    }

                    tri.m_V0 = Vector3(position, normal);
                }

                int matID = shape.mesh.material_ids[f];
                if (matID >= 0 && matID < materials.size()) {
                    const tinyobj::material_t& mat = materials[matID];

                    glm::vec3 albedo(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
                    glm::vec3 emission(mat.emission[0], mat.emission[1], mat.emission[2]);
                    float emissionStrength = mat.emission[0] + mat.emission[1] + mat.emission[2];
                    float roughness = mat.roughness;
                    float metallic = mat.metallic;
                    tri.m_Material = Material(albedo, emission, emissionStrength, roughness, metallic);
                }
                else {
                    tri.m_Material = Material(); //material by default
                }

                m_Triangles.push_back(tri);
                index_offset += fv;
            }
        }
    }

    inline Mesh(const std::string& meshPath, Material material) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials; 
        std::string warn, err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str(), nullptr, false);

        if (!warn.empty()) {
            std::cout << "WARNING: " << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << "ERROR: " << err << std::endl;
        }

        if (!ret) {
            std::cerr << "Failed to load/parse .obj.\n";
            return;
        }


        for (const auto& shape : shapes) {
            size_t index_offset = 0;

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f];
                if (fv != 3) {
                    std::cerr << "Non-triangle face detected. Skipping.\n";
                    index_offset += fv;
                    continue;
                }

                Triangle tri;

                for (size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    glm::vec3 position = glm::vec3(vx, vy, vz);

                    glm::vec3 normal(0.0f, 1.0f, 0.0f); //normal by default
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                        tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                        tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                        normal = glm::vec3(nx, ny, nz);
                    }

                    tri.m_V0 = Vector3(position, normal);
                }

                tri.m_Material = material;

                m_Triangles.push_back(tri);
                index_offset += fv;
            }
        }
    }

    inline ~Mesh() {}

    std::vector<Triangle> m_Triangles;
};

#endif