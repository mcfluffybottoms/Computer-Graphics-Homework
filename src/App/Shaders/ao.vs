#version 330

layout(location=0) in vec3 position;
uniform float aspectRatio = 1.0;
uniform float tanHalfFOV = 0.5;

out vec2 fragTexCoord;
out vec2 viewRay;

void main() {
    gl_Position = vec4(position, 1.0);
    fragTexCoord = (position.xy + vec2(1.0)) / 2.0;
    viewRay = vec2(
        position.x * aspectRatio * tanHalfFOV,
        position.y * tanHalfFOV);
}