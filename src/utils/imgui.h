#pragma once
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "Eigen/Dense"
#include <string>
#include <iostream>
#include <functional>
namespace utils {
	class ImGuiWrapper {
	public:
		ImGuiWrapper(GLFWwindow* window, std::string&& glsl_version) {
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init(glsl_version.c_str());
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
		}

		template<typename T>
		void display(T&) {}

		template<typename Type, int n, int m>
		void display(Eigen::Matrix<Type, n, m>& matrix) {
			for (unsigned int i = 0; i < n; ++i) {
				for (unsigned int j = 0; j < m; ++j) {
					std::cout << matrix(i, j) << " ";
				};
				std::cout << std::endl;
			}
		}

		template<typename Type, int n>
		void display(Eigen::Matrix<Type, n, 1>& matrix) {
			for (unsigned int i = 0; i < n; ++i) {
				std::cout << matrix(i) << " ";
			}
		}

		template<typename Type>
		void display(Eigen::Matrix<Type, 4, 1>& vector) {
			auto callback = [&]() { ImGui::InputFloat4("Rect", vector.data()); };
			callbacks.push_back(callback);
		}

		void render() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("check da stats man!");                          // Create a window called "Hello, world!" and append into it.

			for (auto& callback : callbacks) {
				callback();
			}

			ImGui::End();
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		std::vector<std::function<void()>> callbacks;

	};

}
