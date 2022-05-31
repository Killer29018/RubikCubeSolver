#version 330 core

layout (location = 0) out vec4 f_Colour;
layout (location = 1) out vec4 f_MousePicker;

uniform vec4 u_Colours[7];
uniform int u_CurrentColour;

uniform vec3 u_QBPosition;
uniform int u_QBFace;

void main()
{
    f_Colour = vec4(u_Colours[u_CurrentColour].rgb, 1.0);

    f_MousePicker = vec4(u_QBFace, u_QBPosition);
}
