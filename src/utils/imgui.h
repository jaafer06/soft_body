#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "Eigen/Dense"
#include <string>
#include <iostream>
#include <functional>
#include <string>

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

		template<typename Type, int n, int m>
		void display(Eigen::Matrix<Type, n, m>& matrix, const std::string displayName) {
			const auto callback = [&](std::string& displayName) {
				if (!ImGui::TreeNode(displayName.c_str()) || !ImGui::BeginTable("", m))
					return;
				for (unsigned int i = 0; i < m; ++i) {
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				}

				for (int row = 0; row < n; row++) {
					ImGui::TableNextRow();
					for (int column = 0; column < m; column++) {
						ImGui::TableSetColumnIndex(column);
						const std::string label = std::to_string(row) + "-" + std::to_string(column);
						ImGui::PushID(row + column * n);
						ImGui::InputFloat("", &matrix(row, column));
						ImGui::PopID();
					}
				}
				ImGui::EndTable();
				ImGui::TreePop();
			};
			named_callbacks.push_back({ callback, displayName });
		}

		template<typename Type>
		void display(Eigen::Matrix<Type, 4, 1>& vector, const std::string displayName) {
			const auto callback = [&](std::string& displayName) { 
				ImGui::InputFloat4(displayName.c_str(), vector.data());
			};
			named_callbacks.push_back({ callback,  displayName });
		}

		void render() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("check da stats man!");
			for (auto& named_callback : named_callbacks) {
				auto& [callback, displayName] = named_callback;
				callback(displayName);
			}
			ImGui::End();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		std::vector<std::tuple<std::function<void(std::string&)>, std::string>> named_callbacks;

	};

}
