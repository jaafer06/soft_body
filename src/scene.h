#pragma once
#include <string>
#include <vector>
#include <map>
#include <Eigen/Dense>
#include "camera.h"
#include "utils/mesh.h"
#include "light.h"
#include "utils/vertexArray.h"
#include "utils/eigen.h"
#include "light.h"

using namespace utils::EigenUtils;
template<typename Type, unsigned int n>
using Attribute = utils::Attribute<Type, n>;
using Mesh = utils::Mesh;

class RenderableMesh {
public:
	RenderableMesh(Mesh* mesh, const std::string& name = "n/a") : mesh(mesh) {
		transform = Eigen::Matrix4f::Identity();
		this->name = name;
	}

	void translate(const Eigen::Vector3f& t) {
		transform.block<3, 1>(0, 3) += t;
	}

	void rotate(float xAxis, float yAxis, float zAxis) {
		const auto r = Eigen::AngleAxisf(xAxis, Eigen::Vector3f{ 1, 0, 0 })
			* Eigen::AngleAxisf(yAxis, Eigen::Vector3f{ 0, 1, 0 })
			* Eigen::AngleAxisf(zAxis, Eigen::Vector3f{ 0, 0, 1 });
		transform.block<3, 1>(0, 3) = r * transform.block<3, 1>(0, 3);
		transform.block<3, 3>(0, 0) = r * transform.block<3, 3>(0, 0);
	}

	Eigen::Matrix4f transform;
	Mesh* mesh;
	int vertexArrayId{ -1 };
	int bufferId{ -1 };
	int elementArrayBufferId{ -1 };
	unsigned int index{ 0 };
	std::string name;
};

class Scene {
public:
	Scene(int modelMatrixUniform): modelMatrixUniform(modelMatrixUniform)
	{
		lights.push_back({ { 0, 5, 10, 1 }, { 1, 1, 1, 1 } });
	}

	RenderableMesh& operator[](unsigned int i) {
		return *renderableMeshes[i];
	}

	// todo implement
	//void addLight(const Eigen::Vector4f& position, const Eigen::Vector4f& color) {
	//	lights.push_back({position, color});
	//}

	// todo handle different vertrex layouts
	template<typename T, typename... Args>
	RenderableMesh& addMeshFromObj(const std::string& file, const std::string& name = "") {
		Mesh* mesh = new Mesh();
		mesh->loadObj(file);
		RenderableMesh& rMesh = new RenderableMesh(mesh, name);
		setupRenderableMesh<T, Args...>(rMesh);
		return rMesh;
	}

	template<typename T, typename... Args>
	RenderableMesh& addMesh(Mesh* mesh, const std::string& name = "") {
		RenderableMesh* rMesh = new RenderableMesh(mesh, name);
		setupRenderableMesh<T, Args...>(rMesh);
		return *rMesh;
	}

	void render() {
		for (auto rMeshPointer : renderableMeshes) {
			RenderableMesh& rMesh = *rMeshPointer;
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, rMesh.transform.data());
			glBindVertexArray(rMesh.vertexArrayId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rMesh.elementArrayBufferId);
			glDrawElements(GL_TRIANGLES, rMesh.mesh->getTriangles().size() * 3, GL_UNSIGNED_INT, nullptr);
		}
	}

private:

	template<typename T, typename... Args>
	void setupRenderableMesh(RenderableMesh* rMesh) {
		rMesh->index = renderableMeshes.size();
		renderableMeshes.push_back(rMesh);
		Mesh& mesh = *(rMesh->mesh);
		utils::VertexArray<T, Args...>
			vertexArray(mesh.getVertices().data(), mesh.getVertices().size());
		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getTriangles().size() * 3 * sizeof(unsigned int), mesh.getTriangles().data(), GL_STATIC_DRAW);

		rMesh->bufferId = vertexArray.bufferId;
		rMesh->vertexArrayId = vertexArray.vertexArrayId;
		rMesh->elementArrayBufferId = ibo;
	}

	std::vector<RenderableMesh*> renderableMeshes;
	std::vector<Light> lights;
	int modelMatrixUniform;
};