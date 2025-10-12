#version 330 core

out vec4 colour;

uniform vec3 bgColour;

void main()
{
    colour = vec4(bgColour,1.0);
}