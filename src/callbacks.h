#pragma once
#include <glad/glad.h>
#include "camera.h"
#include <GLFW/glfw3.h>
#include <chrono> 
#include <thread> 

class CallBacks {
public:

	CallBacks(GLFWwindow* window, Camera& camera) : camera(camera), isKeyPressed{ false } {
		glfwSetWindowUserPointer(window, this);
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
			auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
			self.mouseCallback(window, x, y);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
			self.keyCallback(window, key, scancode, action, mods);
		});

		std::thread keyThread([&]() { this->handleKeyEvents(); });
		keyThread.detach();
	}
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
		std::cout << xpos << " " << ypos << std::endl;
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS)
		{
			self.isKeyPressed[key] = true;
		}
		if (action == GLFW_RELEASE) {
			self.isKeyPressed[key] = false;
		}
	}

	void handleKeyEvents() {
		while (true) {
			const int deltaTimeMs = 50;
			std::this_thread::sleep_for(std::chrono::milliseconds(deltaTimeMs));
			if (isKeyPressed[GLFW_KEY_W]) {
				camera.move(Camera::DIRECTION::FORWARD, deltaTimeMs);
			}
			if (isKeyPressed[GLFW_KEY_S]) {
				camera.move(Camera::DIRECTION::BACKWARD, deltaTimeMs);
			}

			if (isKeyPressed[GLFW_KEY_D]) {
				camera.move(Camera::DIRECTION::LEFT, deltaTimeMs);
			}
			if (isKeyPressed[GLFW_KEY_A]) {
				camera.move(Camera::DIRECTION::RIGHT, deltaTimeMs);
			}
		}
	}

private:
	Camera& camera;
	bool isKeyPressed[GLFW_KEY_MENU + 1];
};