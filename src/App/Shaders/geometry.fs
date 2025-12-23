#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

out vec4 posOut;

void main() {
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = vec3(0.95);
}