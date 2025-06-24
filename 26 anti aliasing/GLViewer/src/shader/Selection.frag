#version 450 core

in vec3 Normal;
in vec3 FragPos;

uniform int elementId;

layout(location = 0) out ivec4 out_ElemInfo;

void main()
{
    out_ElemInfo = ivec4(elementId, 0, 0, 1);
}
