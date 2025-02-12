#include "Application.h"
#include "globals/Globals.h"

Application::Application() {
    setupSDL();
    initSDL();
    setupImGui();

    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);

    m_Renderer.init(w, h, m_Window);
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
    m_Window = SDL_CreateWindow("OpenGL-Raytracer",
                                Config::INIT_WINDOW_WIDTH, Config::INIT_WINDOW_HEIGHT,
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
    SDL_GL_SetSwapInterval(0);

    if (glewInit() != GLEW_OK) {
        std::runtime_error(std::string("Failed to initialize GLEW"));
    }

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
    //cleanup Renderer
    m_Renderer.cleanup();

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
    ImVec4 clearColor(0.45f, 0.55f, 0.60f, 1.00f);
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

        int w, h;
        SDL_GetWindowSize(m_Window, &w, &h);

        m_Renderer.runComputeShader(w, h);

        // ----------------------------------------------------------------------------------
        if (m_ShowDemoWindow)
            ImGui::ShowDemoWindow(&m_ShowDemoWindow);

        {
            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &m_ShowDemoWindow);
            ImGui::Checkbox("Another Window", &m_ShowAnotherWindow);

            ImGui::SliderFloat("float", &m_SliderFloat, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clearColor);

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

        ImGui::Render();

        glViewport(0, 0, static_cast<int>(ImGui::GetIO().DisplaySize.x), static_cast<int>(ImGui::GetIO().DisplaySize.y));

        glClearColor(clearColor.x * clearColor.w,
                    clearColor.y * clearColor.w,
                    clearColor.z * clearColor.w,
                    clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Renderer.renderFullscreenQuad();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(m_Window);
    }
}

void Application::processEvents() {
    SDL_Event event;

    const float cameraMoveSpeed     = 0.1f;
    const float cameraRotationSpeed = 0.1f;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch(event.type)
        {
            case SDL_EVENT_QUIT:
            {
                m_Done = true;
            }
            break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                if (event.window.windowID == SDL_GetWindowID(m_Window)) {
                    m_Done = true;
                }
            }
            break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    m_RightMouseButtonDown = true;

                    SDL_GetMouseState(&m_LastMouseX, &m_LastMouseY);
                }
            }
            break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    m_RightMouseButtonDown = false;
                }
            }
            break;

            case SDL_EVENT_MOUSE_MOTION:
            {
                if (m_RightMouseButtonDown) {
                    m_Renderer.m_CameraMoved = true;

                    int currentX = event.motion.x;
                    int currentY = event.motion.y;

                    float deltaX = static_cast<float>(currentX - m_LastMouseX);
                    float deltaY = static_cast<float>(currentY - m_LastMouseY);

                    // deltaX corresponds to rotation around Up (Yaw)
                    // deltaY corresponds to rotation around Right (Pitch)
                    m_Renderer.m_Scene.m_Camera.rotateAroundUp(-deltaX * cameraRotationSpeed);
                    m_Renderer.m_Scene.m_Camera.rotateAroundRight(-deltaY * cameraRotationSpeed);

                    m_LastMouseX = currentX;
                    m_LastMouseY = currentY;
                }
            }
            break;

            case SDL_EVENT_MOUSE_WHEEL:
            {
                m_Renderer.m_CameraMoved = true;

                float delta = static_cast<float>(event.wheel.y) * cameraMoveSpeed;
                m_Renderer.m_Scene.m_Camera.moveForward(delta);
            }
            break;

            case SDL_EVENT_WINDOW_RESIZED:
            {
                int newW = event.window.data1;
                int newH = event.window.data2;

                m_Renderer.resizeComputeTexture(newW, newH);
                m_Renderer.m_Scene.m_Camera.m_AspectRatio = (float)newW/(float)newH;
                m_Renderer.m_Scene.m_Camera.updateUniforms();
                m_Renderer.runComputeShader(newW, newH);
            }
            break;

            default:
                break;
        }
    }

}