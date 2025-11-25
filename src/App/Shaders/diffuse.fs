#version 330 core

uniform sampler2D tex_2d;

struct DirectionalLight {
    vec3 lightColor;  
    vec3 lightPos;
    float ambient;
    float diffuse;
    float specular;
};

struct ProjectionLight {
    vec3 lightColor;  
    vec3 lightPos;
    vec3 lightDir;
    float ambient;
    float diffuse;
    float specular;
    float cutOff;
    float outerCutOff;
};

uniform vec3 viewPos;
uniform bool hasDirectional;  
uniform bool hasProjection; 

uniform DirectionalLight dirLight[1];
uniform ProjectionLight projLight[1];

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 out_col;

void main() {
    vec3 texColor = texture(tex_2d, fragTexCoord).rgb;
    vec3 baseColor = texColor.rgb;
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);

    if (hasDirectional) {
        vec3 lightDirection = normalize(-dirLight[0].lightPos);

        vec3 ambientRes = dirLight[0].ambient * dirLight[0].lightColor;

        float diff = max(dot(norm, lightDirection), 0.0);
        vec3 diffuseRes = dirLight[0].diffuse * diff * dirLight[0].lightColor;

        vec3 reflectDir = reflect(-lightDirection, norm);
        float specAngle = max(dot(viewDir, reflectDir), 0.0);
        float specPower = pow(specAngle, 64.0);
        vec3 specularRes = dirLight[0].specular * specPower * dirLight[0].lightColor;

        result += (ambientRes + diffuseRes + specularRes) * baseColor;
    }

    if (hasProjection) {
        vec3 lightDirection = normalize(projLight[0].lightPos - fragPos);
        float theta = dot(lightDirection, normalize(-projLight[0].lightDir));
        float epsilon = projLight[0].cutOff - projLight[0].outerCutOff;
        float intensity = clamp((theta - projLight[0].outerCutOff) / epsilon, 0.0, 1.0);

        if (theta > projLight[0].outerCutOff) {
            vec3 ambientRes = projLight[0].ambient * projLight[0].lightColor;
            float diff = max(dot(norm, lightDirection), 0.0);
            vec3 diffuseRes = projLight[0].diffuse * diff * projLight[0].lightColor;

            vec3 halfwayDir = normalize(lightDirection + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
            vec3 specularRes = projLight[0].specular * spec * projLight[0].lightColor;

            result += (ambientRes + (diffuseRes + specularRes) * intensity) * baseColor;
        }
    }

    if (!hasDirectional && !hasProjection) {
        result = baseColor;
    }
    out_col = vec4(result, 1.0);
}