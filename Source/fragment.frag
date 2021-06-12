#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 fPosition;
in vec3 fNormal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform samplerCube skybox;

uniform vec3 cameraPosition;
uniform vec3 sunColor;

const int nrPointLights = 4;
struct Material {
    vec4 diffuse;
    vec4 specular;
    float shininess;
    float reflectiveness;
    float refractiveness;
};
uniform Material material;

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

uniform vec3 direction_light;

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
    vec4 ambient = texture(texture_diffuse1, TexCoord) * 0.2; //0.2 is ambient factor
    vec4 diffuse = diff * texture(texture_diffuse1, TexCoord);
    vec4 specular =  spec * texture(texture_specular1, TexCoord);
    ambient = vec4(vec3(ambient.x, ambient.y, ambient.z) * attenuation, ambient.w);
    diffuse = vec4(vec3(diffuse.x, diffuse.y, diffuse.z) * attenuation, diffuse.w);
    specular = vec4(vec3(specular.x, specular.y, specular.z) * attenuation, specular.w);
    specular *= material.specular;
    diffuse *= material.diffuse;
    return (ambient + diffuse + specular);
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
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
    specular *= material.specular;
    diffuse *= material.diffuse;
    return (ambient + diffuse + specular);
}


void main()
{
    
    vec4 col = vec4(0.0);
    vec3 viewDir = normalize(cameraPosition - fPosition);
    for(int i = 0; i < nrPointLights; i++) {
        col += CalcPointLight(pointLights[i], fNormal, fPosition, viewDir);
    }
    col += CalcSpotLight(spotLight, fNormal, fPosition, viewDir);
    
    //directional light
    vec4 ambient = texture(texture_diffuse1, TexCoord) * 0.2; //0.2 is ambient factor
    float diff = max(dot(fNormal, direction_light), 0.0);
    vec4 diffuse = diff * texture(texture_diffuse1, TexCoord);
    col += vec4((ambient+diffuse).rgb*sunColor, 1.0);
    
    vec3 primaryRayDir = normalize(fPosition - cameraPosition);
    col += texture(skybox, reflect(primaryRayDir, normalize(fNormal))) * material.reflectiveness;
    col *= texture(skybox, refract(primaryRayDir, normalize(fNormal), 1.0/material.refractiveness));
    if (texture(texture_diffuse1, TexCoord).a < 0.000001) {
        discard;
    }
    FragColor = col;
}
