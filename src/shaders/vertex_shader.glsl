#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 _normal;
layout(location = 2) in vec4 _vertexColor;

uniform mat4 ViewProjection;
uniform mat4 Model;

out vec3 vertexPosition;
out vec3 normal;
out vec4 vertexColor;
void main() {
	gl_Position = ViewProjection * Model * position;
	vertexPosition = (Model * position).xyz;
	normal = mat3(Model) * _normal.xyz;
	vertexColor = _vertexColor;
}