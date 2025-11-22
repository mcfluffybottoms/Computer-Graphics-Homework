#version 330 core

uniform sampler2D tex_2d;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoord;

out vec4 out_col;

void main() {
    vec4 texel = texture(tex_2d, texCoord);
    float greyscale_factor = dot(texel.rgb, vec3(0.21, 0.71, 0.07));

    out_col = vec4(1.0, 0.0, 0.0, 1.0);

}
