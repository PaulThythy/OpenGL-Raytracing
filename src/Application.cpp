#include "Application.h"

Application::Application() {
    setupSDL();
    initSDL();
    setupImGui();
}

Application::~Application()
{
    cleanup();
}

void Application::setupSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        throw std::runtime_error(std::string("SDL_Init: ") + SDL_GetError());
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    // other flags
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void Application::initSDL() {
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    m_Window = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example",
                                1280, 720,
                                windowFlags);
    if (!m_Window)
    {
        throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());
    }

    SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    m_GLContext = SDL_GL_CreateContext(m_Window);
    if (!m_GLContext)
    {
        throw std::runtime_error(std::string("SDL_GL_CreateContext: ") + SDL_GetError());
    }

    SDL_GL_MakeCurrent(m_Window, m_GLContext);

    // Activate VSync
    SDL_GL_SetSwapInterval(1);

    SDL_ShowWindow(m_Window); 
}

void Application::setupImGui() {
    // GLSL version
#if defined(IMGUI_IMPL_OPENGL_ES2)
    const char* glslVersion = "#version 100";
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    const char* glslVersion = "#version 300 es";
#elif defined(__APPLE__)
    const char* glslVersion = "#version 150";
#else
    const char* glslVersion = "#version 130";
#endif

    // Create ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // ImGui flags
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Default theme
    ImGui::StyleColorsDark();

    // Init backend SDL + OpenGL
    ImGui_ImplSDL3_InitForOpenGL(m_Window, m_GLContext);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void Application::cleanup()
{
    // Close ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    // Close SDL
    if (m_GLContext) {
        SDL_GL_DestroyContext(m_GLContext);
        m_GLContext = nullptr;
    }
    if (m_Window) {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }
    SDL_Quit();
}

void Application::run() {
    while (!m_Done)
    {
        processEvents();

        if (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // ----------------------------------------------------------------------------------
        if (m_ShowDemoWindow)
            ImGui::ShowDemoWindow(&m_ShowDemoWindow);

        {
            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &m_ShowDemoWindow);
            ImGui::Checkbox("Another Window", &m_ShowAnotherWindow);

            ImGui::SliderFloat("float", &m_SliderFloat, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&m_Renderer.m_ClearColor);

            if (ImGui::Button("Button"))
                m_Counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", m_Counter);

            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);

            ImGui::End();
        }

        if (m_ShowAnotherWindow)
        {
            ImGui::Begin("Another Window", &m_ShowAnotherWindow);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                m_ShowAnotherWindow = false;
            ImGui::End();
        }
        // ----------------------------------------------------------------------------------

        m_Renderer.render(ImGui::GetIO());

        SDL_GL_SwapWindow(m_Window);
    }
}

void Application::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
        {
            m_Done = true;
        }
        else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED
                 && event.window.windowID == SDL_GetWindowID(m_Window))
        {
            m_Done = true;
        }
    }

}