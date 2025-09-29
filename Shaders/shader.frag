#version 330 core

in vec4 vCol;
in vec2 TexCoord;
out vec4 colour;

uniform sampler2D texture1;

void main()
{
    colour = texture(texture1, TexCoord);
}