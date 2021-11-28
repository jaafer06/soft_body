#version 330 core
struct Light {
	vec4 position;
	vec4 color;
};

uniform Light light;
in vec4 world_position;
in vec4 normal;

out vec3 color;

void main() {
	
	color = vec3(1,0,0);
}