#version 330 core

uniform sampler2D tex_2d;

// light coefficients
uniform float ambient;
uniform float diffuse;
uniform float specular;

uniform vec3 viewPos;

// directional light
uniform vec3 lightColor;  
uniform vec3 lightPos;        // for directional light = actual direction!
// (lightDir = normalize(-lightPos))

// spotlight
uniform vec3 projLightPos;  
uniform vec3 projLightDir;    // direction TO the scene
uniform float projCutOff;     // cos(inner angle)
uniform float projOuterCutOff;// cos(outer angle)
uniform vec3 projLightColor;

uniform bool hasDirectional;  
uniform bool hasProjection;  

in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoord;

out vec4 out_col;

void main() {
    vec3 baseColor = texture(tex_2d, texCoord).rgb;
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);

    if (hasDirectional) {
        vec3 lightDir = normalize(-lightPos);

        vec3 ambientRes = ambient * lightColor;

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseRes = diffuse * diff * lightColor;

        vec3 reflectDir = reflect(-lightDir, norm);
        float specAngle = max(dot(viewDir, reflectDir), 0.0);
        float specPower = pow(specAngle, 64);
        vec3 specularRes = specular * specPower * lightColor;

        result += (ambientRes + diffuseRes) * baseColor + specularRes;
    }

    if (hasProjection) {
        vec3 lightDir = normalize(projLightPos - fragPos);
        float theta = dot(lightDir, normalize(-projLightDir));
        float epsilon = projCutOff - projOuterCutOff;
        float intensity = clamp((theta - projOuterCutOff) / epsilon, 0.0, 1.0);

        if (theta > projOuterCutOff) {
            vec3 ambientRes = ambient * 0.1 * projLightColor;

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuseRes = diffuse * diff * projLightColor;

            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
            vec3 specularRes = specular * spec * projLightColor;

            vec3 spotLightResult = ambientRes + 
            (diffuseRes + specularRes) * intensity;

            result += spotLightResult * baseColor;
        }
    }

    if (!hasDirectional && !hasProjection) {
        result = baseColor;
    }
    out_col = vec4(result, 1.0);
}
