#version 330 core

in vec3 fragPos;

out vec4 posOut;

void main() {
    posOut = fragPos;
}