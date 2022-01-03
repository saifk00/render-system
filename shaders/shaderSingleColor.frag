#version 330 core

out vec4 FragColor;

uniform vec4 solidColor = vec4(0.04, 0.28, 0.26, 1.0);

void main() {
	FragColor = solidColor;
}