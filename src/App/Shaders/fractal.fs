#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

// uniform float width;
// uniform float height;
// uniform float fromX;
// uniform float fromY;
// uniform float sizeX;
// uniform float sizeY;
// uniform int maxIters;
// uniform bool isSmoothing;

float mandelbrot_one_iter(vec2 coord, float fromX, float fromY, float width, float height,
                          float sizeX, float sizeY,
                          int maxIters, bool isSmoothing)
{
    const float THRESHOLD1 = 256.0f;
    const float THRESHOLD2 = 65536.0f;

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


void main() {
    float width = 1000.0f;
	float height = 1000.0f;
    float fromX = -2.5;
    float fromY = -1.5;
    float sizeX = 1000.0;
    float sizeY = 1000.0;
	int maxIters = 100;
	bool isSmoothing = true;

    vec2 coord = fragCoord;

    float value = mandelbrot_one_iter(coord, fromX, fromY, width, height, sizeX, sizeY, maxIters, isSmoothing);

    vec3 color;
    if (value >= 1.0) {
        color = vec3(0.0);
    } else {
        color = vec3(
            sin(value * 10.0) * 0.5 + 0.5,
            cos(value * 8.0) * 0.5 + 0.5,
            sin(value * 12.0 + 1.0) * 0.5 + 0.5
        );
    }

    fragColor = vec4(color, 1.0);
}
