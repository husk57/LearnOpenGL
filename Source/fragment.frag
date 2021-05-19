
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
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform float time;
uniform vec3 cameraPosition;

void main()
{
    vec2 aTex = TexCoord;// + vec2(0.0, time);
    vec3 camVector = normalize(cameraPosition - fPosition);
    float specularStrength = material.shininess;
    
    vec3 lightDir = normalize(light.position - fPosition);
    float diff = max(dot(lightDir, fNormal), 0.0);
    vec3 reflectDir = normalize(reflect(-lightDir, fNormal));
    float spec = pow(max(dot(camVector, reflectDir), 0.0), material.shininess);
    vec3 ambient  = light.ambient * vec3(texture(material.diffuse, aTex));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, aTex));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, aTex));
    vec3 emission;
    if (vec3(texture(material.specular, aTex)).r < 0.01) {
        emission = texture(material.emission, aTex).rgb; //* (cos(time*20.0) * 0.5 + 0.5);
    } else {
        emission = vec3(0.0);
    }
    FragColor = vec4(ambient+diffuse+specular+emission, 1.0);
}
