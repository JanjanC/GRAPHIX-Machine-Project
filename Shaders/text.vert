#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTex;

out vec2 texCoord;

void main () {
	vec3 normalized = (aPos - vec3(360, 360, 0)) / vec3(360, 360, 1); // [0..800][0..600] -> [-400..400][-300..300]

	gl_Position = vec4(normalized, 1.0);

	texCoord = aTex;
}
