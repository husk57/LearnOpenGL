
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 fPosition;
in vec3 fNormal;


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int lightType;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform Light light;
uniform float time;
uniform vec3 cameraPosition;

vec3 calcColor(vec3 ldir, int type) { //1 pointlight, 2 spotlight
    vec2 aTex = TexCoord;
    vec3 camVector = normalize(cameraPosition - fPosition);
    
    float diff = max(dot(ldir, fNormal), 0.0);
    vec3 reflectDir = normalize(reflect(-ldir, fNormal));
    float spec = pow(max(dot(camVector, reflectDir), 0.0), material.shininess);
    vec3 ambient  = light.ambient * vec3(texture(material.diffuse, aTex));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, aTex));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, aTex));
    vec3 emission;
    if (vec3(texture(material.specular, aTex)).r < 0.01) {
        emission = texture(material.emission, aTex).rgb;
    } else {
        emission = vec3(0.0);
    }
    
    if (type == 2) {
        float theta = dot(ldir, normalize(-light.direction));
            float epsilon = (light.cutOff - light.outerCutOff);
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
            diffuse  *= intensity;
            specular *= intensity;
            emission *= intensity;
    }
    float distance = length(light.position - fPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    emission *= attenuation;
     
    return diffuse+specular+emission+ambient;
}

void main()
{
    vec3 lightSourceColor = calcColor(normalize(light.position - fPosition), light.lightType);
    FragColor = vec4(lightSourceColor, 1.0);
}
