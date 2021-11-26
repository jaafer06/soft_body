#pragma once
#include <Eigen/Dense>
#include <string>
#include <glad/glad.h>
#include <Eigen/Geometry>
#include <utils/eigen.h>

using namespace utils::EigenUtils;

class Camera {

public:

	Camera(unsigned int width, unsigned int height, float focalLength, int MVP_UNIFORM_LOCATION = -1) 
			: ratio(width / (float)height), MVP_UNIFORM_LOCATION(MVP_UNIFORM_LOCATION),
				up{ 0, 1, 0, 0 }, right{ 1, 0, 0, 0 }, width(width), height(height) {
		h = 0;
		v = 0;
		position = { 0, 0, 0, 1 };
		orientation = { 0, 0, 1, 0 };

		projection = Eigen::Matrix<float,4,4>::Identity();
		view = Eigen::Matrix<float,4,4>::Identity();
		model = Eigen::Matrix<float,4,4>::Identity();

		projection(0, 0) = (1 / ratio) * focalLength;
		projection(1, 1) = focalLength;
		projection(3, 2) = -1;
		projection(3, 3) = 0;

		MVP = projection;
		if (MVP_UNIFORM_LOCATION != -1) {
			update_MVP();
		}

	}

	const Eigen::Matrix<float, 4, 4>& getMVP() {
		return MVP;
	}

	void set_MVP_uniform_location(int location) {
		MVP_UNIFORM_LOCATION = location;
		update_MVP();
	}
	
	void update_MVP() {
		glUniformMatrix4fv(MVP_UNIFORM_LOCATION, 1, GL_FALSE, MVP.data());
	}

	enum class DIRECTION {
		FORWARD, BACKWARD, RIGHT, LEFT
	};

	void move(DIRECTION direction, float deltaTimeMs) {
		if (direction == DIRECTION::FORWARD) {
			std::cout << "forward my dudes" << std::endl;
			position += orientation * (deltaTimeMs/500);
		}
		if (direction == DIRECTION::BACKWARD) {
			position -= orientation * (deltaTimeMs / 500);
		}
		if (direction == DIRECTION::RIGHT) {
			position += right * (deltaTimeMs / 500);
		}
		if (direction == DIRECTION::LEFT) {
			position -= right * (deltaTimeMs / 500);
		}
		model.block<3, 1>(0, 3) = model.block<3, 3>(0, 0) * position.head(3);
		std::cout << position << std::endl;
		update();
	}

	void rotate(float horizontal, float vertical) {
		const float rotX = vertical / height;
		const float rotY = horizontal / width;
		//orientation = Eigen::AngleAxisf(-rotX, orientation.cross(up).normalized()) * orientation;
		//orientation = Eigen::AngleAxisf(-rotY, up) * orientation;
		//auto f = orientation.block<3, 1>(0, 0).cross(up.block<3, 1>(0, 0));
		//std::cout << rotX << "\t" << rotY << std::endl;

		const auto r1 = Eigen::AngleAxisf(-rotX, orientation.block<3, 1>(0, 0).cross(up.block<3, 1>(0, 0)).normalized());
		const auto r2 = Eigen::AngleAxisf(rotY, up.head(3));
		const auto R = r2 * r1;
		orientation.head(3) = R * orientation.head(3);
		model.block<3, 3>(0, 0) = R * model.block<3, 3>(0, 0);
		model.block<3, 1>(0, 3) = model.block<3, 3>(0, 0) * position.head(3);
		//model.block<3, 1>(0, 3) = R * model.block<3, 1>(0, 3);
		//std::cout << orientation << std::endl;

		update();
	}

private:
	void update() {
		MVP = projection * model;
	}
	float width, height;
	float h, v;
	Eigen::Matrix<float,4,4> MVP;
	Eigen::Matrix<float,4,4> projection;
	Eigen::Matrix<float,4,4> view;
	Eigen::Matrix<float,4,4> model;

	Eigen::Vector4f position;
	Eigen::Vector4f orientation;
	const Eigen::Vector4f up;
	const Eigen::Vector4f right;
	float focalLength;
	float ratio;
	GLint MVP_UNIFORM_LOCATION;
};