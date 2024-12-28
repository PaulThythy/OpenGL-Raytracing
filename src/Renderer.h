#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>

#include "Shader.h"

struct Renderer {
public:
    Renderer();
    ~Renderer();

    void init(int windowWidth, int windowHeight);
    void cleanup();

    void runComputeShader(int windowWidth, int windowHeight);
    void renderFullscreenQuad();

private:
    void initComputeShader(int windowWidth, int windowHeight);
    void initRenderShader();

    GLuint m_ComputeProgram     = 0;
    GLuint m_RenderProgram      = 0;

    GLuint m_ComputeTexture     = 0;

    GLuint m_QuadVAO            = 0;
    GLuint m_QuadVBO            = 0;
};

#endif