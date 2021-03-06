#version 460 core

layout(location = 0) in vec3 position; // per vert: position
layout(location = 1) in vec3 color;    // per vert: color

in vec3 bdpos;		                   // per inst: bill board pos
in vec3 rgb;		                   // per inst: colour
in float size;		                   // per inst: size
in float angle;		                   // per inst: angle

out vec3 fragmentColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1);
    fragmentColor = color;
}