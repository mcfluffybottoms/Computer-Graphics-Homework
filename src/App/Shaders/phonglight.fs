#version 330 core

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D albedoMap;

uniform sampler2D colorMap;
uniform sampler2D aoMap;

in vec2 fragTexCoord;

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
uniform vec2 screenSize;

uniform DirectionalLight dirLight[1];
uniform ProjectionLight projLight[1];

uniform bool debugAO = false;

out vec4 out_col;

void main() {
    if(debugAO) {
        float aoFactor = texture(aoMap, fragTexCoord).r;
        vec4 ao = vec4(vec3(aoFactor), 1.0);
        out_col = ao;
        return;
    }

    vec3 fragPos = texture(positionMap, fragTexCoord).rgb;
    vec3 fragNormal = texture(normalMap, fragTexCoord).rgb;
    vec4 texColor = texture(albedoMap, fragTexCoord);

    vec4 baseColor = texColor; 

    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);
    float aoFactor = texture(aoMap, fragTexCoord).r;
    vec4 result = vec4(0.0);

    if (hasDirectional) {
        vec3 lightDirection = normalize(-dirLight[0].lightPos);

        vec4 ambientRes = vec4(dirLight[0].ambient * dirLight[0].lightColor * aoFactor, 1.0f);

        float diff = max(dot(norm, lightDirection), 0.0);
        vec4 diffuseRes = vec4(dirLight[0].diffuse * diff * dirLight[0].lightColor, 1.0f);

        vec3 reflectDir = reflect(-lightDirection, norm);
        float specAngle = max(dot(viewDir, reflectDir), 0.0);
        float specPower = pow(specAngle, 64.0);
        vec4 specularRes = vec4(dirLight[0].specular * specPower * dirLight[0].lightColor, 1.0f);

        result += (ambientRes + diffuseRes + specularRes) * baseColor;
    }

    if (hasProjection) {
        vec3 lightDirection = normalize(projLight[0].lightPos - fragPos);
        float theta = dot(lightDirection, normalize(-projLight[0].lightDir));
        float epsilon = projLight[0].cutOff - projLight[0].outerCutOff;
        float intensity = clamp((theta - projLight[0].outerCutOff) / epsilon, 0.0, 1.0);

        if (theta > projLight[0].outerCutOff) {
            vec3 ambientRes = projLight[0].ambient * projLight[0].lightColor * aoFactor;
            float diff = max(dot(norm, lightDirection), 0.0);
            vec3 diffuseRes = projLight[0].diffuse * diff * projLight[0].lightColor;

            vec3 halfwayDir = normalize(lightDirection + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
            vec3 specularRes = projLight[0].specular * spec * projLight[0].lightColor;

            result += vec4((ambientRes + (diffuseRes + specularRes) * intensity), 1.0) * baseColor;
        }
    }

    if (!hasDirectional && !hasProjection) {
        result = baseColor * aoFactor;
    }

    out_col = result;
}