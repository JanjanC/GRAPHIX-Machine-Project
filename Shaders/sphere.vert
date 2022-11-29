#version 330 core

layout(location = 0) in vec3 aPos; //vertices

uniform mat4 projection; //projection matrix
uniform mat4 view; //view matrix
uniform mat4 transform; //transformation matrix

void main () {
	gl_Position = projection * view * transform * vec4(aPos, 1.0); //compute the final position of the vertex
}
