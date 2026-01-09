#version 330 core

uniform sampler2D colorMap;
uniform int samples = 4; // half samples
uniform bool hasBlur = true;

in vec2 fragTexCoord;

out vec4 fragColor;

void main() {
    if(samples > 0 && hasBlur) {
        vec2 size = 1.0 / vec2(textureSize(colorMap, 0));
        float result = 0.0;
        for (float x = -samples; x < samples; ++x) {
            for (float y = -samples; y < samples; ++y) {
                vec2 offset = vec2(float(x), float(y)) * size;
                result += texture(colorMap, fragTexCoord + offset).r;
            }
        }
        result /= samples * samples * 4;

        fragColor = vec4(vec3(result), 1.0);
    } else {
        fragColor = texture(colorMap, fragTexCoord);
    }
}