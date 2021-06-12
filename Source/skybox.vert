#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
    TexCoords = aPos;
    gl_Position = (perspectiveMatrix * viewMatrix * vec4(aPos, 1.0)).xyzw;
}
