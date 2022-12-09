#version 330 core //version

uniform sampler2D tex0;

in vec2 texCoord;

out vec4 FragColor;

void main () {
	vec4 pixelColor = texture(tex0, texCoord);

	if (pixelColor.a < 0.1) {
		discard;
	}

	FragColor = pixelColor;
}