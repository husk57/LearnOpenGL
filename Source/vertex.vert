#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 fPosition;
out vec3 fNormal;
out vec3 testA;
out vec3 testB;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
    fPosition = vec3(modelMatrix * vec4(aPos, 1.0));
    fNormal =normalize(transpose(inverse(mat3(modelMatrix))) * aNormal);
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
