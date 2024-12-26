#ifndef APPLICATION_H
#define APPLICATION_H

#include <GL/glew.h>
#include <SDL3/SDL.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <stdio.h> 
#include <stdexcept>
#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "Shader.h"

class Application {

public:
    Application();
    ~Application();

    void run();

private:
    void setupSDL();
    void initSDL();

    void setupImGui();

    void processEvents();

    void cleanup();

    void initComputeShader();
    void runComputeShader();

    SDL_Window*   m_Window      = nullptr;
    SDL_GLContext m_GLContext   = nullptr;
    bool          m_Done        = false;

    bool m_ShowDemoWindow       = true;
    bool m_ShowAnotherWindow    = false;

    float m_SliderFloat         = 0.0f;
    int   m_Counter             = 0;

    GLuint m_ComputeProgram     = 0;
    GLuint m_ComputeTexture     = 0;
};

#endif