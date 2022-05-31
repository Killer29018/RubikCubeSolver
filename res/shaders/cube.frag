#version 330 core

layout (location = 0) out vec4 f_Colour;

uniform vec4 u_Colours[7];
uniform int u_CurrentColour;

void main()
{
    f_Colour = vec4(u_Colours[u_CurrentColour].rgb, 1.0);
}
