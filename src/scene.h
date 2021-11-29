#pragma once
#include "camera.h"
#include "utils/mesh.h"
#include "light.h"
#include <string>
#include <vector>
#include "utils/vertexArray.h"
#include "utils/eigen.h"
#include "light.h"

using namespace utils::EigenUtils;
template<typename Type, unsigned int n>
using Attribute = utils::Attribute<Type, n>;
using Mesh = utils::Mesh;

struct RenderableMesh {
	Mesh mesh;
	int vertexArrayId;
	int bufferId;
	int elementArrayBufferId;
};

class Scene {
public:
	Scene(int modelMatrixUniform): modelMatrixUniform(modelMatrixUniform)
	{
		lights.push_back({ { 0, 5, 10, 1 }, { 1, 1, 1, 1 } });
	}

	Mesh& operator[](unsigned int i) {
		return renderableMeshes[i].mesh;
	}

	// todo implement
	//void addLight(const Eigen::Vector4f& position, const Eigen::Vector4f& color) {
	//	lights.push_back({position, color});
	//}

	// todo handle different vertrex layouts
	template<typename T, typename... Args>
	Mesh& addMeshFromObj(const std::string& file) {
		renderableMeshes.push_back({});
		RenderableMesh& rMesh = renderableMeshes.back();
		Mesh& mesh = rMesh.mesh;
		mesh.loadObj(file);
		setupRenderableMesh<T, Args...>(rMesh);
		return mesh;
	}

	template<typename T, typename... Args>
	Mesh& addMesh(Mesh& mesh) {
		renderableMeshes.push_back({});
		RenderableMesh& rMesh = renderableMeshes.back();
		rMesh.mesh = mesh;
		setupRenderableMesh<T, Args...>(rMesh);
		return mesh;
	}

	void render() {
		for (auto& rMesh : renderableMeshes) {
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, rMesh.mesh.getTransform().data());
			glBindVertexArray(rMesh.vertexArrayId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rMesh.elementArrayBufferId);
			glDrawElements(GL_TRIANGLES, rMesh.mesh.getTriangles().size() * 3, GL_UNSIGNED_INT, nullptr);
		}
	}

private:

	template<typename T, typename... Args>
	void setupRenderableMesh(RenderableMesh& rMesh) {
		Mesh& mesh = rMesh.mesh;
		utils::VertexArray<T, Args...>
			vertexArray(mesh.getVertices().data(), mesh.getVertices().size());
		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getTriangles().size() * 3 * sizeof(unsigned int), mesh.getTriangles().data(), GL_STATIC_DRAW);

		rMesh.bufferId = vertexArray.bufferId;
		rMesh.vertexArrayId = vertexArray.vertexArrayId;
		rMesh.elementArrayBufferId = ibo;
	}

	std::vector<RenderableMesh> renderableMeshes;
	std::vector<Light> lights;
	int modelMatrixUniform;
};