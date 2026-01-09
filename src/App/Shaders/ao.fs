#version 330

in vec2 fragTexCoord;
in vec2 viewRay;

out vec4 FragColor;

uniform bool hasAO = true;

uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D positionMap;
uniform sampler2D noiseMap;

uniform float sampleRadius = 0.5;
uniform float bias = 0.025;
uniform mat4 projection;

uniform int KERNEL_SIZE = 64;
const int MAX_KERNEL_SIZE = 256;
uniform vec3 kernel[MAX_KERNEL_SIZE];

uniform float zNear = 0.1; 
uniform float zFar  = 100.0; 
uniform vec2 screenSize;

void main() {
    if(!hasAO) {
        FragColor = vec4(1.0);
        return;
    }

    float depth = texture(depthMap, fragTexCoord).x;
    vec3 normal = texture(normalMap, fragTexCoord).xyz;
    vec3 fragPos = texture(positionMap, fragTexCoord).xyz;

    float viewZ = projection[3][2] / (2 * depth -1 - projection[2][2]);
    float viewX = viewRay.x * viewZ;
    float viewY = viewRay.y * viewZ;
    vec3 pos = vec3(viewX, viewY, viewZ);

    vec2 noiseScale = vec2(screenSize.x/4.0, screenSize.y/4.0); 
    vec3 randomVec = normalize(texture(noiseMap, fragTexCoord * noiseScale).xyz);

    // from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion  = 0.0;
    for (int i = 0 ; i < KERNEL_SIZE ; i++) {
        // get sample position
        vec3 samplePos = pos + tbn * kernel[i] * sampleRadius;

        // project sample position to texture
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);

        float depth = texture(depthMap, offset.xy).x;
        float sampleDepth = projection[3][2] / (2 * depth -1 - projection[2][2]);

        // range check
        float rangeCheck = smoothstep(0.0, 1.0, sampleRadius / abs(fragPos.z - sampleDepth));
        if(sampleDepth >= samplePos.z + bias) {
            occlusion += rangeCheck;
        }
    }

    occlusion  = 1.0 - occlusion  / KERNEL_SIZE;
    FragColor = vec4(pow(occlusion , 2.0));
} 