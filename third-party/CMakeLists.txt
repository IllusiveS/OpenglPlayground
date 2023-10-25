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