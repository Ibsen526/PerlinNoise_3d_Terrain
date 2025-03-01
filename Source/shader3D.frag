#version 330 core

layout(location = 0) out vec4 f_color; //Gibt die Farb und Positionswerte zurück zum Programm

in vec3 v_colors;

void main() //Fragment Shader
{
    f_color = vec4(v_colors, 1.0f); //Zuweisung der Farbe aus Uniform
}