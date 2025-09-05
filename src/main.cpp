#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <openglErrorReporting.h>
#include <iostream>
#include <fstream>
#include "shader_loading.h"
#include "window_init.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FastNoiseLite.h>

float vertices[] = {
    // Positions           // Colors
    // Back face (z = -0.5)
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  // 0: back-bottom-left
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,  // 1: back-bottom-right
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,  // 2: back-top-right
    -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  // 3: back-top-left

    // Front face (z = +0.5)
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,  // 4: front-bottom-left
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,  // 5: front-bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,  // 6: front-top-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f   // 7: front-top-left
};

unsigned int indices[] = {
    // Back face
    0, 1, 2,
    2, 3, 0,

    // Front face
    4, 5, 6,
    6, 7, 4,

    // Left face
    0, 4, 7,
    7, 3, 0,

    // Right face
    1, 5, 6,
    6, 2, 1,

    // Bottom face
    0, 1, 5,
    5, 4, 0,

    // Top face
    3, 2, 6,
    6, 7, 3
};

glm::vec3 offsets[6] = {
    glm::vec3( 2.0f, 0.0f, 0.0f), // right
    glm::vec3(-2.0f, 0.0f, 0.0f), // left
    glm::vec3(0.0f,  2.0f, 0.0f), // up
    glm::vec3(0.0f, -2.0f, 0.0f), // down
    glm::vec3(0.0f,  0.0f, 2.0f), // forward
    glm::vec3(0.0f,  0.0f, -2.0f) // backward
};

int main() {
    //enableReportGlErrors();

    FastNoiseLite noise;   // Create a noise generator
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); // or another type

    GLFWwindow* window = initWindow();
    if (!window) {
        std::cerr << "Failed to initialize window!" << std::endl;
        return -1;
    }

    Shader shader;
    if (!shader.loadShaderProgramFromFile(
        "../resources/rainbow_shader.vert", 
        "../resources/rainbow_shader.frag")) {
        std::cerr << "Failed to load shaders!" << std::endl;
        glfwTerminate();
        return -1;
    }
    shader.bind();

    glEnable(GL_DEPTH_TEST);

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    // verter buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // elememt buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // unbind vao
    glBindVertexArray(0);

    // get window size and set viewport
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    float angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.01f;

        float t = glfwGetTime(); // or just increment a float each frame
        float speed = 1.0f;
        t *= speed * 8;
        float fragSpeed = speed;
        glUniform1f(glGetUniformLocation(shader.getId(), "t"), t);
        glUniform1f(glGetUniformLocation(shader.getId(), "speed"), speed);
        glUniform1f(glGetUniformLocation(shader.getId(), "fragSpeed"), fragSpeed);

        /* Smoothly oscillate between 0 and 1 using sin
        float x = (sin(t + 2.0f) + 1.0f) * 0.5f; // 0..1
        float y = (sin(t + 4.0f) + 1.0f) * 0.5f; // offset phase for variety
        float z = (sin(t + 8.0f) + 1.0f) * 0.5f;
        */

        float x = noise.GetNoise(t, 0.0f) * 5.0f; // range
        float y = noise.GetNoise(t, 100.0f) * 5.0f;
        float z = noise.GetNoise(t, 200.0f) * 5.0f;

        glUniform1f(glGetUniformLocation(shader.getId(), "x"), x);
        glUniform1f(glGetUniformLocation(shader.getId(), "y"), y);
        glUniform1f(glGetUniformLocation(shader.getId(), "z"), z);

        glm::vec3 axis(x, y, z);
        if (glm::length(axis) < 0.0001f) axis = glm::vec3(0.0f, 1.0f, 0.0f);
        axis = glm::normalize(axis);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.f);

        glm::mat4 modelCenter = glm::mat4(1.0f);
        modelCenter = glm::translate(modelCenter, glm::vec3(x / 2.0f, y / 2.0f, z / 5.0f)); // moving path
        modelCenter = glm::rotate(modelCenter, angle, axis);
        modelCenter = glm::scale(modelCenter, glm::vec3(0.5f));
        glm::mat4 mvpCenter = projection * view * modelCenter;

        // center model
        glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "u_MVP"), 1, GL_FALSE, glm::value_ptr(mvpCenter));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (int i = 0; i < 6; i++) {
            glm::mat4 model = modelCenter;
            model = glm::translate(model, offsets[i]);
            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "u_MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, &vbo);
    glDeleteBuffers(1, &ebo);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
// cd build
// cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" ..
// mingw32-make
// ./OpenGL.exe