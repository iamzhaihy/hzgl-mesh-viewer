#version 410 core

in vec3 fNormal;
in vec2 fTexCoord;
in vec3 fWorldPos;

out vec4 FragColor;

void main()
{
    FragColor = vec4(fNormal, 1);
}