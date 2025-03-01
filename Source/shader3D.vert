#version 330 core

layout(location = 0) in vec3 a_position; //a steht für Attribut
layout(location = 1) in vec3 a_colors;

out vec3 v_colors;

uniform mat4 u_mvp;

void main() //Vertex Shader
{
	gl_Position = u_mvp * vec4(a_position, 1.0f);
	v_colors = a_colors;
}