cmake_minimum_required(VERSION 3.24)
project(SpaceStationTourism)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(third-party)
add_executable(SpaceStationTourism main.cpp
        Renderer/GLRenderer.h
        Renderer/GLRenderer.cpp
        Renderer/Texture.h
        Renderer/Texture.cpp
        Renderer/Mesh.h
        Renderer/Shader.h
        Renderer/Shader.cpp
        Renderer/Mesh.cpp
        Renderer/Camera.h
        Renderer/Camera.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_include_directories(${PROJECT_NAME} PRIVATE third-party/SDL/include)
target_include_directories(${PROJECT_NAME} PRIVATE third-party/glew/include)
target_include_directories(${PROJECT_NAME} PRIVATE third-party/concurrentqueue)
target_include_directories(${PROJECT_NAME} PRIVATE third-party)


#target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
target_link_libraries(${PROJECT_NAME} PRIVATE glew)
target_link_libraries(${PROJECT_NAME} PRIVATE Tracy::TracyClient)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
