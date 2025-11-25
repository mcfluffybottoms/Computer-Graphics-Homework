#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoords;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 normalMatrix;
uniform float sphereRadius = 1.5;
uniform float morphIntensity = 1.0;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;


vec3 sphereCoord(float radius) {
    vec3 direction = normalize(position);
    return direction * radius - vec3(radius, radius, radius);
}

vec3 reverseCoord(float radius) {
    return vec3(-position.x, -position.y, position.z);
}

void main() {
    vec3 center = vec3(0, 0, 0);
    vec3 calcedPos = sphereCoord(sphereRadius);
    
    vec3 morphedPosition = mix(position, calcedPos, morphIntensity);

    vec3 sphereNormal = normalize(calcedPos);
    vec3 morphedNormal = normalize(mix(normal, sphereNormal, morphIntensity));

    fragPos = vec3(model * vec4(morphedPosition, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * morphedNormal; // better some some out of shader
    fragTexCoord = texCoords;

    gl_Position = mvp * vec4(morphedPosition, 1.0);
}