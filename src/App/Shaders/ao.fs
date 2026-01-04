#version 330

in vec2 TexCoord;
in vec2 ViewRay;
in vec4 pos;

out vec4 FragColor;

uniform sampler2D gDepthMap;
uniform float gSampleRad = 0.5f;

uniform mat4 gProj;
const int MAX_KERNEL_SIZE = 64;
uniform vec3 gKernel[MAX_KERNEL_SIZE];

float zNear = 0.1; 
float zFar  = 100.0; 

float LinearizeDepth(float depth) {
    // преобразуем обратно в NDC
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

float CalcViewZ(vec2 Coords) {
    float Depth = texture(gDepthMap, Coords).x;
    float ViewZ = gProj[3][2] / (2 * Depth -1 - gProj[2][2]);
    return ViewZ;
}

void main() {
    float ViewZ = CalcViewZ(TexCoord);

    float ViewX = ViewRay.x * ViewZ;
    float ViewY = ViewRay.y * ViewZ;

    vec3 Pos = vec3(ViewX, ViewY, ViewZ);

    float AO = 0.0;

    for (int i = 0 ; i < MAX_KERNEL_SIZE ; i++) {
        vec3 samplePos = Pos + gKernel[i];
        vec4 offset = vec4(samplePos, 1.0);
        offset = gProj * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);

        float sampleDepth = CalcViewZ(offset.xy);

        if (abs(Pos.z - sampleDepth) < gSampleRad) {
            AO += step(sampleDepth,samplePos.z);
        }
    }

    AO = 1.0 - AO/64.0;

    FragColor = vec4(pow(AO, 2.0));

    //FragColor = vec4(vec3(ViewZ * 100), 1.0);
    //float depth = LinearizeDepth(texture(gDepthMap, TexCoord).x);
    //FragColor = vec4(vec3(depth), 1.0);
} 