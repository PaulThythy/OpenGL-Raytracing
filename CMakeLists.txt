cmake_minimum_required(VERSION 3.5)
project(OpenglRaytracer C CXX)

file(GLOB_RECURSE sources src/**.cpp)
file(GLOB_RECURSE headers src/**.h)

include_directories(${CMAKE_SOURCE_DIR}/src)

# SDL3
set(SDL_DIR ${CMAKE_SOURCE_DIR}/libs/SDL)
add_subdirectory(${SDL_DIR})
include_directories(${SDL_DIR}/include)

# GLM
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glm/glm)
include_directories(${PROJECT_SOURCE_DIR}/libs/glm)

# Dear ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

add_executable(${PROJECT_NAME} ${sources} ${headers}
    ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# tinyobjloader
set(TINYOBJLOADER_DIR ${CMAKE_SOURCE_DIR}/libs/tinyobjloader)
add_subdirectory(${TINYOBJLOADER_DIR})
include_directories(${TINYOBJLOADER_DIR})

# OpenGL
if(UNIX)
    # Linux 
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)

    set(LIBRARIES "SDL3::SDL3;OpenGL::GL;GLEW::GLEW;glm::glm")
else()
   # Windows 
   set(GLEW_ROOT_DIR "${PROJECT_SOURCE_DIR}/libs/GLEW")
   set(GLEW_INCLUDE_DIR "${GLEW_ROOT_DIR}/include")

   set(GLEW_LIB_DIR     "${GLEW_ROOT_DIR}/lib/Release/x64")
   set(GLEW_DLL_DIR     "${GLEW_ROOT_DIR}/Release/x64")

   include_directories(${GLEW_INCLUDE_DIR})

   set(LIBRARIES
        SDL3::SDL3
        glm::glm 
        "${GLEW_LIB_DIR}/glew32.lib"
        opengl32
    )

   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
       "${GLEW_DLL_DIR}/glew32.dll"
       $<TARGET_FILE_DIR:${PROJECT_NAME}>
   )

   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy 
        $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )

endif(UNIX)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}/shaders" SHADER_DIR)
file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}/mesh" MESH_DIR)
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_DIR="${SHADER_DIR}")
target_compile_definitions(${PROJECT_NAME} PRIVATE MESH_DIR="${MESH_DIR}")