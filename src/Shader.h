#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

namespace Shader {

    inline GLuint loadComputeShader(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open compute shader file: " << filePath << std::endl;
            return 0;
        }

        std::stringstream shaderStream;
        shaderStream << file.rdbuf();
        file.close();

        std::string shaderCode = shaderStream.str();
        const char* shaderSource = shaderCode.c_str();

        GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shaderID, 1, &shaderSource, nullptr);
        glCompileShader(shaderID);

        GLint compileStatus;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == GL_FALSE) {
            GLint logLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(logLength, '\0');
            glGetShaderInfoLog(shaderID, logLength, nullptr, &log[0]);
            std::cerr << "Compute Shader compilation error: " << log << std::endl;

            glDeleteShader(shaderID);
            return 0;
        }

        GLuint programID = glCreateProgram();
        glAttachShader(programID, shaderID);
        glLinkProgram(programID);

        GLint linkStatus;
        glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE) {
            GLint logLength;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(logLength, '\0');
            glGetProgramInfoLog(programID, logLength, nullptr, &log[0]);
            std::cerr << "Program linking error: " << log << std::endl;

            glDeleteProgram(programID);
            return 0;
        }

        glDeleteShader(shaderID); // Shader can be deleted after linking
        return programID;
    }
}


#endif