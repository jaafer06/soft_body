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
#include "scene.h"
#include <string>

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

    camera.setUniformLocation(glGetUniformLocation(programID, "ViewProjection"));
    light.setUniformLocation(glGetUniformLocation(programID, "light"));

    Scene scene(glGetUniformLocation(programID, "Model"));
    Mesh bunnyMesh;
    bunnyMesh.loadObj("../meshes/beast.obj");
    Mesh cubeMesh;
    cubeMesh.loadObj("../meshes/cube.obj");
    cubeMesh.preprocessNormalize();
    cubeMesh.preprocessSetColor({ 1, 1, 1, 1 });
    cubeMesh.preprocessScale(10, 0.1, 10);
    bunnyMesh.preprocessNormalize();
    bunnyMesh.preprocessTranslate({ 0, 2, 0 });

    scene.addMesh<Attribute<float,4>,Attribute<float,4>,Attribute<float,4>>(bunnyMesh);
    scene.addMesh<Attribute<float,4>,Attribute<float,4>,Attribute<float,4>>(cubeMesh);


    imguiWrapper.display(scene[0].getTransform(), "bunny model transform");
    imguiWrapper.display(camera.getViewProjectionMatrix(), "ViewProjection matrix");
    imguiWrapper.displaySlider(light.getPosition(), "light position");

    while (!glfwWindowShouldClose(window)) {
        //mesh.translate({ 0, 0, -0.01 });
        //mesh.rotate(0, 0, 0.01);

        camera.updateUniform();
        light.update();
        scene.render();
        imguiWrapper.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glfwTerminate();
    return 0;
}
