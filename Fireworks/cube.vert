#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

out vec3 fragmentColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1);
    fragmentColor = vertexColor;
}