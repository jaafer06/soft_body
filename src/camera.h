#pragma once
#include <Eigen/Dense>
#include <string>
#include <glad/glad.h>


class Camera {
public:
	Camera(unsigned int width, unsigned int height, int MVP_UNIFORM_LOCATION = -1) 
			: ratio(width / (float)height), MVP_UNIFORM_LOCATION(MVP_UNIFORM_LOCATION) {

		MVP = Eigen::Matrix<float,4,4,Eigen::ColMajor>::Identity();
		MVP(0, 0) = 1/ratio;
		if (MVP_UNIFORM_LOCATION != -1) {
			update_MVP();
		}
	}

	const Eigen::Matrix<float, 4, 4, Eigen::ColMajor>& getMVP() {
		return MVP;
	}

	void set_MVP_uniform_location(int location) {
		MVP_UNIFORM_LOCATION = location;
	}
	
	void update_MVP() {
		glUniformMatrix4fv(MVP_UNIFORM_LOCATION, 1, GL_FALSE, MVP.data());
	}

private:
	Eigen::Matrix<float,4,4,Eigen::ColMajor> MVP;
	Eigen::Vector4f position;
	Eigen::Vector4f forward;
	Eigen::Vector4f up;
	Eigen::Vector4f right;
	float focalLength;
	float ratio;
	GLint MVP_UNIFORM_LOCATION;
};