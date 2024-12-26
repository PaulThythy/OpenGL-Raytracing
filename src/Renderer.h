#ifndef RENDERER_H
#define RENDERER_H

#include "imgui.h"
#include <SDL3/SDL.h>

struct Renderer {
    ImVec4 m_ClearColor;

    inline Renderer():m_ClearColor(0.45f, 0.55f, 0.60f, 1.00f) {}

    inline void render(const ImGuiIO& io) {
        ImGui::Render();

        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));

        glClearColor(m_ClearColor.x * m_ClearColor.w,
                    m_ClearColor.y * m_ClearColor.w,
                    m_ClearColor.z * m_ClearColor.w,
                    m_ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif