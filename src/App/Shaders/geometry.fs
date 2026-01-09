#version 330 core

uniform sampler2D colorMap;

layout(location=0) out vec3 position; // add
layout(location=1) out vec3 normal;
layout(location=2) out vec4 albedo;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

void main() {
    position = fragPos;
    normal = fragNormal;
    albedo = texture(colorMap, fragTexCoord);
}