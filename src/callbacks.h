#pragma once
#include <glad/glad.h>
#include "camera.h"
#include <GLFW/glfw3.h>
#include <chrono> 
#include <thread> 
#include <atomic>

class CallBacks {
public:

	CallBacks(GLFWwindow* window, Camera& camera)
		: window(window), camera(camera), isKeyPressed{ false }, movementMode(false) {
		
		glfwGetWindowSize(window, &width, &height);
		glfwSetWindowUserPointer(window, this);
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
			auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
			self.mouseCallback(window, x, y);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
			self.keyCallback(window, key, scancode, action, mods);
		});

		std::thread eventThread([&]() { this->handleEvents(); });
		eventThread.detach();
	}

	static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		auto& self = *static_cast<CallBacks*>(glfwGetWindowUserPointer(window));
		if (self.movementMode) {
			float _dx = xpos - (self.width / 2.);
			float _dy = ypos - (self.height / 2.);
			glfwSetCursorPos(window, self.width / 2., self.height / 2.);
			self.dx += _dx;
			self.dy += _dy;
		}
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

		if (action == GLFW_PRESS && key == GLFW_KEY_C) {
			self.movementMode = !self.movementMode;
			if (self.movementMode) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(window, self.width / 2., self.height / 2.);
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	}

	void handleEvents() {
		while (true) {
			const int deltaTimeMs = 50;
			std::this_thread::sleep_for(std::chrono::milliseconds(deltaTimeMs));
			if (!movementMode) {
				continue;
			}
			auto const movementDirection = getMovementDirection(deltaTimeMs);
			float _dx = dx.load();
			float _dy = dy.load();
			camera.move(movementDirection, dx, dy);
			dx -= _dx;
			dy -= _dy;
		}
	}

private:
	Camera& camera;
	GLFWwindow* window;
	bool isKeyPressed[GLFW_KEY_MENU + 1];
	bool movementMode;
	int width, height;
	std::atomic<float> dx;
	std::atomic<float> dy;

	Eigen::Vector4f getMovementDirection(float deltaTimeMs) {
		Eigen::Vector4f direction{ 0, 0, 0, 0 };
		if (isKeyPressed[GLFW_KEY_W]) {
			direction += camera.getForwardDirection() * (deltaTimeMs/500);
		}
		if (isKeyPressed[GLFW_KEY_S]) {
			direction -= camera.getForwardDirection() * (deltaTimeMs / 500);
		}
		if (isKeyPressed[GLFW_KEY_D]) {
			direction += camera.getRightDirection() * (deltaTimeMs / 500);
		}
		if (isKeyPressed[GLFW_KEY_A]) {
			direction -= camera.getRightDirection() * (deltaTimeMs / 500);
		}
		return direction;
	}
};