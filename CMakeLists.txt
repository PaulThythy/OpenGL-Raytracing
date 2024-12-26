cmake_minimum_required(VERSION 2.8)
project(OpenglRaytracer C CXX)

file(GLOB_RECURSE sources src/**.cpp)
file(GLOB_RECURSE headers src/**.h)

include_directories(${CMAKE_SOURCE_DIR}/src)

# SDL3
set(SDL_DIR ${CMAKE_SOURCE_DIR}/libs/SDL)
add_subdirectory(${SDL_DIR})
include_directories(${SDL_DIR}/include)

# Dear ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# OpenGL
find_package(OpenGL REQUIRED)

set(LIBRARIES "SDL3::SDL3;OpenGL::GL")

add_executable(${PROJECT_NAME} ${sources} ${headers}
    ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})