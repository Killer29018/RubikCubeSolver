#version 330 core

out vec4 f_Colour;

uniform vec3 u_Colour;

void main()
{
    f_Colour = vec4(u_Colour.rgb, 1.0);
}
