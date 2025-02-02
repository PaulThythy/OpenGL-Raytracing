#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>

#include "shader/Shader.h"
#include "core/Scene.h"

struct Renderer {
public:
    Renderer();
    ~Renderer();

    void init(int windowWidth, int windowHeight, SDL_Window* window);
    void cleanup();

    void runComputeShader(int windowWidth, int windowHeight);
    void renderFullscreenQuad();
    void resizeComputeTexture(int windowWidth, int windowHeight);
    void resetAccumulation(int windowWidth, int windowHeight);

    void initComputeShader(int windowWidth, int windowHeight);
    void initRenderShader();

    void initSkybox();

    GLuint m_ComputeProgram     = 0;
    GLuint m_RenderProgram      = 0;

    GLuint m_CurrentTexture     = 0;
    GLuint m_AccumTexture       = 0;

    GLuint m_QuadVAO            = 0;
    GLuint m_QuadVBO            = 0;

    GLuint m_SkyboxTexture      = 0;

    Scene m_Scene;

    int m_FrameCount            = 1;
    bool m_CameraMoved          = false;
};

#endif