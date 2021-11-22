#version 330 core
layout(location = 0) in vec4 vertexPosition_modelspace;
uniform mat4 MVP;

void main(){
	gl_Position = MVP * vec4(vertexPosition_modelspace.xyz, 1);
	// gl_Position += vec4(0, 0, 0, 1);
}