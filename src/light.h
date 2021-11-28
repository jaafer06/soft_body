#pragma once
#include <glad/glad.h>
#include <Eigen/Dense>


class Light {
public:
	Light(Eigen::Vector4f position, Eigen::Vector4f color = {1, 1, 1, 1}, int uniformLocation = -1) 
		: uniformLocation(uniformLocation), data{position, color}
	{
		if (uniformLocation != -1) {
			glUniform4fv(uniformLocation, 2, data.position.data());
		}
	}

	void setUniformLocation(int uniformLocation) {
		this->uniformLocation = uniformLocation;
	}

	Eigen::Vector4f& getPosition() {
		return data.position;
	}

	Eigen::Vector4f& getColor() {
		return data.color;
	}

	void setPosition(Eigen::Vector4f& newPosition) {
		data.position = newPosition;
	}

	void update() {
		glUniform4fv(uniformLocation, 2, data.position.data());
	}

	void setColor(Eigen::Vector4f& newColor) {
		data.color = newColor;
	}

private:
	int uniformLocation;
	struct  {
		Eigen::Vector4f position;
		Eigen::Vector4f color;
	} data;

};