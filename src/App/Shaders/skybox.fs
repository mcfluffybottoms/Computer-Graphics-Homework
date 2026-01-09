#version 330 core

layout(location=0) out vec3 position; // add
layout(location=2) out vec4 albedo;
layout(location=3) out vec4 applyAO;

uniform samplerCube skybox;

in vec3 TexCoords;
out vec4 FragColor;

void main() {
    FragColor = texture(skybox, TexCoords);
    albedo = FragColor;
}