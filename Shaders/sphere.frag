#version 330 core //version

uniform vec3 customColor; //input fragment color
out vec4 FragColor; //output fragment color

void main () {
	FragColor = vec4(customColor, 1.0f); //set the output color to the value of the fragment color
}