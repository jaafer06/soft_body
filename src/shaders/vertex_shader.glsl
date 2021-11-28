#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 _normal;
layout(location = 2) in vec4 color;

uniform mat4 ViewProjection;
uniform mat4 Model;

out vec4 world_position;
out vec4 normal;

void main(){
	world_position = Model * position;
	gl_Position = ViewProjection * world_position;
	normal = _normal;
}