#version 330

layout(location=0) in vec3 position;

out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(position, 1.0);
    fragTexCoord = (position.xy + vec2(1.0)) / 2.0;
}