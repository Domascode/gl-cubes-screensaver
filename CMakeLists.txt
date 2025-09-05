cmake_minimum_required(VERSION 3.10)
project(OpenGLApp)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    C:/msys64/ucrt64/include
)

link_directories(
    C:/msys64/ucrt64/lib
)

add_library(glad src/glad.c)
add_library(openglErrorReporting src/openglErrorReporting.cpp)

add_executable(OpenGL src/main.cpp)

target_link_libraries(OpenGL glad glfw3 opengl32 gdi32)
