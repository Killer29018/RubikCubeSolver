#version 330 core

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;

layout (location = 0) out vec4 f_Colour;

void main()
{
    f_Colour = texture(u_ScreenTexture, v_TexCoords);
}
