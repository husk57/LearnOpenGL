
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

struct SpotLight {
    vec3 position;
    vec3 direction;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};
uniform SpotLight spotLight;

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); //32 is pow factor
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
    vec4 ambient = texture(texture_diffuse1, TexCoord) * 0.2; //0.2 is ambient factor
    vec4 diffuse = diff * texture(texture_diffuse1, TexCoord);
    vec4 specular = spec * texture(texture_specular1, TexCoord);
    ambient = vec4(vec3(ambient.x, ambient.y, ambient.z) * attenuation, ambient.w);
    diffuse = vec4(vec3(diffuse.x, diffuse.y, diffuse.z) * attenuation, diffuse.w);
    specular = vec4(vec3(specular.x, specular.y, specular.z) * attenuation, specular.w);
    return (ambient + diffuse + specular);
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); //32 is pow factor
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
    vec4 ambient = texture(texture_diffuse1, TexCoord) * 0.2; //0.2 is ambient factor
    vec4 diffuse = diff * texture(texture_diffuse1, TexCoord);
    vec4 specular = spec * texture(texture_specular1, TexCoord);
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    ambient = vec4(vec3(ambient.x, ambient.y, ambient.z) * attenuation * intensity, ambient.w);
    diffuse = vec4(vec3(diffuse.x, diffuse.y, diffuse.z) * attenuation * intensity, diffuse.w);
    specular = vec4(vec3(specular.x, specular.y, specular.z) * attenuation * intensity, specular.w);
    return (ambient + diffuse + specular);
}

void main()
{
    
    vec4 col = vec4(0.0);
    vec3 viewDir = normalize(cameraPosition - fPosition);
    for(int i = 0; i < nrPointLights; i++) {
        col += CalcPointLight(pointLights[i], fNormal, fPosition, viewDir);
    }
    if (texture(texture_diffuse1, TexCoord).a < 0.01) {
        discard;
    }
    col += CalcSpotLight(spotLight, fNormal, fPosition, viewDir);
    FragColor = col;
}
