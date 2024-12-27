#include "Application.h"

Application::Application() {
    setupSDL();
    initSDL();
    setupImGui();
    initComputeShader();
    initRenderShader();
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

void Application::initComputeShader() {
    // 1) Load compute shader
    m_ComputeProgram = Shader::loadComputeShader(std::string(SHADER_DIR) + "/compute.glsl");
    if (!m_ComputeProgram)
    {
        std::cerr << "Error: Could not load compute shader program.\n";
        return;
    }

    // 2) Create a texture that the compute shader will fill.
    glGenTextures(1, &m_ComputeTexture);
    glBindTexture(GL_TEXTURE_2D, m_ComputeTexture);
    glTexImage2D(
        GL_TEXTURE_2D,    // target
        0,                // level
        GL_RGBA32F,       // internal format
        512, 512,         // width, height
        0,                // border
        GL_RGBA,          // px format
        GL_FLOAT,         // type
        nullptr           // data (nullptr => initialized as void)
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3) Specify that this texture can be written as an image in the compute shader
    glBindImageTexture(
        0,                   // unit = 0
        m_ComputeTexture,
        0,                   // level
        GL_FALSE,            // layered
        0,                   // layer
        GL_WRITE_ONLY,       // access
        GL_RGBA32F           // format
    );

    // good practice : unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Compute shader initialized.\n";

}

void Application::initRenderShader()
{
    // 1) Load rendering program: vertex.glsl + fragment.glsl
    m_RenderProgram = Shader::loadRenderShader(
        std::string(SHADER_DIR) + "/vertex.glsl",
        std::string(SHADER_DIR) + "/fragment.glsl"
    );
    if (!m_RenderProgram)
    {
        std::cerr << "Error: Could not load render shader program.\n";
        return;
    }

    // 2) Create a full-screen quad.
    // For example, 2 triangles covering the screen with coordinates NDC -1...1
    // (XY positions and UV texture coord.)

    // Example data: 6 vertices (2 triangles) for a full-screen quad
    const float quadVertices[] = {
        //   X     Y       U    V
        -1.f,  1.f,       0.f, 1.f,
        -1.f, -1.f,       0.f, 0.f,
         1.f, -1.f,       1.f, 0.f,

        -1.f,  1.f,       0.f, 1.f,
         1.f, -1.f,       1.f, 0.f,
         1.f,  1.f,       1.f, 1.f
    };

    // 3) Create a VAO + VBO
    glGenVertexArrays(1, &m_QuadVAO);
    glBindVertexArray(m_QuadVAO);

    glGenBuffers(1, &m_QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // 4) vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,              // index = location = 0
        2,              // components (x, y)
        GL_FLOAT,       // type
        GL_FALSE,       // normalized ?
        4 * sizeof(float), // stride (4 floats by vertex : x,y,u,v)
        (void*)0        // offset (start from the begging)
    );

    // Texture coordinates => location=1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,              
        2,              
        GL_FLOAT,       
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );

    // 5) Unbind to prevent accidental modifications
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Render shader initialized.\n";
}


void Application::runComputeShader()
{
    if (!m_ComputeProgram) {
        return;
    }

    // 1) Activate compute shader
    glUseProgram(m_ComputeProgram);

    // 2) Run calculation on 512x512 pixels, with 16x16 groups
    //    => (512 / 16) = 32 groups in X and 32 groups in Y
    glDispatchCompute(512 / 16, 512 / 16, 1);

    // 3) Make sure that writing to the image2D is complete before reading or using the texture afterwards.
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // 4) Deactivate program
    glUseProgram(0);

    // At this point, m_ComputeTexture contains the result of the compute shader
    // We could now render (a quad fullscreen) by reading this texture
    // using a conventional vertex+fragment shader.
}

void Application::renderFullscreenQuad()
{
    glUseProgram(m_RenderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ComputeTexture);

    GLint loc = glGetUniformLocation(m_RenderProgram, "uTexture");
    if (loc >= 0)
        glUniform1i(loc, 0); 

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
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

        runComputeShader();

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

        renderFullscreenQuad();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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