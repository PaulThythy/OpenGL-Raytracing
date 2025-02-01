#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "math/Triangle.h"
#include "math/Material.h"

//#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

struct Mesh {
    inline Mesh() {}

    /*inline Mesh(const std::string& meshPath, const std::string& materialPath) {
        //TODO
    }*/

    inline Mesh(const std::string& filename, const Material& material, std::vector<Triangle>& outTriangles) {
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig reader_config;

        if (!reader.ParseFromFile(filename, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            return;
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();

        // loop over all shapes
        for (const auto& shape : shapes) {
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                Vector3 vertices[3];
                
                // get the 3 vertices of the triangle
                for (size_t v = 0; v < 3; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[3*f + v];
                    
                    float vx = attrib.vertices[3*idx.vertex_index+0];
                    float vy = attrib.vertices[3*idx.vertex_index+1];
                    float vz = attrib.vertices[3*idx.vertex_index+2];
                    
                    glm::vec3 normal;
                    if (idx.normal_index >= 0) {
                        normal = glm::vec3(
                            attrib.normals[3*idx.normal_index+0],
                            attrib.normals[3*idx.normal_index+1],
                            attrib.normals[3*idx.normal_index+2]
                        );
                    } else {
                        normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    vertices[v] = Vector3(
                        glm::vec3(vx, vy, vz),  
                        normal
                    );
                }

                Triangle tri(vertices[0], vertices[1], vertices[2], material);
                std::cout 
                    << "coordinates : \n" << 
                        "(" << tri.m_V0.m_Position.x << ", " << tri.m_V0.m_Position.y << ", " << tri.m_V0.m_Position.z << "), \n" <<
                        "(" << tri.m_V1.m_Position.x << ", " << tri.m_V1.m_Position.y << ", " << tri.m_V1.m_Position.z << "), \n" <<
                        "(" << tri.m_V2.m_Position.x << ", " << tri.m_V2.m_Position.y << ", " << tri.m_V2.m_Position.z << "), \n"
                    << "normals : \n" <<
                        "(" << tri.m_V0.m_Normal.x << ", " << tri.m_V0.m_Normal.y << ", " << tri.m_V0.m_Normal.z << "), \n" <<
                        "(" << tri.m_V1.m_Normal.x << ", " << tri.m_V1.m_Normal.y << ", " << tri.m_V1.m_Normal.z << "), \n" <<
                        "(" << tri.m_V2.m_Normal.x << ", " << tri.m_V2.m_Normal.y << ", " << tri.m_V2.m_Normal.z << "), \n"
                    << std::endl;

                outTriangles.push_back(tri);
            }
        }

        std::cout << "Loaded mesh: " << filename << std::endl;
    }

    inline ~Mesh() {}
};

#endif