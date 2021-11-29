#version 330 core

uniform vec4[2] light;
in vec3 vertexPosition;
in vec3 normal;
in vec4 vertexColor;

out vec3 color;

void main() {
	vec3 lightPosition = light[0].xyz;
	vec3 lightColor = light[1].xyz;
	
	vec3 vertexToLight = normalize(lightPosition - vertexPosition);
	float lightIntensity = dot(vertexToLight, normalize(normal));
	color = vertexColor.xyz * (clamp(lightIntensity, 0., 1.));
	// color = vec3(1,0,0);
}