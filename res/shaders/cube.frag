#version 330 core

layout (location = 0) out vec4 f_Colour;
layout (location = 1) out vec4 f_MousePicker;

in vec3 v_Normals;
in vec3 v_FragPos;

uniform vec4 u_Colours[7];
uniform int u_CurrentColour;

uniform vec3 u_QBPosition;
uniform int u_QBFace;

uniform vec3 u_ViewPos;

void main()
{
    f_MousePicker = ivec4(u_QBFace, u_QBPosition); // Mouse Picking

    // Basic Lighting

    const float ambientStrength = 0.1;
    const float specularStrength = 0.1;

    const vec3 lightColour = vec3(0.8);

    vec3 lightPosition = u_ViewPos;

    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 lightDir = normalize(lightPosition - v_FragPos);

    vec3 norm = normalize(u_ViewPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = ambientStrength * lightColour;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColour;

    vec3 result = (ambient + diffuse + specular) * u_Colours[u_CurrentColour].rgb;

    f_Colour = vec4(result, 1.0);
}
