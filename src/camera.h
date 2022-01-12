#pragma once
#include <Eigen/Dense>
#include <string>
#include <glad/glad.h>
#include <Eigen/Geometry>


class Camera {

public:

	Camera(unsigned int width, unsigned int height, float focalLength, int uniformLocation = -1)
			: ratio(width / (float)height), uniformLocation(uniformLocation),
				up{ 0, 1, 0, 0 }, right{ 1, 0, 0, 0 }, width(width), height(height) {

		position = { 0, 0, 0, 1 };
		orientation = { 0, 0, -1, 0 };

		projection = Eigen::Matrix4f::Identity();
		view = Eigen::Matrix4f::Identity();

		projection(0, 0) = (1 / ratio) * focalLength;
		projection(1, 1) = focalLength;
		projection(3, 2) = -1;
		projection(3, 3) = 0;

		projection(2, 2) = (100 + 0.1) / (0.1 - 100);
		projection(2, 3) = (2 * 100 * 0.1) / (0.1 - 100);

		ViewProjection = projection;
		if (uniformLocation != -1) {
			updateUniform();
		}
	}

	const Eigen::Matrix<float, 4, 4>& getViewProjectionMatrix() {
		return ViewProjection;
	}

	void setUniformLocation(int location) {
		uniformLocation = location;
	}
	
	void updateUniform() {
		update();
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, ViewProjection.data());
	}

	void move(const Eigen::Vector4f& translation, const float horizontal, const float vertical) {
		position.head(3) += translation.head(3);
		const float rotX = vertical / height;
		const float rotY = horizontal / width;
		const auto r1 = Eigen::AngleAxisf(-rotX, orientation.block<3, 1>(0, 0).cross(up.block<3, 1>(0, 0)).normalized());
		const auto r2 = Eigen::AngleAxisf(-rotY, up.head(3));
		const auto R = r2 * r1;
		orientation.head(3) = R * orientation.head(3);
		right.head(3) = r2 * right.head(3);
		view.block<3, 3>(0, 0) = view.block<3, 3>(0, 0) * R.inverse();
		view.block<3, 1>(0, 3) = view.block<3, 3>(0, 0) * -position.head(3);
		update();
	}

	const Eigen::Vector4f& getForwardDirection() {
		return orientation;
	}

	const Eigen::Vector4f& getRightDirection() {
		return right;
	}

	Eigen::Matrix<float, 4, 4>& getViewMatrix() {
		return view;
	}

private:
	void update() {
		ViewProjection = projection * view;
	}

	float width, height;
	Eigen::Matrix4f ViewProjection;
	Eigen::Matrix4f projection;
	Eigen::Matrix4f view;

	Eigen::Vector4f position;
	Eigen::Vector4f orientation;
	const Eigen::Vector4f up;
	Eigen::Vector4f right;
	float focalLength;
	float ratio;
	GLint uniformLocation;
};