#version 330 core

layout(location = 0) in vec3 a_position; //a steht für Attribut
layout(location = 1) in vec3 a_colors;

void main() //Vertex Shader
{
	gl_Position = vec4(a_position, 1.0f);
}