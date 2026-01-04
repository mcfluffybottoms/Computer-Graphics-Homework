#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoords;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 itMatrix;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
}