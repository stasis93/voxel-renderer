#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 vp;

out vec3 texCoord;

void main()
{
    vec4 pos = vp * vec4(aPos, 1);
    gl_Position = pos.xyww;
    texCoord = aPos;
}
