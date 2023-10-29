cmake_minimum_required(VERSION 3.24)

#add_subdirectory(raylib)
add_subdirectory(flecs)


option( TRACY_ENABLE "" ON)
option( TRACY_ON_DEMAND "" ON)
add_subdirectory(tracy)

add_subdirectory(SDL)

option (GLEW_CUSTOM_OUTPUT_DIRS ON)
add_subdirectory(glew/build/cmake)

add_subdirectory(glm)


add_subdirectory(assimp)

#imgui

FILE(GLOB var "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")

set(var ${var} imgui/backends/imgui_impl_sdl3.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/misc/cpp/imgui_stdlib.cpp)

ADD_LIBRARY( imgui STATIC
        ${var})

target_include_directories(imgui PRIVATE SDL/include)
target_include_directories(imgui PRIVATE imgui/backends)
target_include_directories(imgui PRIVATE imgui)