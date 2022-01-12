#pragma once
#include <Eigen/Dense>
#include <vector>

template<typename T>
class Particule {
public:
	Particule(T& movableObject, float mass): mass(mass), movableObject(movableObject) {}

	void applyForce(const Eigen::Vector3f force) {
		acceleration += 0.5 * force / mass;
		this->force += force;
	}

	void translate(const Eigen::Vector3f& t) {
		movableObject.translate(t);
	}

	T& movableObject;
	Eigen::Vector3f force{ 0, 0, 0 };
	Eigen::Vector3f acceleration{ 0, 0, 0 };
	Eigen::Vector3f velocity{ 0, 0, 0 };
	Eigen::Vector3f position{ 0, 0, 0 };
	float mass;
};

template<typename T>
class Physics {
public:
	Particule<T>& addParticule(T& movableObject, float mass) {
		particules.push_back(new Particule<T>(movableObject, mass));
		return *particules.back();
	}

	void tick(float deltaTime) {

		for (auto* particulePointer: particules) {
			auto& particule = *particulePointer;
			particule.velocity += particule.acceleration * deltaTime;
			particule.translate(particule.velocity * deltaTime);
		}
		reset();
	}

	void reset() {
		for (auto* particule : particules) {
			particule->acceleration = { 0, 0, 0 };
			particule->force = { 0, 0, 0 };
		}
	}

private:
	std::vector<Particule<T>*> particules;
};