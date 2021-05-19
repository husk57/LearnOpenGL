#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 fPosition;
out vec3 fNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
    vec3 past = vec3(modelMatrix * vec4(aPos, 1.0));
    fPosition = vec3(modelMatrix * vec4(aPos, 1.0));
    fNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aNormal);
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
