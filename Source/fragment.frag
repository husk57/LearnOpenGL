
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 fPosition;
in vec3 fNormal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform vec3 cameraPosition;

const int nrPointLights = 4;

struct PointLight {
    vec3 position;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[nrPointLights];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); //32 is pow factor
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = vec3(texture(texture_diffuse1, TexCoord)) * 0.2; //0.2 is ambient factor
    vec3 diffuse = diff * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = spec * vec3(texture(texture_specular1, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


void main()
{
    
    vec3 col = vec3(0.0);
    vec3 viewDir = normalize(cameraPosition - fPosition);

    for(int i = 0; i < nrPointLights; i++) {
        col += CalcPointLight(pointLights[i], fNormal, fPosition, viewDir);
    }
    
    FragColor = vec4(col, 1.0);
}
