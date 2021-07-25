#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fWorldPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Normal;

void main()
{
    gl_Position = Projection * View * Model * vec4(vPosition, 1.0);
    fWorldPos = vec3(Model * vec4(vPosition, 1.0));
    fNormal = mat3(Normal) * vNormal;
    fTexCoord = vTexCoord;
}