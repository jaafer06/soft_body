#pragma once
#include <Eigen/Dense>
#include <string>
#include <glad/glad.h>


class Camera {

public:

	Camera(unsigned int width, unsigned int height, float focalLength, int MVP_UNIFORM_LOCATION = -1) 
			: ratio(width / (float)height), MVP_UNIFORM_LOCATION(MVP_UNIFORM_LOCATION) {

		position = { 0, 0, 0, 1 };
		forward = { 0, 0, 1, 1 };
		right = { 1, 0, 0, 1 };
		up = { 0, 1, 0, 1 };

		projection = Eigen::Matrix<float,4,4>::Identity();
		view = Eigen::Matrix<float,4,4>::Identity();
		model = Eigen::Matrix<float,4,4>::Identity();

		projection(0, 0) = (1 / ratio) * focalLength;
		projection(1, 1) = focalLength;
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
			position.head(3) += forward.head(3) * (deltaTimeMs/500);
		}
		if (direction == DIRECTION::BACKWARD) {
			position.head(3) -= forward.head(3) * (deltaTimeMs / 500);
		}
		if (direction == DIRECTION::RIGHT) {
			position.head(3) += right.head(3) * (deltaTimeMs / 500);
		}
		if (direction == DIRECTION::LEFT) {
			position.head(3) -= right.head(3) * (deltaTimeMs / 500);
		}
		update();
	}

private:
	void update() {
		model.block<3, 1>(0, 3) = position.head(3);
		MVP = projection * model;
	}

	Eigen::Matrix<float,4,4> MVP;
	Eigen::Matrix<float,4,4> projection;
	Eigen::Matrix<float,4,4> view;
	Eigen::Matrix<float,4,4> model;

	Eigen::Vector4f position;
	Eigen::Vector4f forward;
	Eigen::Vector4f up;
	Eigen::Vector4f right;
	float focalLength;
	float ratio;
	GLint MVP_UNIFORM_LOCATION;

};