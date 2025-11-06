#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform float width;
uniform float height;
uniform float fromX;
uniform float fromY;
uniform float sizeX;
uniform float sizeY;
uniform int maxIters;
uniform bool isSmoothing;

uniform float THRESHOLD1;
uniform float THRESHOLD2;

uniform vec3 colorA = vec3(0.5, 0.5, 0.5);
uniform vec3 colorB = vec3(0.5, 0.5, 0.5);
uniform vec3 colorC = vec3(1.0, 1.0, 1.0);
uniform vec3 colorD = vec3(0.30, 0.20, 0.20);
uniform float brightness;
uniform float contrast;

float mandelbrot_one_iter(vec2 coord)
{

    float x0 = fromX + (coord.x + 0.5f) * sizeX / width;
    float y0 = fromY + (coord.y + 0.5f) * sizeY / height;

    float x = x0;
    float y = y0;

    int iter = 0;
    for (; iter < maxIters; ++iter) {
        float xPrev = x;
        x = x * x - y * y + x0;
        y = 2.0f * xPrev * y + y0;
        if ((x * x + y * y) > THRESHOLD2)
            break;
    }

    float result = float(iter);
    if (isSmoothing && iter != maxIters) {
        result = result - log(log(sqrt(x * x + y * y)) / log(THRESHOLD1)) / log(2.0f);
    }

    return result / float(maxIters);
}

//https://iquilezles.org/articles/palettes/
vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
    return a + b * cos(6.28318 * (c * t + d));
}

void main() {

    vec2 coord = fragCoord;

    float value = mandelbrot_one_iter(coord);

    vec3 color;
    if (value >= 1.0) {
        color = colorA;
    } else {
        color =  palette(value, colorA, colorB, colorC, colorD);
        color = (color - 0.5) * contrast + 0.5;
        color *= brightness;
        // color = clamp(color, 0.0, 1.0);
    }

    fragColor = vec4(color, 1.0);
}
