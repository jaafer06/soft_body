
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <iostream>
#include <chrono>
#include "utils/shader.h"
#include "utils/utils.h"
#include "utils/mesh.h"
#include "utils/vertexArray.h"
#include "utils/imgui.h"
#include "camera.h"
#include "callbacks.h"
#include "scene.h"
#include "light.h"
#include "utils/eigen.h"
#include "scene.h"
#include <string>
#include "physics.h"

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
    camera.move({0, 1, 4, 0}, 0, 0);
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
    Mesh* sphereMesh = new Mesh();
    sphereMesh->loadObj("../meshes/sphere.obj");
    Mesh* floorMesh = new Mesh();
    floorMesh->loadObj("../meshes/cube.obj");
    floorMesh->preprocessNormalize();
    floorMesh->preprocessSetColor({ 1, 1, 1, 1 });
    floorMesh->preprocessScale(10, 0.1, 10);
    sphereMesh->preprocessNormalize();
    sphereMesh->preprocessTranslate({ 0, 1, 2 });
    sphereMesh->preprocessScale(0.3);

    auto& sphere = scene.addMesh<Attribute<float,4>,Attribute<float,4>,Attribute<float,4>>(sphereMesh, "sphere");
    auto& floor = scene.addMesh<Attribute<float,4>,Attribute<float,4>,Attribute<float,4>>(floorMesh);

    imguiWrapper.display(sphere.transform, sphere.name);
    //imguiWrapper.display(scene[0].mesh.getTransform(), scene.getMeshName(sphere));
    // imguiWrapper.display(camera.getViewMatrix(), "view matrix");
    imguiWrapper.displaySlider(light.getPosition(), "light position");

    Physics<RenderableMesh> physics;
    auto& particule = physics.addParticule(sphere, 5);

    particule.applyForce({0, 5000, -2000});
    while (!glfwWindowShouldClose(window)) {
        particule.applyForce({ 0, -particule.mass * 10, 0 });
        physics.tick(0.01);

        //mesh.translate({ 0, 0, -0.01 });
        //sphere.mesh.rotate(0, 0, 0.01);
        //sphere.translate({ 0, 0, -0.01 });
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
