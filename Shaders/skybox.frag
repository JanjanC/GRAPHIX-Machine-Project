#version 330 core

out vec4 FragColor; //output fragment color

in vec3 texCoords; //texture coordinates

uniform samplerCube skybox; //skybox cubemap 

void main(){
	FragColor = texture(skybox, texCoords); //call the skybox color
}