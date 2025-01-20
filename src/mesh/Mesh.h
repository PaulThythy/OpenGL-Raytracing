#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "math/Triangle.h"
#include "math/Material.h"

struct Mesh {
    inline Mesh() {}

    /*inline Mesh(const std::string& meshPath, const std::string& materialPath) {
        //TODO
    }*/

    inline Mesh(const std::string& meshPath, Material material, std::vector<Triangle>& sceneTriangles) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> vertexIndices;
        std::vector<unsigned int> normalIndices;

        m_Material = material;
        m_FirstTriangle = sceneTriangles.size();
        m_TriangleCount = 0;

        std::ifstream file(meshPath);
        if (!file.is_open()) {
            std::cerr << "ERROR::MESH::Could not open file: " << meshPath << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {                                          // Vertex
                float x, y, z;
                iss >> x >> y >> z;
                vertices.push_back(glm::vec3(x, y, z));
            }
            else if (type == "vn") {                                    // Normal
                float x, y, z;
                iss >> x >> y >> z;
                normals.push_back(glm::normalize(glm::vec3(x, y, z)));
            }
            else if (type == "f") {                                     // Face
                std::string vertex1, vertex2, vertex3;
                iss >> vertex1 >> vertex2 >> vertex3;
                
                // Parse vertex indices
                auto parseVertex = [](const std::string& vertex) -> std::pair<int, int> {
                    std::istringstream viss(vertex);
                    std::string indexStr;
                    std::vector<std::string> indices;
                    
                    while (std::getline(viss, indexStr, '/')) {
                        indices.push_back(indexStr);
                    }
                    
                    int vertexIndex = indices[0].empty() ? 0 : std::stoi(indices[0]);
                    int normalIndex = (indices.size() < 3 || indices[2].empty()) ? 0 : std::stoi(indices[2]);
                    
                    // OBJ indices start at 1
                    return {vertexIndex - 1, normalIndex - 1};
                };

                std::pair<int, int> vertexPair1 = parseVertex(vertex1);
                int v1 = vertexPair1.first;
                int n1 = vertexPair1.second;

                std::pair<int, int> vertexPair2 = parseVertex(vertex2);
                int v2 = vertexPair2.first;
                int n2 = vertexPair2.second;

                std::pair<int, int> vertexPair3 = parseVertex(vertex3);
                int v3 = vertexPair3.first;
                int n3 = vertexPair3.second;

                vertexIndices.push_back(v1);
                vertexIndices.push_back(v2);
                vertexIndices.push_back(v3);

                normalIndices.push_back(n1);
                normalIndices.push_back(n2);
                normalIndices.push_back(n3);
            }
        }
        file.close();

        for (size_t i = 0; i < vertexIndices.size(); i += 3) {
            Vector3 v0, v1, v2;

            // First vertex
            v0.m_Position = vertices[vertexIndices[i]];
            v0.m_Normal = !normals.empty() ? normals[normalIndices[i]] : glm::vec3(0, 1, 0);

            // Second vertex
            v1.m_Position = vertices[vertexIndices[i + 1]];
            v1.m_Normal = !normals.empty() ? normals[normalIndices[i + 1]] : glm::vec3(0, 1, 0);

            // Third vertex
            v2.m_Position = vertices[vertexIndices[i + 2]];
            v2.m_Normal = !normals.empty() ? normals[normalIndices[i + 2]] : glm::vec3(0, 1, 0);

            // If no normals in file, calculate a normal per face
            if (normals.empty()) {
                glm::vec3 edge1 = v1.m_Position - v0.m_Position;
                glm::vec3 edge2 = v2.m_Position - v0.m_Position;
                glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
                v0.m_Normal = v1.m_Normal = v2.m_Normal = normal;
            }

            sceneTriangles.push_back(Triangle(v0, v1, v2, m_Material));
            m_TriangleCount++;
        }

        std::cout << "Loaded mesh: " << meshPath << std::endl;
        std::cout << "Vertices: " << vertices.size() << std::endl;
        std::cout << "Triangles: " << m_TriangleCount << std::endl;
    }

    inline ~Mesh() {}

    int m_FirstTriangle;
    int m_TriangleCount;
    Material m_Material;
};

#endif