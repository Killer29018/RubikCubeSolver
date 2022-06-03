#version 330 core

layout (location = 0) in vec3 i_Pos;
layout (location = 0) in vec3 i_Normals;

out vec3 v_Normals;
out vec3 v_FragPos;

uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform mat4 u_View;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(i_Pos.xyz, 1.0);

    v_Normals = mat3(transpose(inverse(u_Model))) * i_Normals; // Removed Stretch from model Matrix and apply to normals
    v_FragPos = vec3(u_Model * vec4(i_Pos, 1.0));
}
