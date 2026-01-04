#version 330

layout(location=0) in vec3 position;
uniform float gAspectRatio = 1.0;
uniform float gTanHalfFOV = 0.5;

out vec2 TexCoord;
out vec2 ViewRay;
out vec4 pos;

void main() {
    gl_Position = vec4(position, 1.0);
    pos = vec4(position, 1.0);
    TexCoord = (position.xy + vec2(1.0)) / 2.0;
    ViewRay.x = position.x * gAspectRatio * gTanHalfFOV;
    ViewRay.y = position.y * gTanHalfFOV;
}