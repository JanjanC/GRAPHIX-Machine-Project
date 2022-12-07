#version 330 core

layout(location = 0) in vec3 aPos; //vertices
layout(location = 1) in vec3 vertexNormal; //normals
layout(location = 2) in vec2 aTex; //textures

out vec3 fragPos; //output vertices
out vec3 normCoord; //output normals
out vec2 texCoord; //output textures

uniform mat4 projection; //projection matrix
uniform mat4 view; //view matrix
uniform mat4 transform; //transformation matrix

void main () {

	gl_Position = projection * view * transform * vec4(aPos, 1.0); //compute the final position of the vertex

	texCoord = aTex; //output the texture coordinate

	normCoord = mat3(transpose(inverse(transform))) * vertexNormal; //apply normal matrix to the normal data

	fragPos = vec3(transform * vec4(aPos, 1.0)); //calculate the fragment position after transformation
}
