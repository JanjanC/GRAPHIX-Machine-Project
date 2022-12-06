#version 330 core

layout (location = 0) in vec3 aPos; //vertex of the skybox

out vec3 texCoords; //texture coordinate of the cubemap

uniform mat4 projection; //projection matrix
uniform mat4 view; //view matrix

void main() {
	
	//calculate the position of the skybox
	vec4 pos = projection * view * vec4(aPos, 1.0);

	//calculate the final position in such a way that it gives the illusion that the background will not move despite moving closer
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

	texCoords = aPos; //output the vertex
}