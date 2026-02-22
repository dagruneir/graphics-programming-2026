#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// (todo) 03.X: Add the out variables here
out vec3 Normal;
out vec3 Position;

// (todo) 03.X: Add the uniforms here
uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;


void main()
{
	gl_Position = ViewProjMatrix * WorldMatrix * vec4(VertexPosition, 1.0);
	Normal = vec3(WorldMatrix * vec4(VertexNormal, 0.0));

	// Object space position
	//Position = VertexPosition;

	// World space position
	//Position = vec3(vec4(VertexPosition, 1.0) * WorldMatrix);

	// Clip space position
	Position = vec3(vec4(VertexPosition, 1.0) * WorldMatrix * ViewProjMatrix);
}
