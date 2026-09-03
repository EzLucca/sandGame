cmake_minimum_required(VERSION 3.16)

project(OpenGLProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_PROFILING "Enable gprof profiling" OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(OpenGLProject
    src/main.cpp
    src/Mouse.cpp
    src/MaterialSelector.cpp
    src/particle.cpp
    src/fire.cpp
    src/glad.c
    src/Renderer.cpp
    src/Simulation.cpp
    src/Movements.cpp
)

target_include_directories(OpenGLProject PRIVATE
    include
)

target_compile_definitions(OpenGLProject PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(OpenGLProject
    PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)

if(ENABLE_PROFILING)
    target_compile_options(OpenGLProject PRIVATE -pg)
    target_link_options(OpenGLProject PRIVATE -pg)
endif()
