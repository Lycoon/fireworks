#version 460 core

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec4 particleInfo;
layout(location = 2) in vec4 color;

out vec2 UV;
out vec4 fragmentColor;

uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform mat4 VP;

void main()
{
	float particleSize = particleInfo.w;
	vec3 particleCenter = particleInfo.xyz;
	
	vec3 vertexPosition = particleCenter 
		+ cameraRight * vertices.x * particleSize 
		+ cameraUp * vertices.y * particleSize;

	gl_Position = VP * vec4(vertexPosition, 1.0f);

	UV = vertices.xy + vec2(0.5, 0.5);
	fragmentColor = color;
}
