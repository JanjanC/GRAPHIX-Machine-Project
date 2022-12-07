#version 330 core

layout(location = 0) in vec3 aPos; //vertices
layout(location = 1) in vec3 vertexNormal; //normals
layout(location = 2) in vec2 aTex; //textures
layout(location = 3) in vec3 m_tan; //tangent
layout(location = 4) in vec3 m_btan; //bitangent

out vec3 fragPos; //output vertices
out vec3 normCoord; //output normals
out vec2 texCoord; //output textures

out mat3 TBN; //output TBN matrix

uniform mat4 projection; //projection matrix
uniform mat4 view; //view matrix
uniform mat4 transform; //transformation matrix

void main () {

	gl_Position = projection * view * transform * vec4(aPos, 1.0); //calculate the final position of the vertex

	texCoord = aTex; //output the texture coordinate

	//extract this from the previous formula of normCoord
	mat3 modelMat =mat3(transpose(inverse(transform)));
	normCoord = modelMat * vertexNormal;

	//construct the components of the TBN matrix
	vec3 T = normalize(modelMat * m_tan); //tangent
	vec3 B = normalize(modelMat * m_btan); //bitangent
	vec3 N = normalize(normCoord); //normal

	TBN = mat3(T, B, N); //assign the TBN output

	fragPos = vec3(transform * vec4(aPos, 1.0)); //calculate and output the fragment position after transformation
}
