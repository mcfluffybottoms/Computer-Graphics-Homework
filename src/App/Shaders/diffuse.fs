#version 330 core

uniform sampler2D tex_2d;
uniform float ambient;
uniform float diffuse;

uniform vec3 viewPos;
uniform vec3 lightColor;  
uniform vec3 lightPos;  

in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoord;

out vec4 out_col;

void main() {
	vec3 objectColor = vec3(0.5, 0, 0.5);
    vec4 texel = texture(tex_2d, texCoord);
	vec3 ambientRes = ambient * lightColor;

	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseRes = diffuse * diff * lightColor;

	float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32);
    vec3 specularRes = specularStrength * spec * lightColor;

    vec3 result = (ambientRes + diffuseRes + specularRes) * objectColor;
    out_col = vec4(result, objectColor);
}
