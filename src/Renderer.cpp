#include "Renderer.h"

Renderer::Renderer() {}

Renderer::~Renderer() { cleanup(); }

void Renderer::init(int width, int height) {
    initComputeShader(width, height);
    initRenderShader();
}

void Renderer::cleanup() {
    if (m_ComputeProgram) {
        glDeleteProgram(m_ComputeProgram);
        m_ComputeProgram = 0;
    }
    if (m_RenderProgram) {
        glDeleteProgram(m_RenderProgram);
        m_RenderProgram = 0;
    }
    if (m_ComputeTexture) {
        glDeleteTextures(1, &m_ComputeTexture);
        m_ComputeTexture = 0;
    }
    if (m_QuadVBO) {
        glDeleteBuffers(1, &m_QuadVBO);
        m_QuadVBO = 0;
    }
    if (m_QuadVAO) {
        glDeleteVertexArrays(1, &m_QuadVAO);
        m_QuadVAO = 0;
    }
}

void Renderer::initComputeShader(int width, int height) {
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
        GL_TEXTURE_2D,      // target
        0,                  // level
        GL_RGBA32F,         // internal format
        width, height,               // width, height
        0,                  // border
        GL_RGBA,            // px format
        GL_FLOAT,           // type
        nullptr             // data (nullptr => initialized as void)
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3) Specify that this texture can be written as an image in the compute shader
    glBindImageTexture(
        0,                      // unit = 0
        m_ComputeTexture,
        0,                      // level
        GL_FALSE,               // layered
        0,                      // layer
        GL_WRITE_ONLY,          // access
        GL_RGBA32F              // format
    );

    // good practice : unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Compute shader initialized.\n";
}

void Renderer::initRenderShader() {
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
        //X     Y           U   V
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

void Renderer::runComputeShader(int width, int height)
{
    if (!m_ComputeProgram) {
        return;
    }

    // 1) Activate compute shader
    glUseProgram(m_ComputeProgram);

    // Calculate the number of groups (16x16) covering w,h
    int groupX = (width + 16 - 1) / 16;
    int groupY = (height + 16 - 1) / 16;

    glDispatchCompute(groupX, groupY, 1);

    // 3) Make sure that writing to the image2D is complete before reading or using the texture afterwards.
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // 4) Deactivate program
    glUseProgram(0);

    // At this point, m_ComputeTexture contains the result of the compute shader
    // We could now render (a quad fullscreen) by reading this texture
    // using a conventional vertex+fragment shader.
}

void Renderer::renderFullscreenQuad()
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

void Renderer::resizeComputeTexture(int width, int height)
{
    if (m_ComputeTexture) {
        glDeleteTextures(1, &m_ComputeTexture);
        m_ComputeTexture = 0;
    }
    glGenTextures(1, &m_ComputeTexture);
    glBindTexture(GL_TEXTURE_2D, m_ComputeTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA32F,
        width, height,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindImageTexture(
        0,
        m_ComputeTexture,
        0,
        GL_FALSE,
        0,
        GL_WRITE_ONLY,
        GL_RGBA32F
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}
