cmake_minimum_required(VERSION 3.16)

project(OpenGLProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(OpenGLProject
    src/main.cpp
    src/particle.cpp
    src/fire.cpp
    src/glad.c
)

target_include_directories(OpenGLProject PRIVATE
    include
)

target_link_libraries(OpenGLProject
    PRIVATE
    OpenGL::GL
    glfw
)
