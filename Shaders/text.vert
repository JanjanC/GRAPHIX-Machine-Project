#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 2) in vec2 aTex;

out vec2 texCoord;

void main () {
	vec2 normalized = (aPos - vec2(360,360)) / vec2(360,360); // [0..800][0..600] -> [-400..400][-300..300]

	gl_Position = vec4(normalized, 0.0, 1.0);

	texCoord = aTex;
}
