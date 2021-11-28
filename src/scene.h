#pragma once
#include "camera.h"
#include "utils/mesh.h"

class Scene {
public:
	Scene(Camera& camera): camera(camera) {

	}

	void addMesh() {

	}

private:
	Camera& camera;
};