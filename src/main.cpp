#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <iostream>
#include "utils/shader.h"
#include "utils/utils.h"
#include "utils/mesh.h"
#include "utils/vertexArray.h"
#include "utils/imgui.h"
#include "camera.h"
#include <functional>
#include <type_traits>
#include "callbacks.h"
#include "scene.h"
#include "light.h"
#include "utils/eigen.h"
#include "utils/shapes.h"

using namespace utils::EigenUtils;
template<typename Type, unsigned int n>
using Attribute = utils::Attribute<Type, n>;

int main(void) {

    constexpr unsigned int width = 1280;
    constexpr unsigned int height = 780;
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    utils::ImGuiWrapper imguiWrapper(window, "#version 130");
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    glfwSwapInterval(1);

    Camera camera(width, height, 1);
    camera.move({0, 2, 0, 0}, 0, -100);
    CallBacks callBacks(window, camera);
    Light light({ 0, 5, 10, 1 }, {1, 1, 1, 1});

    gladLoadGL();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    GLuint fragmentShader = utils::loadShader("../src/shaders/fragment_shader.glsl", GL_FRAGMENT_SHADER);
    GLuint vertexShader = utils::loadShader("../src/shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    GLuint programID = utils::linkShaders(fragmentShader, vertexShader);
    glUseProgram(programID);

    utils::Mesh mesh;
    mesh.loadObj("../meshes/cube.obj");
    mesh.preprocessNormalize();
    mesh.preprocessSetColor({ 1, 1, 1, 1 });
    mesh.preprocessScale(10, 0.1, 10);
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getTriangles().size() * 3 * sizeof(unsigned int), mesh.getTriangles().data(), GL_STATIC_DRAW);

    utils::VertexArray<Attribute<float,4>,Attribute<float,4>,Attribute<float, 4>>
        vertexArray(mesh.getVertices().data(), mesh.getVertices().size());
    
    camera.setUniformLocation(glGetUniformLocation(programID, "ViewProjection"));
    light.setUniformLocation(glGetUniformLocation(programID, "light"));

    imguiWrapper.display(mesh.getTransform(), "model transform");
    imguiWrapper.display(camera.getViewProjectionMatrix(), "ViewProjection matrix");
    imguiWrapper.displaySlider(light.getPosition(), "light position");

    while (!glfwWindowShouldClose(window)) {
        //mesh.translate({ 0, 0, -0.01 });
        //mesh.rotate(0, 0, 0.01);

        glUniformMatrix4fv(glGetUniformLocation(programID, "Model"), 1, GL_FALSE, mesh.getTransform().data());
        camera.updateUniform();
        light.update();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, mesh.getTriangles().size() * 3, GL_UNSIGNED_INT, nullptr);
        imguiWrapper.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glfwTerminate();
    return 0;
}
