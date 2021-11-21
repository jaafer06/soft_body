#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <iostream>
#include "utils/shader.h"
#include "utils/utils.h"
#include "utils/mesh.h"
#include "utils/vertexArray.h"
#include "camera.h"

template<typename T, unsigned int N>
using Attribute = utils::Attribute<T,N>;

int main(void) {
    constexpr unsigned int width = 900;
    constexpr unsigned int height = 480;

    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    glfwSwapInterval(1);

    gladLoadGL();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);
    glDebugMessageCallback(MessageCallback, 0);

    GLuint fragmentShader = utils::loadShader("../src/shaders/fragment_shader.glsl", GL_FRAGMENT_SHADER);
    GLuint vertexShader = utils::loadShader("../src/shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    GLuint programID = utils::linkShaders(fragmentShader, vertexShader);
    glUseProgram(programID);

    utils::Mesh mesh;
    mesh.loadObj("../meshes/bunny.obj");
    mesh.normalize();
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getTriangles().size() * 3 * sizeof(unsigned int), mesh.getTriangles().data(), GL_STATIC_DRAW);
    
    utils::VertexArray<Attribute<float,4>,Attribute<float,4>,Attribute<float, 4>>
        vertexArray(mesh.getVertices().data(), mesh.getVertices().size());
    Camera camera(width, height, glGetUniformLocation(programID, "MVP"));
     while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, mesh.getTriangles().size() * 3, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
