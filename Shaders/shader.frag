#version 330 core

in vec4 vCol;
in vec2 TexCoord;
out vec4 colour;

uniform vec3 lightColour;
uniform sampler2D texture1;

void main()
{
    float ambientStrength = 0.3f;
    vec3 ambient = ambientStrength * lightColour;
    colour = texture(texture1, TexCoord) * vec4(ambient, 1.0);
    
}